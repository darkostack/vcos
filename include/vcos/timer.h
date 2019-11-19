#ifndef VCOS_TIMER_H
#define VCOS_TIMER_H

#include <vcos/types.h>

#ifdef __cplusplus
extern "C" {
#endif

vcTimerTicks32 vcTimerNow(void);

vcTimerTicsk64 vcTimerNow64(void);

uint32_t vcTimerNowUsec(void);

uint64_t vcTimerNowUsec64(void);

#ifdef __cplusplus
}
#endif

#endif /* VCOS_TIMER_H */
