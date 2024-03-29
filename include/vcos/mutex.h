#ifndef VCOS_MUTEX_H
#define VCOS_MUTEX_H

#include <vcos/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MUTEX_LOCKED ((vcListNode *)-1)

void vcMutexLock(vcMutex *aMutex);

void vcMutexUnlock(vcMutex *aMutex);

#ifdef __cplusplus
}
#endif

#endif /* VCOS_MUTEX_H */
