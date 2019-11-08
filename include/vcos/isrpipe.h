#ifndef VCOS_ISRPIPE_H
#define VCOS_ISRPIPE_H

#include <stdint.h>

#include <vcos/instance.h>

#ifdef __cplusplus
extern "C" {
#endif

int vcIsrpipeUartWriteOne(vcInstance *aInstance, char aChar);

int vcIsrpipeUartRead(vcInstance *aInstance, char *aBuf, size_t aCount);

#ifdef __cplusplus
}
#endif

#endif /* VCOS_ISRPIPE_H */
