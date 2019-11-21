#ifndef VCOS_TYPES_H
#define VCOS_TYPES_H

#include <inttypes.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#include <vcos/defines.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct vcInstance vcInstance;

typedef void *(*vcThreadHandlerFunc)(void *aArg);

typedef enum
{
    THREAD_STATUS_STOPPED,
    THREAD_STATUS_SLEEPING,
    THREAD_STATUS_MUTEX_BLOCKED,
    THREAD_STATUS_RECEIVE_BLOCKED,
    THREAD_STATUS_SEND_BLOCKED,
    THREAD_STATUS_REPLY_BLOCKED,
    THREAD_STATUS_FLAG_BLOCKED_ANY,
    THREAD_STATUS_FLAG_BLOCKED_ALL,
    THREAD_STATUS_MBOX_BLOCKED,
    THREAD_STATUS_COND_BLOCKED,
    THREAD_STATUS_RUNNING,
    THREAD_STATUS_PENDING,
    THREAD_STATUS_NUMOF
} vcThreadStatus;

typedef int16_t vcKernelPid;

typedef struct vcListNode
{
    struct vcListNode *mNext;
} vcListNode;

typedef struct vcMutex
{
    vcListNode mQueue;
} vcMutex;

typedef vcListNode vcClistNode;

typedef struct vcCib
{
    unsigned int mReadCount;
    unsigned int mWriteCount;
    unsigned int mMask;
} vcCib;

typedef struct vcMsg
{
    vcKernelPid mSenderPid;
    uint16_t    mType;
    union
    {
        void *   mPtr;
        uint32_t mValue;
    } mContent;
} vcMsg;

typedef struct vcThread
{
    char *         mSp;
    vcThreadStatus mStatus;
    uint8_t        mPriority;
    vcKernelPid    mPid;
    vcClistNode    mRqEntry;
    void *         mWaitData;
    vcListNode     mMsgWaiters;
    vcCib          mMsgQueue;
    vcMsg *        mMsgArray;
    char *         mStackStart;
    const char *   mName;
    int            mStackSize;
} vcThread;

typedef struct
{
    uint64_t mTicks64;
} vcTimerTicsk64;

typedef struct
{
    uint32_t mTicks32;
} vcTimerTicks32;

typedef void (*vcTimerCallback)(void *);

typedef struct vcTimer
{
    struct vcTimer *mNext;
    uint32_t        mTarget;
    uint32_t        mLongTarget;
    vcTimerCallback mCallback;
    void *          mArg;
} vcTimer;

typedef struct vcRingBuffer
{
    char *       mBuf;
    unsigned int mSize;
    unsigned int mStart;
    unsigned int mAvail;
} vcRingBuffer;

#ifdef __cplusplus
}
#endif

#endif /* VCOS_TYPES_H */
