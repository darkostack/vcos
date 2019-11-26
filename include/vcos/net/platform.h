#ifndef VCOS_NET_PLATFORM_H
#define VCOS_NET_PLATFORM_H

#include <vcos/instance.h>

#ifdef __cplusplus
extern "C" {
#endif

void vcNetPlatProcessDrivers(vcInstance *aInstance);

extern void vcNetPlatAlarmProcess(vcInstance *aInstance);

#ifdef __cplusplus
}
#endif

#endif /* VCOS_NET_PLATFORM_H */
