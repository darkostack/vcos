#include <stdio.h>

#include <vcos/instance.h>
#include <vcos/thread.h>

#include "cpu.h"

#include "common/instance.hpp"
#include "common/thread.hpp"

extern int main(void);

namespace vc {

static char gMainStack[THREAD_STACKSIZE_MAIN + THREAD_EXTRA_STACKSIZE_PRINTF];
static char gIdleStack[THREAD_STACKSIZE_IDLE];

extern "C" void *mainThreadFunc(void *aArg)
{
    (void) aArg;

    main();

    return NULL;
}

extern "C" void *idleThreadFunc(void *aArg)
{
    (void) aArg;

    printf("kernel enter IDLE state\n");

    while (1) {
        cortexmSleep(0);
    }

    return NULL;
}

extern "C" void vcKernelInit(vcInstance *aInstance)
{
    Instance &instance = *static_cast<Instance *>(aInstance);

    (void) irqDisable();

    printf("\n\nkernel started (version: 0.0.1)\n\n");

    Thread mainThread(instance, gMainStack, sizeof(gMainStack),
                      THREAD_PRIORITY_MAIN,
                      THREAD_CREATE_WOUT_YIELD | THREAD_CREATE_STACKTEST,
                      mainThreadFunc, NULL, "main");

    Thread idleThread(instance, gIdleStack, sizeof(gIdleStack),
                      THREAD_PRIORITY_IDLE,
                      THREAD_CREATE_WOUT_YIELD | THREAD_CREATE_STACKTEST,
                      idleThreadFunc, NULL, "idle");

    vcThreadSwitchContextExit();
}

} // namespace vc
