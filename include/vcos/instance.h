#ifndef VCOS_INSTANCE_H
#define VCOS_INSTANCE_H

#include <stdlib.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct vcInstance vcInstance;

const char *vcGetVersionString(void);

#ifdef __cplusplus
}
#endif

#endif /* VCOS_INSTANCE_H */
