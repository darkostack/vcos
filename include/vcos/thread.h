#ifndef VCOS_THREAD_H
#define VCOS_THREAD_H

#include <vcos/types.h>

#ifdef __cplusplus
extern "C" {
#endif

extern vcKernelPid gMainThreadPid;
extern vcKernelPid gIdleThreadPid;

vcKernelPid vcThreadCreate(vcThread *aThread,
                           char *aStack,
                           int aStackSize,
                           char aPriority,
                           int aFlags,
                           vcThreadHandlerFunc aFunction,
                           void *aArg,
                           const char *aName);

char *vcThreadStackInit(vcThreadHandlerFunc aFunction,
                        void *aArg,
                        void *aStackStart,
                        int aStackSize);

int vcThreadGetContexSwitchRequest(void);

void vcThreadYieldHigher(void);

void vcThreadSwitchContextExit(void);

void vcThreadTaskExit(void);

int vcThreadIsrStackUsage(void);

void *vcThreadIsrStackPointer(void);

void vcSchedRun(void);

static inline int vcPidIsValid(vcKernelPid aPid)
{
    return ((KERNEL_PID_FIRST <= aPid) && (aPid <= KERNEL_PID_LAST));
}

#ifdef __cplusplus
}
#endif

#endif /* VCOS_THREAD_H */
