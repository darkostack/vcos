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
#include "common/msg.hpp"

#include "utils/bitarithm.h"

#include "cpu_conf.h"

/**
 * Optional flags for controlling a threads initial state.
 */
#define THREAD_FLAGS_CREATE_SLEEPING (1)
#define THREAD_FLAGS_AUTO_FREE (2)
#define THREAD_FLAGS_CREATE_WOUT_YIELD (4)
#define THREAD_FLAGS_CREATE_STACKTEST (8)

#define THREAD_STATUS_ON_RUNQUEUE THREAD_STATUS_RUNNING
#define THREAD_STATUS_NOT_FOUND ((ThreadStatus)-1)

namespace vc {

typedef enum {
    THREAD_STATUS_STOPPED,
    THREAD_STATUS_SLEEPING,
    THREAD_STATUS_MUTEX_BLOCKED,
    THREAD_STATUS_RECEIVE_BLOCKED,
    THREAD_STATUS_SEND_BLOCKED,
    THREAD_STATUS_REPLY_BLOCKED,
    THREAD_STATUS_FLAG_BLOCKED_ANY,
    THREAD_STATUS_FLAG_BLOCKED_ALL,
    THREAD_STATUS_MBOX_BLOCKED,
    THREAD_STATUS_COND_BLOCKED,
    THREAD_STATUS_RUNNING,
    THREAD_STATUS_PENDING,
    THREAD_STATUS_NUMOF
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

    char *GetStackPointer(void) { return mSp; }

    void SetStackPointer(char *aStackPtr) { mSp = aStackPtr; }

    int GetStackSize(void) { return mStackSize; }

    void SetStackSize(int aStackSize) { mStackSize = aStackSize; }

    char *GetStackStart(void) { return mStackStart; }

    void SetStackStart(char *aStackStart) { mStackStart = aStackStart; };

    void AddToList(ListNode *aList);

    uintptr_t MeasureStackFree(char *aStack);

    KernelPid GetPid(void) { return mPid; }

    void SetPid(KernelPid aPid) { mPid = aPid; }

    const char *GetName(void) { return mName; }

    void SetName(const char *aName) { mName = aName; }

    uint8_t GetPriority(void) { return mPriority; }

    void SetPriority(uint8_t aPriority) { mPriority = aPriority; }

    ThreadStatus GetStatus(void) { return mStatus; }

    void SetStatus(ThreadStatus aStatus) { mStatus = aStatus; }

    ClistNode &GetRqEntry(void) { return mRqEntry; }

    Cib &GetMsgQueue(void) { return mMsgQueue; }

    void SetWaitData(void *aData) { mWaitData = aData; }

    void *GetWaitData(void) { return mWaitData; }

    ListNode &GetMsgWaiters(void) { return mMsgWaiters; }

    ListNode *GetMsgWaitersNext(void) { return mMsgWaiters.mNext; }

    Msg &GetMsgArray(uint16_t aIndex) { return mMsgArray[aIndex]; }

    void SetMsgArray(Msg *aArray) { mMsgArray = aArray; }

    int HasMsgQueued(void) { return (mMsgArray != NULL); }

private:
    char *mSp;
    ThreadStatus mStatus;
    uint8_t mPriority;
    KernelPid mPid;
    ClistNode mRqEntry;
    void *mWaitData;
    ListNode mMsgWaiters;
    Cib mMsgQueue;
    Msg *mMsgArray;
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

    unsigned int GetContextSwitchRequest(void) { return mSchedContextSwitchRequest; }

    void SetContexSwitchRequest(unsigned int aRequest) { mSchedContextSwitchRequest = aRequest; }

    int GetSchedNumThreads(void) { return mSchedNumThreads; }

    Thread *GetSchedActiveThread(void) { return mSchedActiveThread; }

    void SetSchedActiveThread(Thread *aThread);

    KernelPid GetSchedActivePid(void) { return mSchedActivePid; }

    void SetSchedActivePid(KernelPid aPid) { mSchedActivePid = aPid; }

    void SchedNumThreadsAddOne(void) { mSchedNumThreads++; }

    void SchedNumThreadsRemoveOne(void) { mSchedNumThreads--; }

    Thread *GetThread(KernelPid aPid);

    int GetStatus(KernelPid aPid);

    const char *GetName(KernelPid aPid);

    void Sleep(void);

    int Wakeup(KernelPid aPid);

    void Yield(void);

    Thread *GetThreadPointerFromList(ListNode *aList);

private:
    int mSchedNumThreads;
    unsigned int mSchedContextSwitchRequest;
    Thread *mSchedThreads[KERNEL_PID_LAST + 1];
    Thread *mSchedActiveThread;
    KernelPid mSchedActivePid;
    ClistNode mSchedRunqueues[VCOS_CONFIG_THREAD_SCHED_PRIO_LEVELS];
    uint32_t mRunqueueBitCache;
};

} // namespace vc

#endif /* THREAD_HPP */
