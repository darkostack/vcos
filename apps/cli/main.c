#include <assert.h>
#include <stdio.h>

#include <vcos/cli.h>
#include <vcos/instance.h>
#include <vcos/thread.h>

#include <vcos/net/tasklet.h>

int main(void)
{
    vcInstance *instance = vcInstanceGet();

    assert(vcInstanceIsInitialized(instance));

    vcCliUartInit(instance);

    while (1)
    {
        vcNetTaskletProcess(instance);
        vcThreadYieldHigher();
    }

    return 0;
}
