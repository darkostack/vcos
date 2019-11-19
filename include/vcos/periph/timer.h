#ifndef PERIPH_TIMER_H
#define PERIPH_TIMER_H

#include <stddef.h>
#include <stdint.h>
#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int vcTim;

#define TIMER_DEV(x) (x)

#define TIMER_UNDEF ((vcTim)(UINT_MAX))

typedef void (*vcTimerCallback)(void *aArg, int aChannel);

typedef struct {
    vcTimerCallback mCallback;
    void *mArg;
} vcTimerIsrContext;

int vcTimerInit(vcTim aDev, unsigned long aFreq, vcTimerCallback aCallback, void *aArg);

int vcTimerSetAbsolute(vcTim aDev, int aChannel, unsigned int aValue);

int vcTimerClear(vcTim aDev, int aChannel);

uint32_t vcTimerRead(vcTim aDev);

void vcTimerStart(vcTim aDev);

void vcTimerStop(vcTim aDev);

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_TIMER_H */
