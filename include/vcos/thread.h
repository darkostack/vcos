#ifndef VCOS_THREAD_H
#define VCOS_THREAD_H

#include <vcos/types.h>

#ifdef __cplusplus
extern "C" {
#endif

vcKernelPid vcThreadCreate(vcThread *aThread,
                           char *aStack,
                           int aStackSize,
                           char aPriority,
                           int aFlags,
                           vcThreadHandlerFunc aFunction,
                           void *aArg,
                           const char *aName);

static inline int vcPidIsValid(vcKernelPid aPid)
{
    return ((KERNEL_PID_FIRST <= aPid) && (aPid <= KERNEL_PID_LAST));
}

#ifdef __cplusplus
}
#endif

#endif /* VCOS_THREAD_H */
