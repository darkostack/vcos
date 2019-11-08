#include <stdio.h>
#include <assert.h>

#include <vcos/instance.h>
#include <vcos/isrpipe.h>
#include <vcos/periph/uart.h>

int main(int argc, char *argv[])
{
    vcInstance *instance = vcInstanceInitSingle();

    assert(instance != NULL);

    vcUartInit(UART_DEV(1), 115200, (vcUartRxCallback)vcIsrpipeUartWriteOne, (void *)instance);

    printf("hello world!\n");

    while (1) {
    }

    return 0;
}
