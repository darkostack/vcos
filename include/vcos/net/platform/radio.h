#ifndef VCOS_NET_PLATFORM_RADIO_H
#define VCOS_NET_PLATFORM_RADIO_H

#include <stdint.h>

#include <vcos/instance.h>
#include <vcos/net/error.h>

#ifdef __cplusplus
extern "C" {
#endif

enum
{
    VC_NET_RADIO_FRAME_MAX_SIZE    = 127,
    VC_NET_RADIO_SYMBOLS_PER_OCTET = 2,
    VC_NET_RADIO_BIT_RATE          = 250000,
    VC_NET_RADIO_BITS_PER_OCTET    = 8,

    VC_NET_RADIO_SYMBOL_TIME =
        ((VC_NET_RADIO_BITS_PER_OCTET / VC_NET_RADIO_SYMBOLS_PER_OCTET) * 1000000) / VC_NET_RADIO_BIT_RATE,
    VC_NET_RADIO_LQI_NONE     = 0,
    VC_NET_RADIO_RSSI_INVALID = 127,
};

enum
{
    VC_NET_RADIO_CHANNEL_PAGE_0      = 0,
    VC_NET_RADIO_CHANNEL_PAGE_0_MASK = (1U << VC_NET_RADIO_CHANNEL_PAGE_0),
    VC_NET_RADIO_CHANNEL_PAGE_2      = 2,
    VC_NET_RADIO_CHANNEL_PAGE_2_MASK = (1U << VC_NET_RADIO_CHANNEL_PAGE_2),
    VC_NET_RADIO_CHANNEL_PAGE_MAX    = VC_NET_RADIO_CHANNEL_PAGE_2,
};

enum
{
    VC_NET_RADIO_915MHZ_OQPSK_CHANNEL_MIN  = 1,
    VC_NET_RADIO_915MHZ_OQPSK_CHANNEL_MAX  = 10,
    VC_NET_RADIO_915MHZ_OQPSK_CHANNEL_MASK = 0x3ff << VC_NET_RADIO_915MHZ_OQPSK_CHANNEL_MIN,
    VC_NET_RADIO_2P4GHZ_OQPSK_CHANNEL_MIN  = 11,
    VC_NET_RADIO_2P4GHZ_OQPSK_CHANNEL_MAX  = 26,
    VC_NET_RADIO_2P4GHZ_OQPSK_CHANNEL_MASK = 0xffff << VC_NET_RADIO_2P4GHZ_OQPSK_CHANNEL_MIN,
};

typedef uint8_t vcNetRadioCaps;

enum
{
    VC_NET_RADIO_CAPS_NONE             = 0,
    VC_NET_RADIO_CAPS_ACK_TIMEOUT      = 1 << 0,
    VC_NET_RADIO_CAPS_ENERGY_SCAN      = 1 << 1,
    VC_NET_RADIO_CAPS_TRANSMIT_RETRIES = 1 << 2,
    VC_NET_RADIO_CAPS_CSMA_BACKOFF     = 1 << 3,
};

#define VC_NET_PANID_BROADCAST 0xffff

typedef uint16_t vcNetPanId;

typedef uint16_t vcNetShortAddress;

#define VC_NET_EXT_ADDRESS_SIZE 8

struct vcNetExtAddress
{
    uint8_t m8[VC_NET_EXT_ADDRESS_SIZE];
} __attribute__((packed));

typedef struct vcNetExtAddress vcNetExtAddress;

typedef struct vcNetRadioIeInfo
{
    int64_t mNetworkTimeOffset;
    uint8_t mTimeIeOffset;
    uint8_t mTimeSyncSeq;
} vcNetRadioIeInfo;

typedef struct vcNetRadioFrame
{
    uint8_t *mPsdu;

    uint16_t mLength;
    uint8_t  mChannel;

    union
    {
        struct
        {
            const uint8_t *   mAesKey;
            vcNetRadioIeInfo *mIeInfo;
            uint8_t           mMaxCsmaBackoffs;
            uint8_t           mMaxFrameRetries;
            bool              mIsARetransmission : 1;
            bool              mCsmaCaEnabled : 1;
        } mTxInfo;

        struct
        {
            uint64_t mTimestamp;
            int8_t   mRssi;
            uint8_t  mLqi;
            bool     mAckedWithFramePending : 1;
        } mRxInfo;
    } mInfo;
} vcNetRadioFrame;

typedef enum vcNetRadioState
{
    VC_NET_RADIO_STATE_DISABLED = 0,
    VC_NET_RADIO_STATE_SLEEP    = 1,
    VC_NET_RADIO_STATE_RECEIVE  = 2,
    VC_NET_RADIO_STATE_TRANSMIT = 3,
} vcNetRadioState;

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
