#ifndef VCOS_STDIOBASE_H
#define VCOS_STDIOBASE_H

#include <stdint.h>
#include <unistd.h>

#include <vcos/instance.h>

#ifdef __cplusplus
extern "C" {
#endif

void vcStdioInit(vcInstance *aInstance);

int vcStdioWriteOne(vcInstance *aInstance, char aChar);

ssize_t vcStdioWrite(const void *aBuffer, size_t aLen);

ssize_t vcStdioRead(void *aBuffer, size_t aCount);


#ifdef __cplusplus
}
#endif

#endif /* VCOS_STDIOBASE_H */
