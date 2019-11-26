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

#ifdef __cplusplus
}
#endif

#endif /* VCOS_NET_TYPES_H */
