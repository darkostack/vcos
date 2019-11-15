#include <stdio.h>
#include <assert.h>

#include <vcos/instance.h>
#include <vcos/cli.h>

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
