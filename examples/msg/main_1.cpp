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

    msg.SetType(0x20);
    msg.SetContentPtr(NULL);

    assert(msg.Send(1));

    while (1) {
        if (msg.Receive()) {
            if (msg.GetType() == 0x21 && msg.GetContentPtr() == NULL) {
                printf("testThreadFunc(): get an acked from %" PRIkernel_pid "\r\n", msg.GetSenderPid());
            }
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

    while (1) {
        if (msg.Receive()) {
            if (msg.GetType() == 0x20 && msg.GetContentPtr() == NULL) {
                printf("mainApp(): got message from %" PRIkernel_pid "\r\n", msg.GetSenderPid());
                msg.SetType(0x21);
                msg.SetContentPtr(NULL);
                assert(msg.Send(msg.GetSenderPid()));
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
