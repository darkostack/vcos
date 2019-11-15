#ifndef VCOS_INSTANCE_H
#define VCOS_INSTANCE_H

#include <stdlib.h>
#include <stdbool.h>

#include <vcos/types.h>

#ifdef __cplusplus
extern "C" {
#endif

vcInstance *vcInstanceGet(void);

const char *vcInstanceGetVersionString(void);

#ifdef __cplusplus
}
#endif

#endif /* VCOS_INSTANCE_H */
