#ifndef VCOS_CLI_H
#define VCOS_CLI_H

#include <vcos/instance.h>

#ifdef __cplusplus
extern "C" {
#endif

void vcCliUartInit(vcInstance *aInstance);

void vcCliUartOutputBytes(const uint8_t *aBytes, uint8_t aLength);

void vcCliUartOutputFormat(const char *aFmt, ...);

#ifdef __cplusplus
}
#endif

#endif /* VCOS_CLI_H */
