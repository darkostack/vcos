#include <stdio.h>
#include <assert.h>

#include <vcos/cli.h>

#include "common/instance.hpp"
#include "common/thread.hpp"
#include "common/msg.hpp"

namespace vc {

static char gTestStack[VCOS_CONFIG_THREAD_STACKSIZE_MAIN];

extern "C" void *testThreadFunc(void *aArg)
{
    (void) aArg;

    Msg msg;
    Msg reply;

    msg.mType = 0x20;
    msg.mContent.mPtr = NULL;

    msg.SendReceive(&reply, 1);

    if (reply.mType == 0x21 && reply.mContent.mPtr == NULL)
    {
        printf("testThreadFunc(): got reply message\r\n");
    }

    while (1) {
        if (msg.Receive()) {
            /* put this thread to receive blocking state */
        }
    }

    return NULL;
}

int mainApp(void *aArg)
{
    Instance &instance = *static_cast<Instance *>(aArg);

    assert(instance.IsInitialized());

    vcThread testThread;

    (static_cast<Thread *>(&testThread))->Create(gTestStack,
            sizeof(gTestStack),
            VCOS_CONFIG_THREAD_PRIORITY_MAIN,
            THREAD_FLAGS_CREATE_WOUT_YIELD | THREAD_FLAGS_CREATE_STACKTEST,
            testThreadFunc,
            NULL,
            "test");

    Msg msg;
    Msg reply;

    while (1)
    {
        if (msg.Receive())
        {
            if (msg.mType == 0x20 && msg.mContent.mPtr == NULL)
            {
                printf("mainApp(): got message from %" PRIkernel_pid "\r\n", msg.mSenderPid);
                reply.mType = 0x21;
                reply.mContent.mPtr = NULL;
                msg.Reply(&reply);
                break;
            }
        }
    }

    vcCliUartInit((vcInstance *)&instance);

    vcCliUartRun();

    /* Should not reach here */
    assert(0);

    return 0;
}

} // namespace vc
