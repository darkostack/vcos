#ifndef NET_MESSAGE_HPP
#define NET_MESSAGE_HPP

#include "vcos-core-config.h"

#include <stdint.h>
#include <string.h>
#include <assert.h>

#include <vcos/net/message.h>

#include "common/code_utils.hpp"
#include "common/locator.hpp"

namespace vc {
namespace Net {

enum
{
    kNumBuffers = VCOS_NET_CONFIG_NUM_MESSAGE_BUFFERS,
    kBufferSize = VCOS_NET_CONFIG_MESSAGE_BUFFER_SIZE,
};

class Message;
class MessagePool;
class MessageQueue;
class PriorityQueue;

struct MessageInfo
{
    Message *mNext;
    Message *mPrev;
    MessagePool *mMessagePool;

    union
    {
        MessageQueue *mMessage;
        PriorityQueue *mPriority;
    } mQueue;

    uint32_t mDatagramTag;

    uint16_t mReserved;
    uint16_t mLength;
    uint16_t mOffset;

    uint8_t mType : 2;
    uint8_t mSubType : 4;
    bool mDirectTx : 1;
    bool mLinkSecurity : 1;
    uint8_t mPriority : 2;
    bool mInPriorityQ : 1;
    bool mTxSuccess : 1;
};

class Buffer : public ::vcNetMessage
{
    friend class Message;

public:

    class Buffer *GetNextBuffer(void) const { return static_cast<Buffer *>(mNext); }

    void SetNextBuffer(class Buffer *aBuf) { mNext = static_cast<vcNetMessage *>(aBuf); }

private:
    uint8_t *GetFirstData(void) { return mBuffer.mHead.mData; }

    const uint8_t *GetFirstData(void) const { return mBuffer.mHead.mData; }

    uint8_t *GetData(void) { return mBuffer.mData; }

    const uint8_t *GetData(void) const { return mBuffer.mData; }

    enum
    {
        kBufferDataSize = kBufferSize - sizeof(struct vcNetMessage),
        kHeadBufferDataSize = kBufferDataSize - sizeof(struct MessageInfo),
    };

protected:
    union
    {
        struct
        {
            MessageInfo mInfo;
            uint8_t mData[kHeadBufferDataSize];
        } mHead;
        uint8_t mData[kBufferDataSize];
    } mBuffer;
};

class Message : public Buffer
{
    friend class MessagePool;
    friend class MessageQueue;
    friend class PriorityQueue;

public:
    enum
    {
        kTypeIp6 = 0,
        kType6lowpan = 1,
    };

    enum
    {
        kSubTypeNone = 0,
    };

    enum
    {
        kPriorityLow = VC_NET_MESSAGE_PRIORITY_LOW,
        kPriorityNormal = VC_NET_MESSAGE_PRIORITY_NORMAL,
        kPriorityHigh = VC_NET_MESSAGE_PRIORITY_HIGH,
        kPriorityNet = VC_NET_MESSAGE_PRIORITY_HIGH + 1,

        kNumPriorities = 4,
    };

    void Free(void);

    Message *GetNext(void) const;

    uint16_t GetLength(void) const { return mBuffer.mHead.mInfo.mLength; }

    vcNetError SetLength(uint16_t aLength);

    uint8_t GetBufferCount(void) const;

    uint16_t GetOffset(void) const { return mBuffer.mHead.mInfo.mOffset; }

    vcNetError MoveOffset(int aDelta);

    vcNetError SetOffset(uint16_t aOffset);

    uint8_t GetType(void) const { return mBuffer.mHead.mInfo.mType; }

    void SetType(uint8_t aType) { mBuffer.mHead.mInfo.mType = aType; }

    uint8_t GetSubType(void) const { return mBuffer.mHead.mInfo.mSubType; }

    void SetSubType(uint8_t aSubType) { mBuffer.mHead.mInfo.mSubType = aSubType; }

    uint8_t GetPriority(void) const { return mBuffer.mHead.mInfo.mPriority; }

    vcNetError SetPriority(uint8_t aPriority);

    vcNetError Prepend(const void *aBuf, uint16_t aLength);

    void RemoveHeader(uint16_t aLength);

    vcNetError Append(const void *aBuf, uint16_t aLength);

    uint16_t Read(uint16_t aOffset, uint16_t aLength, void *aBuf) const;

    int Write(uint16_t aOffset, uint16_t aLength, const void *aBuf);

    int CopyTo(uint16_t aSourceOffset, uint16_t aDestinationOffset, uint16_t aLength, Message &aMessage) const;

    Message *Clone(uint16_t aLength) const;

    Message *Clone(void) const { return Clone(GetLength()); }

    uint32_t GetDatagramTag(void) const { return mBuffer.mHead.mInfo.mDatagramTag; }

    void SetDatagramTag(uint32_t aTag) { mBuffer.mHead.mInfo.mDatagramTag = aTag; }

    bool GetDirectTransmission(void) const { return mBuffer.mHead.mInfo.mDirectTx; }

    void ClearDirectTransmission(void) { mBuffer.mHead.mInfo.mDirectTx = false; }

    void SetDirectTransmission(void) { mBuffer.mHead.mInfo.mDirectTx = true; }

    bool GetTxSuccess(void) const { return mBuffer.mHead.mInfo.mTxSuccess; }

    void SetTxSuccess(bool aTxSuccess) { mBuffer.mHead.mInfo.mTxSuccess = aTxSuccess; }

    bool IsLinkSecurityEnabled(void) const { return mBuffer.mHead.mInfo.mLinkSecurity; }

    void SetLinkSecurityEnabled(bool aEnabled) { mBuffer.mHead.mInfo.mLinkSecurity = aEnabled; }

    static uint16_t UpdateChecksum(uint16_t aChecksum, uint16_t aValue);

    static uint16_t UpdateChecksum(uint16_t aChecksum, const void *aBuf, uint16_t aLength);

    uint16_t UpdateChecksum(uint16_t aChecksum, uint16_t aOffset, uint16_t aLength) const;

    MessageQueue *GetMessageQueue(void) const
    {
        return (!mBuffer.mHead.mInfo.mInPriorityQ) ? mBuffer.mHead.mInfo.mQueue.mMessage : NULL;
    }

private:
    MessagePool *GetMessagePool(void) const { return mBuffer.mHead.mInfo.mMessagePool; }

    void SetMessagePool(MessagePool *aMessagePool) { mBuffer.mHead.mInfo.mMessagePool = aMessagePool; }

    bool IsInAQueue(void) const { return (mBuffer.mHead.mInfo.mQueue.mMessage != NULL); }

    void SetMessageQueue(MessageQueue *aMessageQueue);

    PriorityQueue *GetPriorityQueue(void) const
    {
        return (mBuffer.mHead.mInfo.mInPriorityQ) ? mBuffer.mHead.mInfo.mQueue.mPriority : NULL;
    }

    void SetPriorityQueue(PriorityQueue *aPriorityQueue);

    Message *&Next(void) { return mBuffer.mHead.mInfo.mNext; }

    Message *const &Next(void) const { return mBuffer.mHead.mInfo.mNext; }

    Message *&Prev(void) { return mBuffer.mHead.mInfo.mPrev; }

    uint16_t GetReserved(void) const { return mBuffer.mHead.mInfo.mReserved; }

    void SetReserved(uint16_t aReservedHeader) { mBuffer.mHead.mInfo.mReserved = aReservedHeader; }

    vcNetError ResizeMessage(uint16_t aLength);
};

class MessageQueue : public vcNetMessageQueue
{
    friend class Message;
    friend class PriorityQueue;

public:
    enum QueuePosition
    {
        kQueuePositionHead,
        kQueuePositionTail,
    };

    MessageQueue(void);

    Message *GetHead(void) const;

    vcNetError Enqueue(Message &aMessage) { return Enqueue(aMessage, kQueuePositionTail); }

    vcNetError Enqueue(Message &aMessage, QueuePosition aPosition);

    vcNetError Dequeue(Message &aMessage);

    void GetInfo(uint16_t &aMessageCount, uint16_t &aBufferCount) const;

private:
    Message *GetTail(void) const { return static_cast<Message *>(mData); }

    void SetTail(Message *aMessage) { mData = aMessage; }
};

class PriorityQueue
{
    friend class Message;
    friend class MessageQueue;
    friend class MessagePool;

public:
    PriorityQueue(void);

    Message *GetHead(void) const;

    Message *GetHeadForPriority(uint8_t aPriority) const;

    vcNetError Enqueue(Message &aMessage);

    vcNetError Dequeue(Message &aMessage);

    void GetInfo(uint16_t &aMessageCount, uint16_t &aBufferCount) const;

    Message *GetTail(void) const;

private:
    uint8_t PrevPriority(uint8_t aPriority) const
    {
        return (aPriority == Message::kNumPriorities - 1) ? 0 : (aPriority + 1);
    }

    Message *FindFirstNonNullTail(uint8_t aStartPriorityLevel) const;

private:
    Message *mTails[Message::kNumPriorities];
};

class MessagePool : public InstanceLocator
{
    friend class Message;
    friend class MessageQueue;
    friend class PriorityQueue;

public:
    explicit MessagePool(void);

    Message *New(uint8_t aType, uint16_t aReserveHeader, uint8_t aPriority = kDefaultMessagePriority);

    Message *New(uint8_t aType, uint16_t aReserveHeader, const vcNetMessageSettings *aSettings);

    void Free(Message *aMessage);

    uint16_t GetFreeBufferCount(void) const;

private:
    enum
    {
        kDefaultMessagePriority = Message::kPriorityNormal,
    };

    Buffer *NewBuffer(uint8_t aPriority);
    void FreeBuffers(Buffer *aBuffer);
    vcNetError ReclaimBuffers(int aNumBuffers, uint8_t aPriority);

    uint16_t mNumFreeBuffers;
    Buffer mBuffers[kNumBuffers];
    Buffer *mFreeBuffers;
};

} // namespace Net
} // namespace vc

#endif /* NET_MESSAGE_HPP */
