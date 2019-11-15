#include <vcos/irq.h>

#include "common/locator-getters.hpp"
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

    DEBUG("Mutex::SetLock() PID(%" PRIkernel_pid "): mutex in use.\r\n",
          Get<ThreadScheduler>().GetSchedActivePid());

    if (mQueue.GetNext() == NULL)
    {
        /* mutex is unlocked */
        mQueue.SetNext(MUTEX_LOCKED);

        DEBUG("Mutex::SetLock() PID(%" PRIkernel_pid "): mutex wait early out.\r\n",
              Get<ThreadScheduler>().GetSchedActivePid());

        irqRestore(state);

        return 1;
    }
    else if (aBlocking)
    {
        Thread *me = Get<ThreadScheduler>().GetSchedActiveThread();

        DEBUG("Mutex::SetLock() PID(%" PRIkernel_pid "): adding node to mutex queue prio: %" PRIu32 "\r\n",
              Get<ThreadScheduler>().GetSchedActivePid(), (uint32_t)me->GetPriority());

        Get<ThreadScheduler>().SetStatus(me, THREAD_STATUS_MUTEX_BLOCKED);

        if (mQueue.GetNext() == MUTEX_LOCKED)
        {
            mQueue.SetNext((List *)&me->GetRqEntry());
            (mQueue.GetNext())->SetNext(NULL);
        }
        else
        {
            me->AddToList(&mQueue);
        }

        irqRestore(state);

        vcThreadYieldHigher();

        /* We were woken up by scheduler. Waker removed us from queue.
         * We have the mutex now. */

        return 1;
    }
    else
    {
        irqRestore(state);
        return 0;
    }
}

void Mutex::Unlock(void)
{
    unsigned state = irqDisable();

    DEBUG("Mutex::Unlock() mQueue.GetNext(): %p pid: %" PRIkernel_pid "\r\n",
          (void *)mQueue.GetNext(), Get<ThreadScheduler>().GetSchedActivePid());

    if (mQueue.GetNext() == NULL)
    {
        /* the mutex was not locked */
        irqRestore(state);
        return;
    }

    if (mQueue.GetNext() == MUTEX_LOCKED)
    {
        mQueue.SetNext(NULL);
        /* the mutex was locked and no thread was waiting for it */
        irqRestore(state);
        return;
    }

    List *next = mQueue.RemoveHead();

    Thread *process = Get<ThreadScheduler>().GetThreadPointerFromList(next);

    DEBUG("Mutex::Unlock() waking up waiting thread %" PRIkernel_pid "\r\n",
          process->GetPid());

    Get<ThreadScheduler>().SetStatus(process, THREAD_STATUS_PENDING);

    if (!mQueue.GetNext())
    {
        mQueue.SetNext(MUTEX_LOCKED);
    }

    uint16_t processPrio = process->GetPriority();
    irqRestore(state);
    Get<ThreadScheduler>().Switch(processPrio);
}

void Mutex::UnlockAndSleep(void)
{
    DEBUG("Mutex::UnlockAndSleep() PID(%" PRIkernel_pid "): unlocking mutex. mQueue.GetNext(): %p, and "
          "taking a nap\r\n", Get<ThreadScheduler>().GetSchedActivePid(), (void *)mQueue.GetNext());

    unsigned state = irqDisable();

    if (mQueue.GetNext())
    {
        if (mQueue.GetNext() == MUTEX_LOCKED)
        {
            mQueue.SetNext(NULL);
        }
        else
        {
            List *next = mQueue.RemoveHead();

            Thread *process = Get<ThreadScheduler>().GetThreadPointerFromList(next);

            DEBUG("Mutex::UnlockAndSleep() PID(%" PRIkernel_pid "): waking up waiter.\r\n", process->GetPid());

            Get<ThreadScheduler>().SetStatus(process, THREAD_STATUS_PENDING);

            if (!mQueue.GetNext())
            {
                mQueue.SetNext(MUTEX_LOCKED);
            }
        }
    }

    DEBUG("Mutex::UnlockAndSleep() PID(%" PRIkernel_pid "): going to sleep.\r\n",
          Get<ThreadScheduler>().GetSchedActivePid());

    Get<ThreadScheduler>().SetStatus(Get<ThreadScheduler>().GetSchedActiveThread(), THREAD_STATUS_SLEEPING);

    irqRestore(state);

    vcThreadYieldHigher();
}

} // namespace vc
