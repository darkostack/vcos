#ifndef VCOS_THREAD_H
#define VCOS_THREAD_H

#include <vcos/types.h>

#ifdef __cplusplus
extern "C" {
#endif
/**
 * Optional flags for controlling a threads initial state.
 */
#define THREAD_FLAGS_CREATE_SLEEPING (1)
#define THREAD_FLAGS_AUTO_FREE (2)
#define THREAD_FLAGS_CREATE_WOUT_YIELD (4)
#define THREAD_FLAGS_CREATE_STACKTEST (8)

#define THREAD_STATUS_ON_RUNQUEUE THREAD_STATUS_RUNNING
#define THREAD_STATUS_NOT_FOUND ((vcThreadStatus)-1)

vcKernelPid vcThreadCreate(vcThread *          aThread,
                           char *              aStack,
                           int                 aStackSize,
                           char                aPriority,
                           int                 aFlags,
                           vcThreadHandlerFunc aFunction,
                           void *              aArg,
                           const char *        aName);

void vcThreadSchedulerRun(void);

void vcThreadTaskExit(void);

int vcThreadPidIsValid(vcKernelPid aPid);

#ifdef __cplusplus
}
#endif

#endif /* VCOS_THREAD_H */
