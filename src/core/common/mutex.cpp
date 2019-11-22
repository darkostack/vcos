#include <vcos/irq.h>

#include "common/locator-getters.hpp"
#include "common/mutex.hpp"
#include "common/thread.hpp"

#define ENABLE_DEBUG (0)
#include "common/debug.h"

#if ENABLE_DEBUG
#include <inttypes.h>
#endif

namespace vc {

int Mutex::SetLock(int aBlocking)
{
    unsigned state = vcIrqDisable();

    DEBUG("Mutex::SetLock() PID(%" PRIkernel_pid "): mutex in use.\r\n", Get<ThreadScheduler>().GetSchedActivePid());

    if (mQueue.mNext == NULL)
    {
        /* mutex is unlocked */
        mQueue.mNext = MUTEX_LOCKED;

        DEBUG("Mutex::SetLock() PID(%" PRIkernel_pid "): mutex wait early out.\r\n",
              Get<ThreadScheduler>().GetSchedActivePid());

        vcIrqRestore(state);

        return 1;
    }
    else if (aBlocking)
    {
        Thread *me = Get<ThreadScheduler>().GetSchedActiveThread();

        DEBUG("Mutex::SetLock() PID(%" PRIkernel_pid "): adding node to mutex queue prio: %" PRIu32 "\r\n",
              Get<ThreadScheduler>().GetSchedActivePid(), (uint32_t)me->mPriority);

        Get<ThreadScheduler>().SetStatus(me, THREAD_STATUS_MUTEX_BLOCKED);

        if (mQueue.mNext == MUTEX_LOCKED)
        {
            mQueue.mNext        = &me->GetRqEntry();
            mQueue.mNext->mNext = NULL;
        }
        else
        {
            me->AddToList(static_cast<List *>(&mQueue));
        }

        vcIrqRestore(state);

        ThreadScheduler::YieldHigher();

        /* We were woken up by scheduler. Waker removed us from queue.
         * We have the mutex now. */

        return 1;
    }
    else
    {
        vcIrqRestore(state);
        return 0;
    }
}

void Mutex::Unlock(void)
{
    unsigned state = vcIrqDisable();

    DEBUG("Mutex::Unlock() mQueue.mNext: %p pid: %" PRIkernel_pid "\r\n", (void *)mQueue.mNext,
          Get<ThreadScheduler>().GetSchedActivePid());

    if (mQueue.mNext == NULL)
    {
        /* the mutex was not locked */
        vcIrqRestore(state);
        return;
    }

    if (mQueue.mNext == MUTEX_LOCKED)
    {
        mQueue.mNext = NULL;
        /* the mutex was locked and no thread was waiting for it */
        vcIrqRestore(state);
        return;
    }

    List *next = (static_cast<List *>(&mQueue))->RemoveHead();

    Thread *process = Get<ThreadScheduler>().GetThreadPointerFromList(next);

    DEBUG("Mutex::Unlock() waking up waiting thread %" PRIkernel_pid "\r\n", process->mPid);

    Get<ThreadScheduler>().SetStatus(process, THREAD_STATUS_PENDING);

    if (!mQueue.mNext)
    {
        mQueue.mNext = MUTEX_LOCKED;
    }

    uint16_t processPrio = process->mPriority;

    vcIrqRestore(state);

    Get<ThreadScheduler>().Switch(processPrio);
}

void Mutex::UnlockAndSleep(void)
{
    DEBUG("Mutex::UnlockAndSleep() PID(%" PRIkernel_pid "): unlocking mutex. mQueue.mNext: %p, and "
          "taking a nap\r\n",
          Get<ThreadScheduler>().GetSchedActivePid(), (void *)mQueue.mNext);

    unsigned state = vcIrqDisable();

    if (mQueue.mNext)
    {
        if (mQueue.mNext == MUTEX_LOCKED)
        {
            mQueue.mNext = NULL;
        }
        else
        {
            List *next = (static_cast<List *>(&mQueue))->RemoveHead();

            Thread *process = Get<ThreadScheduler>().GetThreadPointerFromList(next);

            DEBUG("Mutex::UnlockAndSleep() PID(%" PRIkernel_pid "): waking up waiter.\r\n", process->mPid);

            Get<ThreadScheduler>().SetStatus(process, THREAD_STATUS_PENDING);

            if (!mQueue.mNext)
            {
                mQueue.mNext = MUTEX_LOCKED;
            }
        }
    }

    DEBUG("Mutex::UnlockAndSleep() PID(%" PRIkernel_pid "): going to sleep.\r\n",
          Get<ThreadScheduler>().GetSchedActivePid());

    Get<ThreadScheduler>().SetStatus(Get<ThreadScheduler>().GetSchedActiveThread(), THREAD_STATUS_SLEEPING);

    vcIrqRestore(state);

    ThreadScheduler::YieldHigher();
}

} // namespace vc
