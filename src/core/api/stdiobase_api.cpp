#include <vcos/periph/uart.h>
#include <vcos/stdiobase.h>

#include "common/instance.hpp"

using namespace vc;

void vcStdioInit(vcInstance *aInstance)
{
    vcUartInit(STDIOBASE_UART_DEV, 115200, (vcUartRxCallback)vcStdioWriteOne, (void *)aInstance);
}

int vcStdioWriteOne(vcInstance *aInstance, char aChar)
{
    Instance &instance = *static_cast<Instance *>(aInstance);
    return instance.Get<Utils::UartIsrpipe>().WriteOne(aChar);
}

ssize_t vcStdioWrite(const void *aBuffer, size_t aLen)
{
    return vcUartWrite(STDIOBASE_UART_DEV, (const uint8_t *)aBuffer, aLen);
}

ssize_t vcStdioRead(void *aBuffer, size_t aCount)
{
    Instance &instance = Instance::Get();
    return instance.Get<Utils::UartIsrpipe>().Read(static_cast<char *>(aBuffer), aCount);
}

int vcStdioReadAvailable(void)
{
    Instance &instance = Instance::Get();
    return instance.Get<Utils::UartIsrpipe>().GetTsrb().Avail();
}
