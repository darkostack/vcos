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
    vcMsg reply;

    msg.mType = 0x20;
    msg.mContent.mPtr = NULL;

    vcMsgSendReceive(&msg, &reply, gMainThreadPid);

    if (reply.mType == 0x21 && reply.mContent.mPtr == NULL)
    {
        printf("testThreadFunc(): got reply message\r\n");
    }

    while (1) {
        if (vcMsgReceive(&msg)) {
            /* put this thread to receive blocking state */
        }
    }

    return NULL;
}

int main(void)
{
    vcInstance *instance = vcInstanceGet();

    assert(vcInstanceIsInitialized(instance));

    gTestThreadPid = vcThreadCreate(&sTestThread, sTestStack, sizeof(sTestStack),
                                    VCOS_CONFIG_THREAD_PRIORITY_MAIN,
                                    THREAD_FLAGS_CREATE_WOUT_YIELD | THREAD_FLAGS_CREATE_STACKTEST,
                                    testThreadFunc, NULL, "test");

    vcMsg msg;
    vcMsg reply;

    while (1)
    {
        if (vcMsgReceive(&msg))
        {
            if (msg.mType == 0x20 && msg.mContent.mPtr == NULL)
            {
                printf("mainApp(): got message from %" PRIkernel_pid "\r\n", msg.mSenderPid);
                reply.mType = 0x21;
                reply.mContent.mPtr = NULL;
                vcMsgReply(&msg, &reply);
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
