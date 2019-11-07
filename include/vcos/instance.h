#ifndef VCOS_INSTANCE_H
#define VCOS_INSTANCE_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct vcInstance vcInstance;

vcInstance *vcInstanceInitSingle(void);

bool vcInstanceIsInitialized(vcInstance *aInstance);

void vcInstanceFinalize(vcInstance *aInstance);

void vcInstanceReset(vcInstance *aInstance);

const char *vcGetVersionString(void);

#ifdef __cplusplus
}
#endif

#endif /* VCOS_INSTANCE_H */
