#ifndef VCOS_NET_PLATFORM_ALARM_MICRO_H
#define VCOS_NET_PLATFORM_ALARM_MICRO_H

#include <stdint.h>

#include <vcos/instance.h>

#ifdef __cplusplus
extern "C" {
#endif

void vcNetPlatAlarmMicroStartAt(vcInstance *aInstance, uint32_t aT0, uint32_t aDt);

void vcNetPlatAlarmMicroStop(vcInstance *aInstance);

uint32_t vcNetPlatAlarmMicroGetNow(void);

extern void vcNetPlatAlarmMicroFired(vcInstance *aInstance);

#ifdef __cplusplus
}
#endif

#endif /* VCOS_NET_PLATFORM_ALARM_MICRO_H */
