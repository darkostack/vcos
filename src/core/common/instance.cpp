#include <stdint.h>

#include "common/instance.hpp"
#include "common/new.hpp"

namespace vc {

// Define the raw storage use for nesx instance (in single-instance case).
vcDEFINE_ALIGNED_VAR(gInstanceRaw, sizeof(Instance), uint64_t);

Instance::Instance(void)
    : mThreadScheduler()
    , mTimerScheduler(*this)
    , mUartIsrpipe()
    , mMessagePool()
    , mTaskletScheduler()
    , mTimerMilliScheduler()
    , mTimerMicroScheduler()
    , mHeap()
    , mIsInitialized(false)
{
}

Instance &Instance::Init(void)
{
    Instance *instance = &Get();

    VerifyOrExit(instance->mIsInitialized == false);

    instance = new (&gInstanceRaw) Instance();

    instance->AfterInit();

exit:
    return *instance;
}

Instance &Instance::Get(void)
{
    void *instance = &gInstanceRaw;

    return *static_cast<Instance *>(instance);
}

void Instance::Reset(void)
{
}

void Instance::AfterInit(void)
{
    mIsInitialized = true;
}

void Instance::Finalize(void)
{
    VerifyOrExit(mIsInitialized == true);

    mIsInitialized = false;

exit:
    return;
}

} // namespace vc
