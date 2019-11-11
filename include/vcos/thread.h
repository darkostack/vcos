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

void vcThreadYieldHigher(void);

void vcThreadSwitchContextExit(void);

void vcThreadTaskExit(void);

void vcThreadStackPrint(void);

int vcThreadIsrStackUsage(void);

void *vcThreadIsrStackPointer(void);

void *vcSchedGetActiveThread(void);

void vcSchedRun(void);

#ifdef __cplusplus
}
#endif

#endif /* VCOS_THREAD_H */
