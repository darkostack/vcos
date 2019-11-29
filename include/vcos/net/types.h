#ifndef VCOS_NET_TYPES_H
#define VCOS_NET_TYPES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum vcNetError
{
    VC_NET_ERROR_NONE = 0,
    VC_NET_ERROR_FAILED,
    VC_NET_ERROR_DROP,
    VC_NET_ERROR_NO_BUFS,
    VC_NET_ERROR_NO_ROUTE,
    VC_NET_ERROR_BUSY,
    VC_NET_ERROR_PARSE,
    VC_NET_ERROR_INVALID_ARGS,
    VC_NET_ERROR_NOT_FOUND,
    VC_NET_ERROR_ALREADY,
    VC_NET_ERROR_NUMS,
} vcNetError;

typedef struct vcNetMessage
{
    struct vcNetMessage *mNext;
} vcNetMessage;

typedef struct vcNetBufferInfo
{
    uint16_t mTotalBuffers;
    uint16_t mFreeBuffers;
} vcNetBufferInfo;

typedef enum vcNetMessagePriority
{
    VC_NET_MESSAGE_PRIORITY_LOW    = 0,
    VC_NET_MESSAGE_PRIORITY_NORMAL = 1,
    VC_NET_MESSAGE_PRIORITY_HIGH   = 2,
} vcNetMessagePriority;

typedef struct vcNetMessageSettings
{
    bool                 mLinkSecurityEnabled;
    vcNetMessagePriority mPriority;
} vcNetMessageSettings;

typedef struct
{
    void *mData;
} vcNetMessageQueue;

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

typedef uint32_t vcNetChangedFlags;

typedef void (*vcNetStateChangedCallback)(vcNetChangedFlags aFlags, void *aContext);

#ifdef __cplusplus
}
#endif

#endif /* VCOS_NET_TYPES_H */
