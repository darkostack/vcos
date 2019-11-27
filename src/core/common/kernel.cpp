#include <assert.h>
#include <stdio.h>

#include <vcos/cpu.h>
#include <vcos/instance.h>
#include <vcos/kernel.h>
#include <vcos/stdiobase.h>

#include "common/instance.hpp"
#include "common/kernel.hpp"

namespace vc {

static int sLoopFunctionUndefined = 0;

extern "C" __attribute__((weak)) void loop(void)
{
    sLoopFunctionUndefined = 1;
    return;
}

extern "C" __attribute__((weak)) void setup(void)
{
    return;
}

extern "C" __attribute__((weak)) int main(void)
{
    while (1)
    {
    }
}

void *Kernel::MainThreadFunc(void *aArgs)
{
    (void)aArgs;

    setup();

    loop();

    if (sLoopFunctionUndefined == 0)
    {
        while (1)
        {
            loop();
        }
    }

    main();

    return NULL;
}

void *Kernel::IdleThreadFunc(void *aArgs)
{
    (void)aArgs;

    while (1)
    {
        vcCpuSleep(0);
    }

    return NULL;
}

void Kernel::Init(Instance *aInstance)
{
    mMainKernelPid = mMainThread.Create(mMainThreadStack, sizeof(mMainThreadStack), VCOS_CONFIG_THREAD_PRIORITY_MAIN,
                                        THREAD_FLAGS_CREATE_WOUT_YIELD | THREAD_FLAGS_CREATE_STACKTEST, MainThreadFunc,
                                        static_cast<void *>(aInstance), "main");

    mIdleKernelPid = mIdleThread.Create(mIdleThreadStack, sizeof(mIdleThreadStack), VCOS_CONFIG_THREAD_PRIORITY_IDLE,
                                        THREAD_FLAGS_CREATE_WOUT_YIELD | THREAD_FLAGS_CREATE_STACKTEST, IdleThreadFunc,
                                        static_cast<void *>(aInstance), "idle");
}

extern "C" void vcKernelInit(void)
{
    (void)vcIrqDisable();

    Instance &instance = Instance::Init();

    assert(instance.IsInitialized());

    vcStdioInit((vcInstance *)&instance);

    printf("\r\n\r\nkernel started (version: 0.0.1)\r\n\r\n");

    instance.Get<Kernel>().Init(&instance);

    ThreadScheduler::SwitchContextExit();
}

} // namespace vc
