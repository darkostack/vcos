#ifndef KERNEL_HPP
#define KERNEL_HPP

#include "vcos-core-config.h"

#include "common/locator.hpp"
#include "common/thread.hpp"

namespace vc {

class Kernel : public InstanceLocator
{
public:
    explicit Kernel(void)
        : mMainThread()
        , mIdleThread()
        , mMainKernelPid(KERNEL_PID_UNDEF)
        , mIdleKernelPid(KERNEL_PID_UNDEF)
    {
    }

    void Init(Instance *aInstance);

    vcKernelPid GetMainThreadPid(void) { return mMainKernelPid; }

    vcKernelPid GetIdleThreadPid(void) { return mIdleKernelPid; }

private:
    Thread mMainThread;
    Thread mIdleThread;

    vcKernelPid mMainKernelPid;
    vcKernelPid mIdleKernelPid;

    char mMainThreadStack[VCOS_CONFIG_THREAD_STACKSIZE_MAIN];
    char mIdleThreadStack[VCOS_CONFIG_THREAD_STACKSIZE_IDLE];

    static void *MainThreadFunc(void *aArgs);
    static void *IdleThreadFunc(void *aArgs);
};

} // namespace vc

#endif /* KERNEL_HPP */
