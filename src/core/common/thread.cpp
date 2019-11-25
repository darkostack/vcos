#include <vcos/irq.h>
#include <vcos/thread.h>
#include <vcos/timer.h>

#include "common/locator-getters.hpp"
#include "common/thread.hpp"

#define ENABLE_DEBUG (0)
#include "common/debug.h"

/**
 * Note: this global pointer will be use in SVC and PendSV handler assembly code
 * to get the current active TCB (Thread Control Block).
 */
volatile void *gSchedActiveThread;

namespace vc {

vcKernelPid Thread::Create(char *              aStack,
                           int                 aStackSize,
                           char                aPriority,
                           int                 aFlags,
                           vcThreadHandlerFunc aFunction,
                           void *              aArg,
                           const char *        aName)
{
    if (aPriority >= VCOS_CONFIG_THREAD_SCHED_PRIO_LEVELS)
    {
        DEBUG("Thread::Create() priority level must less than %u\r\n", VCOS_CONFIG_THREAD_SCHED_PRIO_LEVELS);
        return KERNEL_PID_UNDEF;
    }

    mWaitData = NULL;
    mMsgArray = NULL;

    (static_cast<Cib *>(&mMsgQueue))->Init(0);

    int totalStacksize = aStackSize;

    /* align the stack on 16/32bit boundary */
    uintptr_t misalignment = reinterpret_cast<uintptr_t>(aStack) % 8;

    if (misalignment)
    {
        misalignment = 8 - misalignment;
        aStack += misalignment;
        aStackSize -= misalignment;
    }

    /* make room for the thread control block */
    aStackSize -= sizeof(Thread);

    /* round down the stacksize to multiple of Thread aligments (usually 16/32 bit) */
    aStackSize -= aStackSize % 8;

    if (aStackSize < 0)
    {
        DEBUG("Thread::Create() stack size is too small!\r\n");
    }

    /* allocate out thread control block at the top of our stackspace */
    Thread *cb = (Thread *)(aStack + aStackSize);

    if (aFlags & THREAD_FLAGS_CREATE_STACKTEST)
    {
        /* assign each int of the stack the value of it's address */
        uintptr_t *stackmax = reinterpret_cast<uintptr_t *>(aStack + aStackSize);
        uintptr_t *stackp   = reinterpret_cast<uintptr_t *>(aStack);

        while (stackp < stackmax)
        {
            *stackp = reinterpret_cast<uintptr_t>(stackp);
            stackp++;
        }
    }
    else
    {
        /* create stack guard */
        *(uintptr_t *)aStack = reinterpret_cast<uintptr_t>(aStack);
    }

    unsigned state = vcIrqDisable();

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
        DEBUG("Thread::Create() too many threads!\r\n");
        vcIrqRestore(state);
        return KERNEL_PID_UNDEF;
    }

    Get<ThreadScheduler>().SetSchedThreads(cb, pid);

    cb->mPid = pid;
    cb->mSp  = StackInit(aFunction, aArg, aStack, aStackSize);

    cb->mStackStart = aStack;
    cb->mStackSize  = totalStacksize;
    cb->mName       = aName;

    cb->mPriority = aPriority;
    cb->mStatus   = THREAD_STATUS_STOPPED;

    cb->mRqEntry.mNext    = NULL;
    cb->mMsgWaiters.mNext = NULL;

    Get<ThreadScheduler>().SchedNumThreadsAddOne();

    DEBUG("Thread::Create() created thread %s. PID %" PRIkernel_pid ". Priority: %u\r\n", aName, cb->mPid, aPriority);

    if (aFlags & THREAD_FLAGS_CREATE_SLEEPING)
    {
        Get<ThreadScheduler>().SetStatus(cb, THREAD_STATUS_SLEEPING);
    }
    else
    {
        Get<ThreadScheduler>().SetStatus(cb, THREAD_STATUS_PENDING);

        if (!(aFlags & THREAD_FLAGS_CREATE_WOUT_YIELD))
        {
            vcIrqRestore(state);
            Get<ThreadScheduler>().Switch(aPriority);
            return cb->mPid;
        }
    }

    vcIrqRestore(state);

    return cb->mPid;
}

void Thread::AddToList(List *aList)
{
    assert(mStatus < THREAD_STATUS_ON_RUNQUEUE);

    uint16_t myPrio  = mPriority;
    List *   newNode = static_cast<List *>(&mRqEntry);

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
    aList->mNext   = newNode;
}

uintptr_t Thread::MeasureStackFree(void)
{
    uintptr_t *stackp = reinterpret_cast<uintptr_t *>(mStackStart);

    /* assume that the comparison fails before or after end of stack */
    /* assume that the stack grows "downwards" */

    while (*stackp == reinterpret_cast<uintptr_t>(stackp))
    {
        stackp++;
    }

    uintptr_t spacefree = reinterpret_cast<uintptr_t>(stackp) - reinterpret_cast<uintptr_t>(mStackStart);

    return spacefree;
}

extern "C" void vcThreadTaskExit(void)
{
    Instance &instance = Instance::Get();
    instance.Get<ThreadScheduler>().TaskExit();
}

char *Thread::StackInit(vcThreadHandlerFunc aFunction, void *aArg, void *aStackStart, int aStackSize)
{
    uint32_t *stk;
    stk = (uint32_t *)(reinterpret_cast<uintptr_t>(aStackStart) + aStackSize);

    /* adjust to 32 bit boundary by clearing the last two bits in the address */
    stk = (uint32_t *)((reinterpret_cast<uintptr_t>(stk)) & ~((uint32_t)0x3));

    /* stack start marker */
    stk--;
    *stk = STACK_MARKER;

    /* make sure the stack is double word aligned (8 bytes) */
    /* This is required in order to conform with Procedure Call Standard for the
     * ARM® Architecture (AAPCS) */
    /* http://infocenter.arm.com/help/topic/com.arm.doc.ihi0042e/IHI0042E_aapcs.pdf */
    if ((reinterpret_cast<uintptr_t>(stk) & 0x7) != 0)
    {
        /* add a single word padding */
        --stk;
        *stk = ~((uint32_t)STACK_MARKER);
    }

    /* ****************************** */
    /* Automatically popped registers */
    /* ****************************** */

    /* The following eight stacked registers are popped by the hardware upon
     * return from exception. (bx instruction in context_restore) */

    /* xPSR - initial status register */
    stk--;
    *stk = (uint32_t)INITIAL_XPSR;
    /* pc - initial program counter value := thread entry function */
    stk--;
    *stk = reinterpret_cast<uintptr_t>(aFunction);
    /* lr - contains the return address when the thread exits */
    stk--;
    *stk = reinterpret_cast<uintptr_t>(vcThreadTaskExit);
    /* r12 */
    stk--;
    *stk = 0;
    /* r3 - r1 */
    for (int i = 3; i >= 1; i--)
    {
        stk--;
        *stk = i;
    }
    /* r0 - contains the thread function parameter */
    stk--;
    *stk = reinterpret_cast<uintptr_t>(aArg);

    /* ************************* */
    /* Manually popped registers */
    /* ************************* */

    /* The following registers are not handled by hardware in return from
     * exception, but manually by context_restore.
     * For the Cortex-M3 and Cortex-M4 we write them continuously onto the stack
     * as they can be read/written continuously by stack instructions. */

    /* r11 - r4 */
    for (int i = 11; i >= 4; i--)
    {
        stk--;
        *stk = i;
    }

    /* exception return code  - return to task-mode process stack pointer */
    stk--;
    *stk = (uint32_t)EXCEPT_RET_TASK_MODE;

    /* The returned stack pointer will be aligned on a 32 bit boundary not on a
     * 64 bit boundary because of the odd number of registers above (8+9).
     * This is not a problem since the initial stack pointer upon process entry
     * _will_ be 64 bit aligned (because of the cleared bit 9 in the stacked
     * xPSR and aligned stacking of the hardware-handled registers). */

    return (char *)stk;
}

int ThreadScheduler::Run(void)
{
    SetContexSwitchRequest(0);

    Thread *activeThread = static_cast<Thread *>(GetSchedActiveThread());

    int nextrq = bitarithmLsb(mRunqueueBitCache);

    Thread *nextThread = GetThreadPointerFromList(static_cast<List *>((mSchedRunqueues[nextrq].mNext)->mNext));

    DEBUG("ThreadScheduler::Run() active thread: %" PRIkernel_pid ", next thread: %" PRIkernel_pid ".\r\n",
          (vcKernelPid)((activeThread == NULL) ? KERNEL_PID_UNDEF : activeThread->mPid), nextThread->mPid);

    if (activeThread == nextThread)
    {
        DEBUG("ThreadScheduler::Run() done, mSchedActiveThread was not changed.\r\n");
        return 0;
    }

    uint32_t now = vcTimerNow().mTicks32;

    if (activeThread)
    {
        if (activeThread->mStatus == THREAD_STATUS_RUNNING)
        {
            activeThread->mStatus = THREAD_STATUS_PENDING;
        }

        vcSchedStat *activeStat = &mSchedPidList[activeThread->mPid];

        if (activeStat->mLastStart)
        {
            activeStat->mRuntimeTicks += now - activeStat->mLastStart;
        }
    }

    vcSchedStat *nextStat = &mSchedPidList[nextThread->mPid];

    nextStat->mLastStart = now;
    nextStat->mSchedules++;

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
            DEBUG("ThreadScheduler::SetStatus() adding thread %" PRIkernel_pid " to runqueue %u.\r\n", aThread->mPid,
                  aThread->mPriority);

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

    DEBUG("ThreadScheduler::Switch() active pid=%" PRIkernel_pid " prio=%" PRIu16 " on_runqueue=%i "
          ", other_prio=%" PRIu16 ".\r\n",
          activeThread->mPid, currentPrio, onRunqueue, aOtherPrio);

    if (!onRunqueue || (currentPrio > aOtherPrio))
    {
        if (vcIrqIsIn())
        {
            DEBUG("ThreadScheduler::Switch() setting mSchedContextSwitchRequest.\r\n");
            SetContexSwitchRequest(1);
        }
        else
        {
            DEBUG("ThreadScheduler::Switch() yielding immediately.\r\n");
            YieldHigher();
        }
    }
    else
    {
        DEBUG("ThreadScheduler::Switch() continuing without yield.\r\n");
    }
}

void ThreadScheduler::TaskExit(void)
{
    DEBUG("ThreadScheduler::TaskExit() ending thread %" PRIkernel_pid "...\r\n", GetSchedActiveThread()->mPid);

    (void)vcIrqDisable();

    SetSchedThreads(NULL, GetSchedActivePid());

    SchedNumThreadsRemoveOne();

    SetStatus(GetSchedActiveThread(), THREAD_STATUS_STOPPED);

    SetSchedActiveThread(NULL);

    SwitchContextExit();
}

void ThreadScheduler::SetSchedActiveThread(Thread *aThread)
{
    mSchedActiveThread = aThread;
    gSchedActiveThread = mSchedActiveThread;
}

Thread *ThreadScheduler::GetThread(vcKernelPid aPid)
{
    if (vcThreadPidIsValid(aPid))
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
    if (vcIrqIsIn())
    {
        return;
    }

    unsigned state = vcIrqDisable();

    SetStatus(GetSchedActiveThread(), THREAD_STATUS_SLEEPING);

    vcIrqRestore(state);

    YieldHigher();
}

int ThreadScheduler::Wakeup(vcKernelPid aPid)
{
    DEBUG("ThreadScheduler::Wakeup() trying to wakeup PID %" PRIkernel_pid "...\r\n", aPid);

    unsigned state = vcIrqDisable();

    Thread *otherThread = GetThread(aPid);

    if (!otherThread)
    {
        DEBUG("ThreadScheduler::Wakeup() thread does not exist!\r\n");
    }
    else if (otherThread->mStatus == THREAD_STATUS_SLEEPING)
    {
        DEBUG("ThreadScheduler::Wakeup() thread is sleeping.\r\n");

        SetStatus(otherThread, THREAD_STATUS_RUNNING);

        vcIrqRestore(state);

        Switch(otherThread->mPriority);

        return 1;
    }
    else
    {
        DEBUG("ThreadScheduler::Wakeup() thread is not sleeping!\r\n");
    }

    vcIrqRestore(state);

    return (int)THREAD_STATUS_NOT_FOUND;
}

void ThreadScheduler::Yield(void)
{
    unsigned state = vcIrqDisable();

    Thread *me = GetSchedActiveThread();

    if (me->mStatus >= THREAD_STATUS_ON_RUNQUEUE)
    {
        mSchedRunqueues[me->mPriority].LeftPopRightPush();
    }

    vcIrqRestore(state);

    YieldHigher();
}

Thread *ThreadScheduler::GetThreadPointerFromList(List *aList)
{
    return container_of(aList, Thread, mRqEntry);
}

int ThreadScheduler::IsrStackUsage(void)
{
    uint32_t *ptr = &_sstack;

    while (((*ptr) == STACK_CANARY_WORD) && (ptr < &_estack))
    {
        ++ptr;
    }

    ptrdiff_t num_used_words = &_estack - ptr;
    return num_used_words * sizeof(*ptr);
}

void *ThreadScheduler::IsrStackPointer(void)
{
    void *msp = vcCpuGetMsp();
    return msp;
}

void *ThreadScheduler::IsrStackStart(void)
{
    return reinterpret_cast<void *>(&_sstack);
}

void ThreadScheduler::SwitchContextExit(void)
{
    vcCpuSwitchContextExit();
}

extern "C" void vcCpuIsrEnd(void)
{
    Instance &instance = Instance::Get();

    if (instance.Get<ThreadScheduler>().GetContextSwitchRequest())
    {
        ThreadScheduler::YieldHigher();
    }
}

void ThreadScheduler::YieldHigher(void)
{
    vcCpuTriggerPendSV();
}

const char *ThreadScheduler::ThreadStatusToString(vcThreadStatus aStatus)
{
    const char *retval;

    switch (aStatus)
    {
    case THREAD_STATUS_RUNNING:
        retval = "running";
        break;

    case THREAD_STATUS_PENDING:
        retval = "pending";
        break;

    case THREAD_STATUS_STOPPED:
        retval = "stopped";
        break;

    case THREAD_STATUS_SLEEPING:
        retval = "sleeping";
        break;

    case THREAD_STATUS_MUTEX_BLOCKED:
        retval = "bl mutex";
        break;

    case THREAD_STATUS_RECEIVE_BLOCKED:
        retval = "bl rx";
        break;

    case THREAD_STATUS_SEND_BLOCKED:
        retval = "bl send";
        break;

    case THREAD_STATUS_REPLY_BLOCKED:
        retval = "bl reply";
        break;

    default:
        retval = "unknown";
        break;
    }

    return retval;
}

void ThreadScheduler::ProcessStatus(void)
{
    const char queuedName[] = {'_', 'Q'};

    int overallStackSize = 0;
    int overallStackUsed = 0;

    printf("\tpid | "
           "%-21s| "
           "%-9sQ | pri "
           "| stack  ( used) | base addr  | current     "
           "| runtime  | switches"
           "\r\n",
           "name", "state");

    int   isrUsage    = IsrStackUsage();
    void *isrStart    = IsrStackStart();
    void *isrStackPtr = IsrStackPointer();

    printf("\t  - | isr_stack            | -        - |"
           "   - | %6i (%5i) | %10p | %10p\r\n",
           CPU_ISR_STACKSIZE, isrUsage, isrStart, isrStackPtr);

    overallStackSize += CPU_ISR_STACKSIZE;

    if (isrUsage > 0)
    {
        overallStackUsed += isrUsage;
    }

    uint64_t rtSum = 0;

    for (vcKernelPid i = KERNEL_PID_FIRST; i <= KERNEL_PID_LAST; i++)
    {
        Thread *p = (Thread *)mSchedThreads[i];

        if (p != NULL)
        {
            rtSum += mSchedPidList[i].mRuntimeTicks;
        }
    }

    for (vcKernelPid i = KERNEL_PID_FIRST; i <= KERNEL_PID_LAST; i++)
    {
        Thread *p = (Thread *)mSchedThreads[i];

        if (p != NULL)
        {
            vcThreadStatus state = p->mStatus;

            const char *sname = ThreadStatusToString(state);

            const char *queued = &queuedName[(int)(state >= THREAD_STATUS_ON_RUNQUEUE)];

            int stackSize = p->mStackSize;

            overallStackSize += stackSize;

            stackSize -= p->MeasureStackFree();

            overallStackUsed += stackSize;

            /* multiply with 100 for percentage and to avoid floats/doubles */
            uint64_t runtimeTicks = mSchedPidList[i].mRuntimeTicks * 100;
            unsigned runtimeMajor = (runtimeTicks / rtSum);
            unsigned runtimeMinor = ((runtimeTicks % rtSum) * 1000) / rtSum;

            unsigned switches = mSchedPidList[i].mSchedules;

            printf("\t%3" PRIkernel_pid " | %-20s"
                   " | %-8s %.1s | %3i"
                   " | %6i (%5i) | %10p | %10p "
                   " | %2d.%03d%% |  %8u"
                   "\r\n",
                   p->mPid, p->mName, sname, queued, p->mPriority, p->mStackSize, stackSize, (void *)p->mStackStart,
                   (void *)p->mSp, runtimeMajor, runtimeMinor, switches);
        }
    }

    printf("\t%5s %-21s|%13s%6s %6i (%5i)\r\n", "|", "SUM", "|", "|", overallStackSize, overallStackUsed);
}

} // namespace vc
