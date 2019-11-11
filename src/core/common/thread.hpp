#ifndef THREAD_HPP
#define THREAD_HPP

#include <stdint.h>

#include <vcos/thread.h>

#include "vcos-core-config.h"

#include "kernel_defines.h"
#include "kernel_types.h"

#include "common/clist.hpp"
#include "common/cib.hpp"
#include "common/locator.hpp"

#include "utils/bitarithm.h"

#include "cpu_conf.h"

/**
 * Thread default parameters.
 */
#ifndef THREAD_STACKSIZE_DEFAULT
#error THREAD_STACKSIZE_DEFAULT must be defined per CPU
#endif

#ifndef THREAD_STACKSIZE_IDLE
#error THREAD_STACKSIZE_IDLE must be defined per CPU
#endif

#ifndef THREAD_EXTRA_STACKSIZE_PRINTF
#error THREAD_EXTRA_STACKSIZE_PRINTF must be defined per CPU
#endif

#ifndef THREAD_STACKSIZE_MAIN
#define THREAD_STACKSIZE_MAIN (THREAD_STACKSIZE_DEFAULT + THREAD_EXTRA_STACKSIZE_PRINTF)
#endif

#ifndef THREAD_STACKSIZE_LARGE
#define THREAD_STACKSIZE_LARGE (THREAD_STACKSIZE_MEDIUM * 2)
#endif

#ifndef THREAD_STACKSIZE_MEDIUM
#define THREAD_STACKSIZE_MEDIUM THREAD_STACKSIZE_DEFAULT
#endif

#ifndef THREAD_STACKSIZE_SMALL
#define THREAD_STACKSIZE_SMALL (THREAD_STACKSIZE_MEDIUM / 2)
#endif

#ifndef THREAD_STACKSIZE_TINY
#define THREAD_STACKSIZE_TINY (THREAD_STACKSIZE_MEDIUM / 4)
#endif

#ifndef THREAD_STACKSIZE_MINIMUM
#define THREAD_STACKSIZE_MINIMUM (sizeof(Thread))
#endif

#define THREAD_PRIORITY_MIN (SCHED_PRIO_LEVELS - 1)
#define THREAD_PRIORITY_IDLE (THREAD_PRIORITY_MIN)

#ifndef THREAD_PRIORITY_MAIN
#define THREAD_PRIORITY_MAIN (THREAD_PRIORITY_MIN - (SCHED_PRIO_LEVELS / 2))
#endif

/**
 * Optional flags for controlling a threads initial state.
 */
#define THREAD_CREATE_SLEEPING (1)
#define THREAD_AUTO_FREE (2)
#define THREAD_CREATE_WOUT_YIELD (4)
#define THREAD_CREATE_STACKTEST (8)

#define STATUS_ON_RUNQUEUE STATUS_RUNNING
#define STATUS_NOT_FOUND ((ThreadStatus)-1)

#ifndef SCHED_PRIO_LEVELS
#define SCHED_PRIO_LEVELS 16
#endif

namespace vc {

typedef enum {
    STATUS_STOPPED,
    STATUS_SLEEPING,
    STATUS_MUTEX_BLOCKED,
    STATUS_RECEIVED_BLOCKED,
    STATUS_SEND_BLOCKED,
    STATUS_REPLY_BLOCKED,
    STATUS_FLAG_BLOCKED_ANY,
    STATUS_FLAG_BLOCKED_ALL,
    STATUS_MBOX_BLOCKED,
    STATUS_COND_BLOCKED,
    STATUS_RUNNING,
    STATUS_PENDING,
    STATUS_NUMOF
} ThreadStatus;

class ThreadScheduler;

class Thread : public InstanceLocator
{
    friend class ThreadScheduler;

public:
    Thread(Instance &aInstance,
           char *aStack,
           int aStackSize,
           char aPriority,
           int aFlags,
           vcThreadHandlerFunc aFunction,
           void *aArg,
           const char *aName);

    void AddToList(ListNode *aList, Thread *aThread);

    uintptr_t MeasureStackFree(char *aStack);

    KernelPid GetPid(void) { return mPid; }

    const char *GetName(void) { return mName; }

    uint8_t GetPriority(void) { return mPriority; }

    ThreadStatus GetStatus(void) { return mStatus; }

private:
    char *mSp;
    ThreadStatus mStatus;
    uint8_t mPriority;
    KernelPid mPid;
    ClistNode mRqEntry;
    char *mStackStart;
    const char *mName;
    int mStackSize;
};

class ThreadScheduler : public Clist
{
public:
    ThreadScheduler(void)
        : mSchedNumThreads(0)
        , mSchedContextSwitchRequest(0)
        , mSchedActivePid(KERNEL_PID_UNDEF)
        , mRunqueueBitCache(0)
    {
    }

    int Run(void);

    void SetStatus(Thread *aThread, ThreadStatus aStatus);

    void Switch(uint16_t aOtherPrio);

    void SwtchContextExit(void);

    void TaskExit(void);

    Thread *GetSchedThreads(KernelPid aPid) { return mSchedThreads[aPid]; }

    void SetSchedThreads(Thread *aThread, KernelPid aPid) { mSchedThreads[aPid] = aThread; }

    void SetContexSwitchRequest(unsigned int aRequest) { mSchedContextSwitchRequest = aRequest; }

    int GetSchedNumThreads(void) { return mSchedNumThreads; }

    Thread *GetSchedActiveThread(void) { return mSchedActiveThread; }

    KernelPid GetSchedActivePid(void) { return mSchedActivePid; }

    void SchedNumThreadsAdd(void) { mSchedNumThreads++; }

    Thread *GetThread(KernelPid aPid);

    int GetStatus(KernelPid aPid);

    const char *GetName(KernelPid aPid);

    void Sleep(void);

    int Wakeup(KernelPid aPid);

    void Yield(void);

private:
    int mSchedNumThreads;
    unsigned int mSchedContextSwitchRequest;
    Thread *mSchedThreads[KERNEL_PID_LAST + 1];
    Thread *mSchedActiveThread;
    KernelPid mSchedActivePid;
    ClistNode mSchedRunqueues[SCHED_PRIO_LEVELS];
    uint32_t mRunqueueBitCache;
};

} // namespace vc

#endif /* THREAD_HPP */
