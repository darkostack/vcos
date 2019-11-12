#ifndef VCOS_THREAD_H
#define VCOS_THREAD_H

#ifdef __cplusplus
extern "C" {
#endif

typedef void *(*vcThreadHandlerFunc)(void *aArg);

char *vcThreadStackInit(vcThreadHandlerFunc aFunction,
                        void *aArg,
                        void *aStackStart,
                        int aStackSize);

int vcThreadContexSwitchRequest(void);

void vcThreadYieldHigher(void);

void vcThreadSwitchContextExit(void);

void vcThreadTaskExit(void);

int vcThreadIsrStackUsage(void);

void *vcThreadIsrStackPointer(void);

void vcSchedRun(void);

#ifdef __cplusplus
}
#endif

#endif /* VCOS_THREAD_H */
