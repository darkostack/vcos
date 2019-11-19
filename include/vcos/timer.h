#ifndef VCOS_TIMER_H
#define VCOS_TIMER_H

#include <vcos/types.h>

#ifdef __cplusplus
extern "C" {
#endif

void vcTimerInit(vcTimer *aTimer, vcTimerCallback aCallback, void *aArg);

vcTimerTicks32 vcTimerNow(void);

vcTimerTicsk64 vcTimerNow64(void);

uint32_t vcTimerNowUsec(void);

uint64_t vcTimerNowUsec64(void);

void vcTimerSet(vcTimer *aTimer, uint32_t aOffset);

void vcTimerSet64(vcTimer *aTimer, uint32_t aOffset, uint32_t aLongOffset);

void vcTimerSleepUsec(uint32_t aUsec);

void vcTimerSleepUsec64(uint64_t aUsec);

#ifdef __cplusplus
}
#endif

#endif /* VCOS_TIMER_H */
