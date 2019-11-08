#include <assert.h>

#include <vcos/isrpipe.h>

#include "stdio_base.h"
#include "periph/uart.h"

extern vcInstance *gInstancePtr;

void stdio_init(void)
{
    uart_init(UART_DEV(1), 115200, (uart_rx_cb_t)vcIsrpipeUartWriteOne, (void *)gInstancePtr);
}

ssize_t stdio_read(void *buffer, size_t count)
{
    return (ssize_t)vcIsrpipeUartRead(gInstancePtr, (char *)buffer, count);
}

ssize_t stdio_write(const void *buffer, size_t len)
{
    uart_write(UART_DEV(1), (const uint8_t *)buffer, len);
    return len;
}
