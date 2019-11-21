#include <assert.h>
#include <stdio.h>

#include <vcos/cli.h>
#include <vcos/instance.h>

int main(void)
{
    vcInstance *instance = vcInstanceGet();

    assert(vcInstanceIsInitialized(instance));

    vcCliUartInit(instance);

    vcCliUartRun();

    /* Should not reach here */
    assert(0);

    return 0;
}
