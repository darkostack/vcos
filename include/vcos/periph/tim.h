#ifndef PERIPH_TIM_H
#define PERIPH_TIM_H

#include <limits.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int vcTim;

#define TIM_DEV(x) (x)

#define TIM_UNDEF ((vcTim)(UINT_MAX))

typedef void (*vcTimCallback)(void *aArg, int aChannel);

typedef struct
{
    vcTimCallback mCallback;
    void *        mArg;
} vcTimIsrContext;

int vcTimInit(vcTim aDev, unsigned long aFreq, vcTimCallback aCallback, void *aArg);

int vcTimSetAbsolute(vcTim aDev, unsigned aChannel, unsigned int aValue);

int vcTimClear(vcTim aDev, unsigned aChannel);

uint32_t vcTimRead(vcTim aDev);

void vcTimStart(vcTim aDev);

void vcTimStop(vcTim aDev);

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_TIM_H */
