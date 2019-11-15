#include <vcos/irq.h>
#include <vcos/thread.h>

#include "common/thread.hpp"
#include "common/locator-getters.hpp"

#define ENABLE_DEBUG (0)
#include "common/debug.h"

/**
 * Note: this global pointer will be use in cortexm thread_arch.c assembly code
 * to get the current active TCB.
 */
volatile void *gSchedActiveThread;

namespace vc {

vcKernelPid Thread::Create(char *aStack,
                           int aStackSize,
                           char aPriority,
                           int aFlags,
                           vcThreadHandlerFunc aFunction,
                           void *aArg,
                           const char *aName)
{
    if (aPriority >= VCOS_CONFIG_THREAD_SCHED_PRIO_LEVELS)
    {
        DEBUG("Thread::Thread() priority level must less than %u\r\n", VCOS_CONFIG_THREAD_SCHED_PRIO_LEVELS);
        return KERNEL_PID_UNDEF;
    }

    mWaitData = NULL;
    (static_cast<Cib *>(&mMsgQueue))->Init(0);
    mMsgArray = NULL;

    int totalStacksize = aStackSize;

    /* align the stack on 16/32bit boundary */
    uintptr_t misalignment = (uintptr_t)aStack % ALIGN_OF(void *);

    if (misalignment)
    {
        misalignment = ALIGN_OF(void *) - misalignment;
        aStack += misalignment;
        aStackSize -= misalignment;
    }

    /* make room for the thread control block */
    aStackSize -= sizeof(Thread);

    /* round down the stacksize to multiple of Thread aligments (usually 16/32
     * bit) */
    aStackSize -= aStackSize % ALIGN_OF(Thread);

    if (aStackSize < 0)
    {
        DEBUG("Thread::Thread() stack size is too small!\r\n");
    }

    /* allocate out thread control block at the top of our stackspace */
    Thread *cb = (Thread *)(aStack + aStackSize);

    if (aFlags & THREAD_FLAGS_CREATE_STACKTEST)
    {
        /* assign each int of the stack the value of it's address */
        uintptr_t *stackmax = (uintptr_t *)(aStack + aStackSize);
        uintptr_t *stackp = (uintptr_t *)aStack;

        while (stackp < stackmax)
        {
            *stackp = (uintptr_t) stackp;
            stackp++;
        }
    }
    else
    {
        /* create stack guard */
        *(uintptr_t *)aStack = (uintptr_t)aStack;
    }

    unsigned state = irqDisable();

    vcKernelPid pid = KERNEL_PID_UNDEF;

    for (vcKernelPid i = KERNEL_PID_FIRST; i <= KERNEL_PID_LAST; ++i)
    {
        if (Get<ThreadScheduler>().GetSchedThreads(i) == NULL)
        {
            pid = i;
            break;
        }
    }

    if (pid == KERNEL_PID_UNDEF)
    {
        DEBUG("Thread::Thread() too many threads!\r\n");
        irqRestore(state);
        return KERNEL_PID_UNDEF;
    }

    Get<ThreadScheduler>().SetSchedThreads(cb, pid); 

    cb->mPid = pid;
    cb->mSp = vcThreadStackInit(aFunction, aArg, aStack, aStackSize);

    cb->mStackStart = aStack;
    cb->mStackSize = totalStacksize;
    cb->mName = aName;

    cb->mPriority = aPriority;
    cb->mStatus = THREAD_STATUS_STOPPED;

    cb->mRqEntry.mNext = NULL;
    cb->mMsgWaiters.mNext = NULL;

    Get<ThreadScheduler>().SchedNumThreadsAddOne();

    DEBUG("Thread::Thread() created thread %s. PID %" PRIkernel_pid ". Priority: %u\r\n",
          aName, cb->mPid, aPriority);

    if (aFlags & THREAD_FLAGS_CREATE_SLEEPING)
    {
        Get<ThreadScheduler>().SetStatus(cb, THREAD_STATUS_SLEEPING);
    }
    else
    {
        Get<ThreadScheduler>().SetStatus(cb, THREAD_STATUS_PENDING);

        if (!(aFlags & THREAD_FLAGS_CREATE_WOUT_YIELD))
        {
            irqRestore(state);
            Get<ThreadScheduler>().Switch(aPriority);
            return cb->mPid;
        }
    }

    irqRestore(state);

    return cb->mPid;
}

void Thread::AddToList(List *aList)
{
    assert(mStatus < THREAD_STATUS_ON_RUNQUEUE);

    uint16_t myPrio = mPriority;
    List *newNode = static_cast<List *>(&mRqEntry);

    while (aList->mNext)
    {
        Thread *listEntry = Get<ThreadScheduler>().GetThreadPointerFromList(static_cast<List *>(aList->mNext));

        if (listEntry->mPriority > myPrio)
        {
            break;
        }
        aList = static_cast<List *>(aList->mNext);
    }

    newNode->mNext = aList->mNext;
    aList->mNext = newNode;
}

uintptr_t Thread::MeasureStackFree(char *aStack)
{
    uintptr_t *stackp = (uintptr_t *)aStack;

    /* assume that the comparison fails before or after end of stack */
    /* assume that the stack grows "downwards" */

    while (*stackp == (uintptr_t) stackp)
    {
        stackp++;
    }

    uintptr_t spacefree = (uintptr_t)stackp - (uintptr_t)aStack;

    return spacefree;
}

int ThreadScheduler::Run(void)
{
    SetContexSwitchRequest(0);

    Thread *activeThread = static_cast<Thread *>(GetSchedActiveThread());

    int nextrq = bitarithmLsb(mRunqueueBitCache);
    Thread *nextThread = GetThreadPointerFromList(static_cast<List *>((mSchedRunqueues[nextrq].mNext)->mNext));

    DEBUG("ThreadScheduler::Run() active thread: %" PRIkernel_pid ", next thread: %" PRIkernel_pid ".\r\n",
          (vcKernelPid)((activeThread == NULL) ? KERNEL_PID_UNDEF : activeThread->mPid),
          nextThread->mPid);

    if (activeThread == nextThread)
    {
        DEBUG("ThreadScheduler::Run() done, mSchedActiveThread was not changed.\r\n");
        return 0;
    }

    if (activeThread)
    {
        if (activeThread->mStatus == THREAD_STATUS_RUNNING)
        {
            activeThread->mStatus = THREAD_STATUS_PENDING;
        }
    }

    nextThread->mStatus = THREAD_STATUS_RUNNING;

    SetSchedActivePid(nextThread->mPid);
    SetSchedActiveThread(nextThread);

    DEBUG("ThreadScheduler::Run() done, changed mSchedActiveThread.\r\n");

    return 1;
}

void ThreadScheduler::SetStatus(Thread *aThread, vcThreadStatus aStatus)
{
    if (aStatus >= THREAD_STATUS_ON_RUNQUEUE)
    {
        if (!(aThread->mStatus >= THREAD_STATUS_ON_RUNQUEUE))
        {
            DEBUG("ThreadScheduler::SetStatus() adding thread %" PRIkernel_pid " to runqueue %u.\r\n",
                  aThread->mPid, aThread->mPriority);

            mSchedRunqueues[aThread->mPriority].RightPush(static_cast<Clist *>(&(aThread->mRqEntry)));

            mRunqueueBitCache |= 1 << aThread->mPriority;
        }
    }
    else
    {
        if (aThread->mStatus >= THREAD_STATUS_ON_RUNQUEUE)
        {
            DEBUG("ThreadScheduler::SetStatus() removing thread %" PRIkernel_pid " from runqueue %u.\r\n",
                  aThread->mPid, aThread->mPriority);

            mSchedRunqueues[aThread->mPriority].LeftPop();

            if (!mSchedRunqueues[aThread->mPriority].mNext)
            {
                mRunqueueBitCache &= ~(1 << aThread->mPriority);
            }
        }
    }

    aThread->mStatus = aStatus;
}

void ThreadScheduler::Switch(uint16_t aOtherPrio)
{
    Thread *activeThread = GetSchedActiveThread();
    uint16_t currentPrio = activeThread->mPriority;
    int onRunqueue = (activeThread->mStatus >= THREAD_STATUS_ON_RUNQUEUE);

    DEBUG("ThreadScheduler::Switch() active pid=%" PRIkernel_pid" prio=%" PRIu16 " on_runqueue=%i "
          ", other_prio=%" PRIu16 ".\r\n", activeThread->mPid, currentPrio, onRunqueue, aOtherPrio);

    if (!onRunqueue || (currentPrio > aOtherPrio))
    {
        if (irqIsIn())
        {
            DEBUG("ThreadScheduler::Switch() setting mSchedContextSwitchRequest.\r\n");

            SetContexSwitchRequest(1);
        }
        else
        {
            DEBUG("ThreadScheduler::Switch() yielding immediately.\r\n");

            vcThreadYieldHigher();
        }
    }
    else
    {
        DEBUG("ThreadScheduler::Switch() continuing without yield.\r\n");
    }
}

void ThreadScheduler::TaskExit(void)
{
    DEBUG("ThreadScheduler::TaskExit() ending thread %" PRIkernel_pid "...\r\n",
          GetSchedActiveThread()->mPid);

    (void) irqDisable();

    SetSchedThreads(NULL, GetSchedActivePid());
    SchedNumThreadsRemoveOne();

    SetStatus(GetSchedActiveThread(), THREAD_STATUS_STOPPED);

    SetSchedActiveThread(NULL);
    vcThreadSwitchContextExit();
}

void ThreadScheduler::SetSchedActiveThread(Thread *aThread)
{
    mSchedActiveThread = aThread;
    gSchedActiveThread = mSchedActiveThread;
}

Thread *ThreadScheduler::GetThread(vcKernelPid aPid)
{
    if (vcPidIsValid(aPid))
    {
        return GetSchedThreads(aPid);
    }
    return NULL;
}

int ThreadScheduler::GetStatus(vcKernelPid aPid)
{
    Thread *t = GetThread(aPid);
    return t ? static_cast<int>(t->mStatus) : static_cast<int>(THREAD_STATUS_NOT_FOUND);
}

const char *ThreadScheduler::GetName(vcKernelPid aPid)
{
    Thread *t = GetThread(aPid);
    return t ? t->mName : NULL;
}

void ThreadScheduler::Sleep(void)
{
    if (irqIsIn())
    {
        return;
    }

    unsigned state = irqDisable();

    SetStatus(GetSchedActiveThread(), THREAD_STATUS_SLEEPING);

    irqRestore(state);

    vcThreadYieldHigher();
}

int ThreadScheduler::Wakeup(vcKernelPid aPid)
{
    DEBUG("ThreadScheduler::Wakeup() trying to wakeup PID %" PRIkernel_pid "...\r\n", aPid);

    unsigned state = irqDisable();

    Thread *otherThread = GetThread(aPid);

    if (!otherThread)
    {
        DEBUG("ThreadScheduler::Wakeup() thread does not exist!\r\n");
    }
    else if (otherThread->mStatus == THREAD_STATUS_SLEEPING)
    {
        DEBUG("ThreadScheduler::Wakeup() thread is sleeping.\r\n");

        SetStatus(otherThread, THREAD_STATUS_RUNNING);

        irqRestore(state);

        Switch(otherThread->mPriority);

        return 1;
    }
    else
    {
        DEBUG("ThreadScheduler::Wakeup() thread is not sleeping!\r\n");
    }

    irqRestore(state);

    return (int)THREAD_STATUS_NOT_FOUND;
}

void ThreadScheduler::Yield(void)
{
    unsigned state = irqDisable();

    Thread *me = GetSchedActiveThread();

    if (me->mStatus >= THREAD_STATUS_ON_RUNQUEUE)
    {
        mSchedRunqueues[me->mPriority].LeftPopRightPush();
    }

    irqRestore(state);

    vcThreadYieldHigher();
}

Thread *ThreadScheduler::GetThreadPointerFromList(List *aList)
{
    return container_of(static_cast<Clist *>(aList), Thread, mRqEntry);
}

} // namespace vc
