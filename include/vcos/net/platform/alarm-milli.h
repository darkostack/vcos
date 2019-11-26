#ifndef VCOS_NET_PLATFORM_ALARM_MILLI_H
#define VCOS_NET_PLATFORM_ALARM_MILLI_H

#include <stdint.h>

#include <vcos/instance.h>

#ifdef __cplusplus
extern "C" {
#endif

void vcNetPlatAlarmMilliStartAt(vcInstance *aInstance, uint32_t aT0, uint32_t aDt);

void vcNetPlatAlarmMilliStop(vcInstance *aInstance);

uint32_t vcNetPlatAlarmMilliGetNow(void);

extern void vcNetPlatAlarmMilliFired(vcInstance *aInstance);

#ifdef __cplusplus
}
#endif

#endif /* VCOS_NET_PLATFORM_ALARM_MILLI_H */
