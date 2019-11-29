#ifndef INSTANCE_HPP
#define INSTANCE_HPP

#include "vcos-core-config.h"

#include "common/code_utils.hpp"
#include "common/kernel.hpp"
#include "common/thread.hpp"
#include "common/timer.hpp"

#include "net/message.hpp"
#include "net/notifier.hpp"
#include "net/tasklet.hpp"
#include "net/timer.hpp"
#include "net/utils/heap.hpp"

#include "utils/isrpipe.hpp"

typedef struct vcInstance
{
} vcInstance;

namespace vc {

class Instance : public vcInstance
{
public:
    static Instance &Init(void);

    static Instance &Get(void);

    bool IsInitialized(void) const { return mIsInitialized; }

    void Reset(void);

    void Finalize(void);

    void HeapFree(void *aPointer) { mHeap.Free(aPointer); }

    void *HeapCAlloc(size_t aCount, size_t aSize) { return mHeap.CAlloc(aCount, aSize); }

    Net::Utils::Heap &GetHeap(void) { return mHeap; }

    template <typename Type> inline Type &Get(void);

private:
    explicit Instance(void);

    void AfterInit(void);

    Kernel mKernel;

    ThreadScheduler mThreadScheduler;

    TimerScheduler mTimerScheduler;

    Utils::UartIsrpipe mUartIsrpipe;

    Net::MessagePool mMessagePool;

    Net::Notifier mNotifier;

    Net::TaskletScheduler mTaskletScheduler;

    Net::TimerMilliScheduler mTimerMilliScheduler;

    Net::TimerMicroScheduler mTimerMicroScheduler;

    Net::Utils::Heap mHeap;

    bool mIsInitialized;
};

template <> inline Kernel &Instance::Get(void)
{
    return mKernel;
}

template <> inline ThreadScheduler &Instance::Get(void)
{
    return mThreadScheduler;
}

template <> inline TimerScheduler &Instance::Get(void)
{
    return mTimerScheduler;
}

template <> inline Utils::UartIsrpipe &Instance::Get(void)
{
    return mUartIsrpipe;
}

template <> inline Net::MessagePool &Instance::Get(void)
{
    return mMessagePool;
}

template <> inline Net::Notifier &Instance::Get(void)
{
    return mNotifier;
}

template <> inline Net::TaskletScheduler &Instance::Get(void)
{
    return mTaskletScheduler;
}

template <> inline Net::TimerMilliScheduler &Instance::Get(void)
{
    return mTimerMilliScheduler;
}

template <> inline Net::TimerMicroScheduler &Instance::Get(void)
{
    return mTimerMicroScheduler;
}

} // namespace vc

#endif /* INSTANCE_HPP */
