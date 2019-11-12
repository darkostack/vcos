#include <stdio.h>
#include <assert.h>

#include <vcos/cli.h>
#include <vcos/instance.h>

int main(void)
{
    vcInstance *instance = vcInstanceGet();

    assert(instance != NULL);

    vcCliUartInit(instance);

    vcCliUartRun();

    return 0;
}
