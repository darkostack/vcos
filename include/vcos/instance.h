#ifndef VCOS_INSTANCE_H
#define VCOS_INSTANCE_H

#include <stdbool.h>
#include <stdlib.h>

#include <vcos/types.h>

#ifdef __cplusplus
extern "C" {
#endif

vcInstance *vcInstanceInit(void);

bool vcInstanceIsInitialized(vcInstance *aInstance);

vcInstance *vcInstanceGet(void);

const char *vcInstanceGetVersionString(void);

#ifdef __cplusplus
}
#endif

#endif /* VCOS_INSTANCE_H */
