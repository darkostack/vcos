#ifndef VCOS_NET_PLATFORM_RADIO_H
#define VCOS_NET_PLATFORM_RADIO_H

#include <stdint.h>

#include <vcos/instance.h>
#include <vcos/net/error.h>

#ifdef __cplusplus
extern "C" {
#endif

vcNetRadioCaps vcNetPlatRadioGetCaps(vcInstance *aInstance);

const char *vcNetPlatRadioGetVersionString(vcInstance *aInstance);

int8_t vcNetPlatRadioGetReceiveSensitivity(vcInstance *aInstance);

void vcNetPlatRadioGetIeeeEui64(vcInstance *aInstance, uint8_t *aIeeeEui64);

void vcNetPlatRadioSetPanId(vcInstance *aInstance, vcNetPanId aPanId);

void vcNetPlatRadioSetExtendedAddress(vcInstance *aInstance, const vcNetExtAddress *aExtAddress);

void vcNetPlatRadioSetShortAddress(vcInstance *aInstance, vcNetShortAddress aShortAddress);

vcNetError vcNetPlatRadioGetTransmitPower(vcInstance *aInstance, int8_t *aPower);

vcNetError vcNetPlatRadioSetTransmitPower(vcInstance *aInstance, int8_t aPower);

bool vcNetPlatRadioGetPromiscuous(vcInstance *aInstance);

void vcNetPlatRadioSetPromiscuous(vcInstance *aInstance, bool aEnabled);

vcNetRadioState vcNetPlatRadioGetState(vcInstance *aInstance);

vcNetError vcNetPlatRadioEnable(vcInstance *aInstance);

vcNetError vcNetPlatRadioDisable(vcInstance *aInstance);

bool vcNetPlatRadioIsEnabled(vcInstance *aInstance);

vcNetError vcNetPlatRadioSleep(vcInstance *aInstance);

vcNetError vcNetPlatRadioReceive(vcInstance *aInstance, uint8_t aChannel);

extern void vcNetPlatRadioReceiveDone(vcInstance *aInstance, vcNetRadioFrame *aFrame, vcNetError aError);

vcNetRadioFrame *vcNetPlatRadioGetTransmitBuffer(vcInstance *aInstance);

vcNetError vcNetPlatRadioTransmit(vcInstance *aInstance, vcNetRadioFrame *aFrame);

extern void vcNetPlatRadioTxStarted(vcInstance *aInstance, vcNetRadioFrame *aFrame);

extern void vcNetPlatRadioTxDone(vcInstance *     aInstance,
                                 vcNetRadioFrame *aFrame,
                                 vcNetRadioFrame *aAckFrame,
                                 vcNetError       aError);

extern void vcNetPlatRadioFrameUpdated(vcInstance *aInstance, vcNetRadioFrame *aFrame);

int8_t vcNetPlatRadioGetRssi(vcInstance *aInstance);

vcNetError vcNetPlatRadioEnergyScan(vcInstance *aInstance, uint8_t aScanChannel, uint16_t aScanDuration);

extern void vcNetPlatRadioEnergyScanDone(vcInstance *aInstance, int8_t aEnergyScanMaxRssi);

void vcNetPlatRadioEnableSrcMatch(vcInstance *aInstance, bool aEnable);

vcNetError vcNetPlatRadioAddSrcMatchShortEntry(vcInstance *aInstance, vcNetShortAddress aShortAddress);

vcNetError vcNetPlatRadioAddSrcMatchExtEntry(vcInstance *aInstance, const vcNetExtAddress *aExtAddress);

vcNetError vcNetPlatRadioClearSrcMatchShortEntry(vcInstance *aInstance, vcNetShortAddress aShortAddress);

vcNetError vcNetPlatRadioClearSrcMatchExtEntry(vcInstance *aInstance, const vcNetExtAddress *aExtAddress);

void vcNetPlatRadioClearSrcMatchShortEntries(vcInstance *aInstance);

void vcNetPlatRadioClearSrcMatchExtEntries(vcInstance *aInstance);

uint32_t vcNetPlatRadioGetSupportedChannelMask(vcInstance *aInstance);

uint32_t vcNetPlatRadioGetPreferredChannelMask(vcInstance *aInstance);

#ifdef __cplusplus
}
#endif

#endif /* VCOS_NET_PLATFORM_RADIO_H */
