#include <stdio.h>
#include <assert.h>

#include <vcos/instance.h>
#include <vcos/stdiobase.h>
#include <vcos/thread.h>

#include "cpu.h"

#include "common/instance.hpp"
#include "common/thread.hpp"

extern int main(void);

namespace vc {

static char gMainStack[VCOS_CONFIG_THREAD_STACKSIZE_MAIN];
static char gIdleStack[VCOS_CONFIG_THREAD_STACKSIZE_IDLE];

extern "C" void *mainThreadFunc(void *aArg)
{
    (void) aArg;

    main();

    return NULL;
}

extern "C" void *idleThreadFunc(void *aArg)
{
    (void) aArg;

    while (1) {
        cortexmSleep(0);
    }

    return NULL;
}

extern "C" void vcKernelInit(void)
{
    (void) irqDisable();

    Instance &instance = Instance::InitSingle();

    assert(instance.IsInitialized());

    vcStdioInit((vcInstance *)&instance);

    printf("\n\nkernel started (version: 0.0.1)\n\n");

    Thread mainThread(instance, gMainStack, sizeof(gMainStack),
                      VCOS_CONFIG_THREAD_PRIORITY_MAIN,
                      THREAD_FLAGS_CREATE_WOUT_YIELD | THREAD_FLAGS_CREATE_STACKTEST,
                      mainThreadFunc, NULL, "main");

    Thread idleThread(instance, gIdleStack, sizeof(gIdleStack),
                      VCOS_CONFIG_THREAD_PRIORITY_IDLE,
                      THREAD_FLAGS_CREATE_WOUT_YIELD | THREAD_FLAGS_CREATE_STACKTEST,
                      idleThreadFunc, NULL, "idle");

    vcThreadSwitchContextExit();
}

} // namespace vc
