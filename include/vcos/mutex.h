#ifndef MUTEX_H
#define MUTEX_H

#include <vcos/types.h>

#ifdef __cplusplus
extern "C" {
#endif

void vcMutexLock(vcMutex *aMutex);

void vcMutexUnlock(vcMutex *aMutex);

#ifdef __cplusplus
}
#endif

#endif /* MUTEX_H */
