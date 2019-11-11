#include <vcos/isrpipe.h>

#include "utils/isrpipe.hpp"

using namespace vc::Utils;

char gUartTsrb[VCOS_CONFIG_TSRB_UART_MEMORY_SIZE];
Isrpipe UartIsrpipe(static_cast<char *>(&gUartTsrb[0]), static_cast<unsigned int>(VC_ARRAY_LENGTH(gUartTsrb)));

int vcIsrpipeUartWriteOne(vcInstance *aInstance, char aChar)
{
    (void) aInstance;
    return UartIsrpipe.WriteOne(aChar);
}


int vcIsrpipeUartRead(vcInstance *aInstance, char *aBuf, size_t aCount)
{
    (void) aInstance;
    return UartIsrpipe.Read(aBuf, aCount);
}
