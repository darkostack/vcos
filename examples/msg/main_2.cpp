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
    Instance &instance = *static_cast<Instance *>(aArg);

    Msg msg(instance);
    Msg reply(instance);

    msg.SetType(0x20);
    msg.SetContentPtr(NULL);

    msg.SendReceive(&reply, 1);

    if (reply.GetType() == 0x21 && reply.GetContentPtr() == NULL)
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

    Thread testThread(instance, gTestStack, sizeof(gTestStack),
                      VCOS_CONFIG_THREAD_PRIORITY_MAIN,
                      THREAD_FLAGS_CREATE_WOUT_YIELD | THREAD_FLAGS_CREATE_STACKTEST,
                      testThreadFunc, (void *)&instance, "test");

    Msg msg(instance);
    Msg reply(instance);

    while (1)
    {
        if (msg.Receive())
        {
            if (msg.GetType() == 0x20 && msg.GetContentPtr() == NULL)
            {
                printf("mainApp(): got message from %" PRIkernel_pid "\r\n", msg.GetSenderPid());
                reply.SetType(0x21);
                reply.SetContentPtr(NULL);
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
