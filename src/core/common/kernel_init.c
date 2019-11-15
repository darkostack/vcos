#include <stdio.h>
#include <assert.h>

#include <vcos/instance.h>
#include <vcos/stdiobase.h>
#include <vcos/thread.h>

#include "cpu.h"

#include "vcos-core-config.h"

extern int main(void);

vcKernelPid     gMainThreadPid;
static vcThread sMainThread;
static char     sMainStack[VCOS_CONFIG_THREAD_STACKSIZE_MAIN];

vcKernelPid     gIdleThreadPid;
static vcThread sIdleThread;
static char     sIdleStack[VCOS_CONFIG_THREAD_STACKSIZE_IDLE];

void *mainThreadFunc(void *aArg)
{
    main();

    return NULL;
}

void *idleThreadFunc(void *aArg)
{
    (void) aArg;

    while (1) {
        cortexmSleep(0);
    }

    return NULL;
}

void vcKernelInit(void)
{
    (void) irqDisable();

    vcInstance *instance = vcInstanceInit();

    assert(vcInstanceIsInitialized(instance));

    vcStdioInit(instance);

    printf("\r\n\r\nkernel started (version: 0.0.1)\r\n\r\n");

    gMainThreadPid = vcThreadCreate(&sMainThread, sMainStack, sizeof(sMainStack),
                                    VCOS_CONFIG_THREAD_PRIORITY_MAIN,
                                    THREAD_FLAGS_CREATE_WOUT_YIELD | THREAD_FLAGS_CREATE_STACKTEST,
                                    mainThreadFunc, NULL, "main");

    gIdleThreadPid = vcThreadCreate(&sIdleThread, sIdleStack, sizeof(sIdleStack),
                                    VCOS_CONFIG_THREAD_PRIORITY_IDLE,
                                    THREAD_FLAGS_CREATE_WOUT_YIELD | THREAD_FLAGS_CREATE_STACKTEST,
                                    idleThreadFunc, NULL, "idle");

    vcThreadSwitchContextExit();
}
