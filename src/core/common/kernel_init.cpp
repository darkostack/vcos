#include <stdio.h>
#include <assert.h>

#include <vcos/instance.h>
#include <vcos/stdiobase.h>

#include "common/instance.hpp"
#include "common/thread.hpp"

#include "cpu.h"

#include "vcos-core-config.h"

namespace vc {

extern "C" __attribute__((weak)) int main(void)
{
    while (1) {}
}

void *mainThreadFunc(void *aArg)
{
    main();
    return NULL;
}

void *idleThreadFunc(void *aArg)
{
    (void) aArg;

    while (1) {
        vcCpuSleep(0);
    }

    return NULL;
}

static char sMainStack[VCOS_CONFIG_THREAD_STACKSIZE_MAIN];
static char sIdleStack[VCOS_CONFIG_THREAD_STACKSIZE_IDLE];

extern "C" void vcKernelInit(void)
{
    (void) vcIrqDisable();

    Instance &instance = Instance::Init();

    assert(instance.IsInitialized());

    vcStdioInit((vcInstance *)&instance);

    printf("\r\n\r\nkernel started (version: 0.0.1)\r\n\r\n");

    Thread mainThread(sMainStack, sizeof(sMainStack),
                      VCOS_CONFIG_THREAD_PRIORITY_MAIN,
                      THREAD_FLAGS_CREATE_WOUT_YIELD | THREAD_FLAGS_CREATE_STACKTEST,
                      mainThreadFunc, NULL, "main");

    Thread idleThread(sIdleStack, sizeof(sIdleStack),
                      VCOS_CONFIG_THREAD_PRIORITY_IDLE,
                      THREAD_FLAGS_CREATE_WOUT_YIELD | THREAD_FLAGS_CREATE_STACKTEST,
                      idleThreadFunc, NULL, "idle");

    ThreadScheduler::SwitchContextExit();
}

} // namespace vc
