#ifndef THREAD_HPP
#define THREAD_HPP

#include <stdint.h>

#include <vcos/thread.h>

#include "vcos-core-config.h"

#include "common/clist.hpp"
#include "common/cib.hpp"
#include "common/locator.hpp"
#include "common/msg.hpp"

#include "utils/bitarithm.h"

#include "cpu_conf.h"

namespace vc {

class ThreadScheduler;

class Thread : public vcThread, public InstanceLocator
{
    friend class ThreadScheduler;

public:
    vcKernelPid Create(char *aStack,
                       int aStackSize,
                       char aPriority,
                       int aFlags,
                       vcThreadHandlerFunc aFunction,
                       void *aArg,
                       const char *aName);

    void AddToList(List *aList);

    uintptr_t MeasureStackFree(char *aStack);

    Clist &GetRqEntry(void) { return *static_cast<Clist *>(&mRqEntry); }

    Cib &GetMsgQueue(void) { return *static_cast<Cib *>(&mMsgQueue); }

    List &GetMsgWaiters(void) { return *static_cast<List *>(&mMsgWaiters); }

    List *GetMsgWaitersNext(void) { return static_cast<List *>(mMsgWaiters.mNext); }

    Msg &GetMsgArray(uint16_t aIndex) { return *static_cast<Msg *>(&mMsgArray[aIndex]); }

    int HasMsgQueued(void) { return (mMsgArray != NULL); }
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

    void SetStatus(Thread *aThread, vcThreadStatus aStatus);

    void Switch(uint16_t aOtherPrio);

    void SwtchContextExit(void);

    void TaskExit(void);

    Thread *GetSchedThreads(vcKernelPid aPid) { return mSchedThreads[aPid]; }

    void SetSchedThreads(Thread *aThread, vcKernelPid aPid) { mSchedThreads[aPid] = aThread; }

    unsigned int GetContextSwitchRequest(void) { return mSchedContextSwitchRequest; }

    void SetContexSwitchRequest(unsigned int aRequest) { mSchedContextSwitchRequest = aRequest; }

    int GetSchedNumThreads(void) { return mSchedNumThreads; }

    Thread *GetSchedActiveThread(void) { return mSchedActiveThread; }

    void SetSchedActiveThread(Thread *aThread);

    vcKernelPid GetSchedActivePid(void) { return mSchedActivePid; }

    void SetSchedActivePid(vcKernelPid aPid) { mSchedActivePid = aPid; }

    void SchedNumThreadsAddOne(void) { mSchedNumThreads++; }

    void SchedNumThreadsRemoveOne(void) { mSchedNumThreads--; }

    Thread *GetThread(vcKernelPid aPid);

    int GetStatus(vcKernelPid aPid);

    const char *GetName(vcKernelPid aPid);

    void Sleep(void);

    int Wakeup(vcKernelPid aPid);

    void Yield(void);

    Thread *GetThreadPointerFromList(List *aList);

private:
    int mSchedNumThreads;
    unsigned int mSchedContextSwitchRequest;
    Thread *mSchedThreads[KERNEL_PID_LAST + 1];
    Thread *mSchedActiveThread;
    vcKernelPid mSchedActivePid;
    Clist mSchedRunqueues[VCOS_CONFIG_THREAD_SCHED_PRIO_LEVELS];
    uint32_t mRunqueueBitCache;
};

} // namespace vc

#endif /* THREAD_HPP */
