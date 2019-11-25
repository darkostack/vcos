#ifndef VCOS_THREAD_H
#define VCOS_THREAD_H

#include <vcos/types.h>

#ifdef __cplusplus
extern "C" {
#endif

vcKernelPid vcThreadCreate(vcThread *          aThread,
                           char *              aStack,
                           int                 aStackSize,
                           char                aPriority,
                           int                 aFlags,
                           vcThreadHandlerFunc aFunction,
                           void *              aArg,
                           const char *        aName);

void vcThreadSchedulerRun(void);

int vcThreadPidIsValid(vcKernelPid aPid);

#ifdef __cplusplus
}
#endif

#endif /* VCOS_THREAD_H */
