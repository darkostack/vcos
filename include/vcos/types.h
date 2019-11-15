#ifndef VCOS_TYPES_H
#define VCOS_TYPES_H

#include <stdint.h>
#include <stddef.h>
#include <inttypes.h>
#include <limits.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct vcInstance vcInstance;

typedef void *(*vcThreadHandlerFunc)(void *aArg);

typedef enum {
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

typedef struct vcListNode {
    struct vcListNode *mNext;
} vcListNode;

typedef vcListNode vcClistNode;

typedef struct vcCib {
    unsigned int mReadCount;
    unsigned int mWriteCount;
    unsigned int mMask;
} vcCib;

typedef struct vcMsg {
    vcKernelPid mSenderPid;
    uint16_t    mType;
    union {
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

#ifndef KERNEL_MAXTHREADS
#define KERNEL_MAXTHREADS   (32)
#endif

#define KERNEL_PID_UNDEF    (0)
#define KERNEL_PID_FIRST    (KERNEL_PID_UNDEF + 1)
#define KERNEL_PID_LAST     (KERNEL_PID_FIRST + KERNEL_MAXTHREADS - 1)

#define PRIkernel_pid       PRIi16

#ifdef __GNUC__
#define container_of(PTR, TYPE, MEMBER) \
        (__extension__ ({ \
            __extension__ const __typeof__ (((TYPE *) 0)->MEMBER) *__m____ = (PTR); \
            ((TYPE *) ((char *) __m____ - offsetof(TYPE, MEMBER))); \
        }))
#else
#define container_of(PTR, TYPE, MEMBER) \
        ((TYPE *) ((char *) (PTR) - offsetof(TYPE, MEMBER)))
#endif

#ifdef __GNUC__
#define NORETURN  __attribute__((noreturn))
#else
#define NORETURN
#endif

#ifdef __GNUC__
#define CONST  __attribute__((const))
#else
#define CONST
#endif

#ifdef __GNUC__
#define PURE  __attribute__((pure))
#else
#define PURE
#endif

#if ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 5)) || (__GNUC__ >= 5)
#define UNREACHABLE() __builtin_unreachable()
#else
#define UNREACHABLE() do { /* nothing */ } while (1)
#endif

#define ALIGN_OF(T) (offsetof(struct { char c; T t; }, t))

#define BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2 * !!(condition)]))

#ifndef SSIZE_MAX
#define SSIZE_MAX ((ssize_t) (SIZE_MAX / 2))
#endif

#ifdef __cplusplus
}
#endif

#endif /* VCOS_TYPES_H */
