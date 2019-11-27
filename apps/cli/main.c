#include <assert.h>
#include <stdio.h>

#include <vcos/cli.h>
#include <vcos/instance.h>
#include <vcos/timer.h>

int main(void)
{
    vcInstance *instance = vcInstanceGet();

    assert(vcInstanceIsInitialized(instance));

    vcCliUartInit(instance);

    while (1)
    {
        vcTimerSleepUsec(100);
    }

    return 0;
}
