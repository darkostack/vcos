#include <vcos/irq.h>

#include "common/mutex.hpp"

#define ENABLE_DEBUG (0)
#include "common/debug.h"

#if ENABLE_DEBUG
#include <inttypes.h>
#endif

namespace vc {

int Mutex::SetLock(int aBlocking)
{
    unsigned state = irqDisable();

    DEBUG("Mutex::SetLock() PID(%" PRIkernel_pid "): mutex in use.\n",
          GetSchedActivePid());

    if (mQueue.mNext == NULL) {
        /* mutex is unlocked */
        mQueue.mNext = MUTEX_LOCKED;
        DEBUG("Mutex::SetLock() PID(%" PRIkernel_pid "): mutex wait eary out.\n",
              GetSchedActivePid());
        irqRestore(state);
        return 1;
    } else if (aBlocking) {
        Thread *me = GetSchedActiveThread();
        DEBUG("Mutex::SetLock() PID(%" PRIkernel_pid "): adding node to mutex queue prio: %" PRIu32 "\n",
              GetSchedActivePid(), (uint32_t)me->GetPriority());
        SetStatus(me, THREAD_STATUS_MUTEX_BLOCKED);
        if (mQueue.mNext == MUTEX_LOCKED) {
            mQueue.mNext = (ListNode *)&me->GetRqEntry();
            mQueue.mNext->mNext = NULL;
        } else {
            me->AddToList(&mQueue);
        }
        irqRestore(state);
        vcThreadYieldHigher();
        /* We were woken up by scheduler. Waker removed us from queue.
         * We have the mutex now. */
        return 1;
    } else {
        irqRestore(state);
        return 0;
    }
}

void Mutex::Unlock(void)
{
    unsigned state = irqDisable();

    DEBUG("Mutex::Unlock() mQueue.mNext: %p pid: %" PRIkernel_pid "\n",
          (void *)mQueue.mNext, GetSchedActivePid());

    if (mQueue.mNext == NULL) {
        /* the mutex was not locked */
        irqRestore(state);
        return;
    }

    if (mQueue.mNext == MUTEX_LOCKED) {
        mQueue.mNext = NULL;
        /* the mutex was locked and no thread was waiting for it */
        irqRestore(state);
        return;
    }

    ListNode *next = RemoveHead(&mQueue);

    Thread *process = GetThreadPointerFromList(next);

    DEBUG("Mutex::Unlock() waking up waiting thread %" PRIkernel_pid "\n",
          process->GetPid());

    SetStatus(process, THREAD_STATUS_PENDING);

    if (!mQueue.mNext) {
        mQueue.mNext = MUTEX_LOCKED;
    }

    uint16_t processPrio = process->GetPriority();
    irqRestore(state);
    Switch(processPrio);
}

void Mutex::UnlockAndSleep(void)
{
    DEBUG("Mutex::UnlockAndSleep() PID(%" PRIkernel_pid "): unlocking mutex. mQueue.mNext: %p, and "
          "taking a nap\n", GetSchedActivePid(), (void *)mQueue.mNext);

    unsigned state = irqDisable();

    if (mQueue.mNext) {
        if (mQueue.mNext == MUTEX_LOCKED) {
            mQueue.mNext = NULL;
        } else {
            ListNode *next = RemoveHead(&mQueue);
            Thread *process = GetThreadPointerFromList(next);
            DEBUG("Mutex::UnlockAndSleep() PID(%" PRIkernel_pid "): waking up waiter.\n", process->GetPid());
            SetStatus(process, THREAD_STATUS_PENDING);
            if (!mQueue.mNext) {
                mQueue.mNext = MUTEX_LOCKED;
            }
        }
    }

    DEBUG("Mutex::UnlockAndSleep() PID(%" PRIkernel_pid "): going to sleep.\n", GetSchedActivePid());
    SetStatus(GetSchedActiveThread(), THREAD_STATUS_SLEEPING);
    irqRestore(state);
    vcThreadYieldHigher();
}

} // namespace vc
