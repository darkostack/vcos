#include <stdio.h>
#include <assert.h>

#include <vcos/instance.h>
#include <vcos/cli.h>
#include <vcos/msg.h>
#include <vcos/thread.h>

#include "vcos-core-config.h"

vcKernelPid     gTestThreadPid;
static vcThread sTestThread;
static char     sTestStack[VCOS_CONFIG_THREAD_STACKSIZE_MAIN];

void *testThreadFunc(void *aArg)
{
    (void) aArg;

    vcMsg msg;

    msg.mType = 0x20;
    msg.mContent.mPtr = NULL;

    assert(vcMsgSend(&msg, gMainThreadPid));

    while (1) {
        if (vcMsgReceive(&msg)) {
            if (msg.mType == 0x21 && msg.mContent.mPtr == NULL) {
                printf("testThreadFunc(): get an acked from %" PRIkernel_pid "\r\n", msg.mSenderPid);
            }
        }
    }

    return NULL;
}

int main(void)
{
    vcInstance *instance = vcInstanceGet();

    assert(vcInstanceIsInitialized(instance));

    gTestThreadPid =  vcThreadCreate(&sTestThread, sTestStack, sizeof(sTestStack),
                                     VCOS_CONFIG_THREAD_PRIORITY_MAIN,
                                     THREAD_FLAGS_CREATE_WOUT_YIELD | THREAD_FLAGS_CREATE_STACKTEST,
                                     testThreadFunc, NULL, "test");

    vcMsg msg;

    while (1) {
        if (vcMsgReceive(&msg)) {
            if (msg.mType == 0x20 && msg.mContent.mPtr == NULL) {
                printf("mainApp(): got message from %" PRIkernel_pid "\r\n", msg.mSenderPid);
                msg.mType = 0x21;
                msg.mContent.mPtr = NULL;
                assert(vcMsgSend(&msg, msg.mSenderPid));
                break;
            }
        }
    }

    vcCliUartInit(instance);

    vcCliUartRun();

    /* Should not reach here */
    assert(0);

    return 0;
}
