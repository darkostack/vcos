#include <vcos/stdiobase.h>
#include <vcos/periph/uart.h>

#include "common/instance.hpp"

using namespace vc;

void vcStdioInit(vcInstance *aInstance)
{
    vcUartInit(UART_DEV(1),
               115200,
               (vcUartRxCallback)vcStdioWriteOne,
               (void *)aInstance);
}

int vcStdioWriteOne(vcInstance *aInstance, char aChar)
{
    Instance &instance = *static_cast<Instance *>(aInstance);
    return instance.Get<Utils::UartIsrpipe>().WriteOne(aChar);
}

ssize_t vcStdioRead(void *aBuffer, size_t aCount)
{
    return Instance::Get().Get<Utils::UartIsrpipe>().Read(static_cast<char *>(aBuffer), aCount);
}

ssize_t vcStdioWrite(const void *aBuffer, size_t aLen)
{
    return vcUartWrite(UART_DEV(1), (const uint8_t *)aBuffer, aLen);
}
