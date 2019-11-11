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

Thread::Thread(Instance &aInstance,
               char *aStack,
               int aStackSize,
               char aPriority,
               int aFlags,
               vcThreadHandlerFunc aFunction,
               void *aArg,
               const char *aName)
    : InstanceLocator(aInstance)
{
    if (aPriority >= VCOS_CONFIG_THREAD_SCHED_PRIO_LEVELS) {
        DEBUG("Thread::Thread() priority level must less than %u\n", VCOS_CONFIG_THREAD_SCHED_PRIO_LEVELS);
        return;
    }

    int totalStacksize = aStackSize;

    /* align the stack on 16/32bit boundary */
    uintptr_t misalignment = (uintptr_t)aStack % ALIGN_OF(void *);

    if (misalignment) {
        misalignment = ALIGN_OF(void *) - misalignment;
        aStack += misalignment;
        aStackSize -= misalignment;
    }

    /* make room for the thread control block */
    aStackSize -= sizeof(Thread);

    /* round down the stacksize to multiple of Thread aligments (usually 16/32
     * bit) */
    aStackSize -= aStackSize % ALIGN_OF(Thread);

    if (aStackSize < 0) {
        DEBUG("Thread::Thread() stack size is too small!\n");
    }

    /* allocate out thread control block at the top of our stackspace */
    Thread *cb = (Thread *)(aStack + aStackSize);

    if (aFlags & THREAD_FLAGS_CREATE_STACKTEST) {
        /* assign each int of the stack the value of it's address */
        uintptr_t *stackmax = (uintptr_t *)(aStack + aStackSize);
        uintptr_t *stackp = (uintptr_t *)aStack;

        while (stackp < stackmax) {
            *stackp = (uintptr_t) stackp;
            stackp++;
        }
    } else {
        /* create stack guard */
        *(uintptr_t *)aStack = (uintptr_t)aStack;
    }

    unsigned state = irqDisable();

    KernelPid pid = KERNEL_PID_UNDEF;

    for (KernelPid i = KERNEL_PID_FIRST; i <= KERNEL_PID_LAST; ++i) {
        if (Get<ThreadScheduler>().GetSchedThreads(i) == NULL) {
            pid = i;
            break;
        }
    }

    if (pid == KERNEL_PID_UNDEF) {
        DEBUG("Thread::Thread() too many threads!\n");
        irqRestore(state);
        return;
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

    Get<ThreadScheduler>().SchedNumThreadsAdd();

    DEBUG("Thread::Thread() created thread %s. PID %" PRIkernel_pid ". Priority: %u\n",
          aName, cb->mPid, aPriority);

    if (aFlags & THREAD_FLAGS_CREATE_SLEEPING) {
        Get<ThreadScheduler>().SetStatus(cb, THREAD_STATUS_SLEEPING);
    } else {
        Get<ThreadScheduler>().SetStatus(cb, THREAD_STATUS_PENDING);
        if (!(aFlags & THREAD_FLAGS_CREATE_WOUT_YIELD)) {
            irqRestore(state);
            Get<ThreadScheduler>().Switch(aPriority);
            return;
        }
    }

    irqRestore(state);

    return;
}

void Thread::AddToList(ListNode *aList, Thread *aThread)
{
    assert(aThread->mStatus < THREAD_STATUS_ON_RUNQUEUE);

    uint16_t myPrio = aThread->mPriority;
    ListNode *newNode = static_cast<ListNode *>(&aThread->mRqEntry);

    while (aList->mNext) {
        Thread *listEntry = container_of(static_cast<ClistNode *>(aList->mNext), Thread, mRqEntry);
        if (listEntry->mPriority > myPrio) {
            break;
        }
        aList = aList->mNext;
    }

    newNode->mNext = aList->mNext;
    aList->mNext = newNode;
}

uintptr_t Thread::MeasureStackFree(char *aStack)
{
    uintptr_t *stackp = (uintptr_t *)aStack;
    /* assume that the comparison fails before or after end of stack */
    /* assume that the stack grows "downwards" */
    while (*stackp == (uintptr_t) stackp) {
        stackp++;
    }
    uintptr_t spacefree = (uintptr_t)stackp - (uintptr_t)aStack;
    return spacefree;
}

int ThreadScheduler::Run(void)
{
    mSchedContextSwitchRequest = 0;

    Thread *activeThread = static_cast<Thread *>(mSchedActiveThread);

    int nextrq = bitarithmLsb(mRunqueueBitCache);
    Thread *nextThread = container_of(static_cast<ClistNode *>(mSchedRunqueues[nextrq].mNext->mNext), Thread, mRqEntry);

    DEBUG("ThreadScheduler::Run() active thread: %" PRIkernel_pid ", next thread: %" PRIkernel_pid ".\n",
          (KernelPid)((activeThread == NULL) ? KERNEL_PID_UNDEF : activeThread->mPid),
          nextThread->mPid);

    if (activeThread == nextThread) {
        DEBUG("ThreadScheduler::Run() done, mSchedActiveThread was not changed.\n");
        return 0;
    }

    if (activeThread) {
        if (activeThread->mStatus == THREAD_STATUS_RUNNING) {
            activeThread->mStatus = THREAD_STATUS_PENDING;
        }
    }

    nextThread->mStatus = THREAD_STATUS_RUNNING;
    mSchedActivePid = nextThread->mPid;
    mSchedActiveThread = nextThread;

    /* update global current TCB pointer to current active thread */
    gSchedActiveThread = mSchedActiveThread;

    DEBUG("ThreadScheduler::Run() done, changed mSchedActiveThread.\n");

    return 1;
}

void ThreadScheduler::SetStatus(Thread *aThread, ThreadStatus aStatus)
{
    if (aStatus >= THREAD_STATUS_ON_RUNQUEUE) {
        if (!(aThread->mStatus >= THREAD_STATUS_ON_RUNQUEUE)) {
            DEBUG("ThreadScheduler::SetStatus() adding thread %" PRIkernel_pid " to runqueue %u.\n",
                  aThread->mPid, aThread->mPriority);
            RightPush(&mSchedRunqueues[aThread->mPriority], &(aThread->mRqEntry));
            mRunqueueBitCache |= 1 << aThread->mPriority;
        }
    } else {
        if (aThread->mStatus >= THREAD_STATUS_ON_RUNQUEUE) {
            DEBUG("ThreadScheduler::SetStatus() removing thread %" PRIkernel_pid " from runqueue %u.\n",
                  aThread->mPid, aThread->mPriority);
            LeftPop(&mSchedRunqueues[aThread->mPriority]);
            if (!mSchedRunqueues[aThread->mPriority].mNext) {
                mRunqueueBitCache &= ~(1 << aThread->mPriority);
            }
        }
    }

    aThread->mStatus = aStatus;
}

void ThreadScheduler::Switch(uint16_t aOtherPrio)
{
    Thread *activeThread = mSchedActiveThread;
    uint16_t currentPrio = activeThread->mPriority;
    int onRunqueue = (activeThread->mStatus >= THREAD_STATUS_ON_RUNQUEUE);

    DEBUG("ThreadScheduler::Switch() active pid=%" PRIkernel_pid" prio=%" PRIu16 " on_runqueue=%i "
          ", other_prio=%" PRIu16 ".\n",
          activeThread->mPid, currentPrio, onRunqueue, aOtherPrio);

    if (!onRunqueue || (currentPrio > aOtherPrio)) {
        if (irqIsIn()) {
            DEBUG("ThreadScheduler::Switch() setting mSchedContextSwitchRequest.\n");
            mSchedContextSwitchRequest = 1;
        } else {
            DEBUG("ThreadScheduler::Switch() yielding immediately.\n");
            vcThreadYieldHigher();
        }
    } else {
        DEBUG("ThreadScheduler::Switch() continuing without yield.\n");
    }
}

void ThreadScheduler::TaskExit(void)
{
    DEBUG("ThreadScheduler::TaskExit() ending thread %" PRIkernel_pid "...\n", mSchedActiveThread->mPid);

    (void) irqDisable();
    mSchedThreads[mSchedActivePid] = NULL;
    mSchedNumThreads--;

    SetStatus(mSchedActiveThread, THREAD_STATUS_STOPPED);

    mSchedActiveThread = NULL;
    vcThreadSwitchContextExit();
}

Thread *ThreadScheduler::GetThread(KernelPid aPid)
{
    if (pidIsValid(aPid)) {
        return mSchedThreads[aPid];
    }
    return NULL;
}

int ThreadScheduler::GetStatus(KernelPid aPid)
{
    Thread *t = GetThread(aPid);
    return t ? static_cast<int>(t->mStatus) : static_cast<int>(THREAD_STATUS_NOT_FOUND);
}

const char *ThreadScheduler::GetName(KernelPid aPid)
{
    Thread *t = GetThread(aPid);
    return t ? t->mName : NULL;
}

void ThreadScheduler::Sleep(void)
{
    if (irqIsIn()) {
        return;
    }
    unsigned state = irqDisable();
    SetStatus(mSchedActiveThread, THREAD_STATUS_SLEEPING);
    irqRestore(state);
    vcThreadYieldHigher();
}

int ThreadScheduler::Wakeup(KernelPid aPid)
{
    DEBUG("ThreadScheduler::Wakeup() trying to wakeup PID %" PRIkernel_pid "...\n", aPid);

    unsigned state = irqDisable();

    Thread *otherThread = GetThread(aPid);

    if (!otherThread) {
        DEBUG("ThreadScheduler::Wakeup() thread does not exist!\n");
    } else if (otherThread->mStatus == THREAD_STATUS_SLEEPING) {
        DEBUG("ThreadScheduler::Wakeup() thread is sleeping.\n");

        SetStatus(otherThread, THREAD_STATUS_RUNNING);
        irqRestore(state);
        Switch(otherThread->mPriority);

        return 1;
    } else {
        DEBUG("ThreadScheduler::Wakeup() thread is not sleeping!\n");
    }

    irqRestore(state);
    return (int)THREAD_STATUS_NOT_FOUND;
}

void ThreadScheduler::Yield(void)
{
    unsigned state = irqDisable();
    Thread *me = mSchedActiveThread;
    if (me->mStatus >= THREAD_STATUS_ON_RUNQUEUE) {
        LeftPopRightPush(&mSchedRunqueues[me->mPriority]);
    }
    irqRestore(state);
    vcThreadYieldHigher();
}

} // namespace vc
