#include "net/message.hpp"

#include "common/locator-getters.hpp"

#define ENABLE_DEBUG (0)
#include "common/debug.h"

namespace vc {
namespace Net {

MessagePool::MessagePool()
{
    memset(mBuffers, 0, sizeof(mBuffers));

    mFreeBuffers = mBuffers;

    for (uint16_t i = 0; i < kNumBuffers - 1; i++)
    {
        mBuffers[i].SetNextBuffer(&mBuffers[i + 1]);
    }

    mBuffers[kNumBuffers - 1].SetNextBuffer(NULL);
    mNumFreeBuffers = kNumBuffers;
}

Message *MessagePool::New(uint8_t aType, uint16_t aReserveHeader, uint8_t aPriority)
{
    vcNetError error   = VC_NET_ERROR_NONE;
    Message *  message = NULL;

    VerifyOrExit((message = static_cast<Message *>(NewBuffer(aPriority))) != NULL);

    memset(message, 0, sizeof(*message));

    message->SetMessagePool(this);
    message->SetType(aType);
    message->SetReserved(aReserveHeader);
    message->SetLinkSecurityEnabled(false);

    SuccessOrExit(error = message->SetPriority(aPriority));
    SuccessOrExit(error = message->SetLength(0));

exit:
    if (error != VC_NET_ERROR_NONE)
    {
        Free(message);
        message = NULL;
    }

    return message;
}

Message *MessagePool::New(uint8_t aType, uint16_t aReserveHeader, const vcNetMessageSettings *aSettings)
{
    Message *message;
    bool     linkSecurityEnabled;
    uint8_t  priority;

    if (aSettings == NULL)
    {
        linkSecurityEnabled = false;
        priority            = VC_NET_MESSAGE_PRIORITY_NORMAL;
    }
    else
    {
        linkSecurityEnabled = aSettings->mLinkSecurityEnabled;
        priority            = aSettings->mPriority;
    }

    message = New(aType, aReserveHeader, priority);

    if (message)
    {
        message->SetLinkSecurityEnabled(linkSecurityEnabled);
    }

    return message;
}

void MessagePool::Free(Message *aMessage)
{
    assert(aMessage->Next() == NULL && aMessage->Prev() == NULL);

    FreeBuffers(static_cast<Buffer *>(aMessage));
}

Buffer *MessagePool::NewBuffer(uint8_t aPriority)
{
    Buffer *buffer = NULL;

    SuccessOrExit(ReclaimBuffers(1, aPriority));

    if (mFreeBuffers != NULL)
    {
        buffer       = mFreeBuffers;
        mFreeBuffers = mFreeBuffers->GetNextBuffer();
        buffer->SetNextBuffer(NULL);
        mNumFreeBuffers--;
    }

    if (buffer == NULL)
    {
        DEBUG("MessagePool::NewBuffer() No available message buffer\r\n");
    }

exit:
    return buffer;
}

void MessagePool::FreeBuffers(Buffer *aBuffer)
{
    while (aBuffer != NULL)
    {
        Buffer *tmpBuffer = aBuffer->GetNextBuffer();
        aBuffer->SetNextBuffer(mFreeBuffers);
        mFreeBuffers = aBuffer;
        mNumFreeBuffers++;
        aBuffer = tmpBuffer;
    }
}

vcNetError MessagePool::ReclaimBuffers(int aNumBuffers, uint8_t aPriority)
{
    (void)aPriority;

    // First comparison is to get around issues with comparing
    // signed and unsigned numbers, if aNumBuffers is negative then
    // the second comparison wont be attempted.

    return (aNumBuffers < 0 || aNumBuffers <= GetFreeBufferCount()) ? VC_NET_ERROR_NONE : VC_NET_ERROR_NO_BUFS;
}

uint16_t MessagePool::GetFreeBufferCount(void) const
{
    return mNumFreeBuffers;
}

vcNetError Message::ResizeMessage(uint16_t aLength)
{
    vcNetError error = VC_NET_ERROR_NONE;

    // add buffers
    Buffer * curBuffer = this;
    Buffer * lastBuffer;
    uint16_t curLength = kHeadBufferDataSize;

    while (curLength < aLength)
    {
        if (curBuffer->GetNextBuffer() == NULL)
        {
            curBuffer->SetNextBuffer(GetMessagePool()->NewBuffer(GetPriority()));
            VerifyOrExit(curBuffer->GetNextBuffer() != NULL, error = VC_NET_ERROR_NO_BUFS);
        }

        curBuffer = curBuffer->GetNextBuffer();
        curLength += kBufferDataSize;
    }

    // remove buffers
    lastBuffer = curBuffer;
    curBuffer  = curBuffer->GetNextBuffer();
    lastBuffer->SetNextBuffer(NULL);

    GetMessagePool()->FreeBuffers(curBuffer);

exit:
    return error;
}

void Message::Free(void)
{
    GetMessagePool()->Free(this);
}

Message *Message::GetNext(void) const
{
    Message *next;
    Message *tail;

    if (mBuffer.mHead.mInfo.mInPriorityQ)
    {
        PriorityQueue *priorityQueue = GetPriorityQueue();
        VerifyOrExit(priorityQueue != NULL, next = NULL);
        tail = priorityQueue->GetTail();
    }
    else
    {
        MessageQueue *messageQueue = GetMessageQueue();
        VerifyOrExit(messageQueue != NULL, next = NULL);
        tail = messageQueue->GetTail();
    }

    next = (this == tail) ? NULL : Next();

exit:
    return next;
}

vcNetError Message::SetLength(uint16_t aLength)
{
    vcNetError error              = VC_NET_ERROR_NONE;
    uint16_t   totalLengthRequest = GetReserved() + aLength;
    uint16_t   totalLengthCurrent = GetReserved() + GetLength();
    int        bufs               = 0;

    VerifyOrExit(totalLengthRequest >= GetReserved(), error = VC_NET_ERROR_INVALID_ARGS);

    if (totalLengthRequest > kHeadBufferDataSize)
    {
        bufs = (((totalLengthRequest - kHeadBufferDataSize) - 1) / kBufferDataSize) + 1;
    }

    if (totalLengthCurrent > kHeadBufferDataSize)
    {
        bufs -= (((totalLengthCurrent - kHeadBufferDataSize) - 1) / kBufferDataSize) + 1;
    }

    SuccessOrExit(error = GetMessagePool()->ReclaimBuffers(bufs, GetPriority()));

    SuccessOrExit(error = ResizeMessage(totalLengthRequest));

    mBuffer.mHead.mInfo.mLength = aLength;

    // Correct offset in case shorter length is set.
    if (GetOffset() > aLength)
    {
        SetOffset(aLength);
    }

exit:
    return error;
}

uint8_t Message::GetBufferCount(void) const
{
    uint8_t rval = 1;

    for (const Buffer *curBuffer = GetNextBuffer(); curBuffer; curBuffer = curBuffer->GetNextBuffer())
    {
        rval++;
    }

    return rval;
}

vcNetError Message::MoveOffset(int aDelta)
{
    vcNetError error = VC_NET_ERROR_NONE;

    assert(GetOffset() + aDelta <= GetLength());

    VerifyOrExit(GetOffset() + aDelta <= GetLength(), error = VC_NET_ERROR_INVALID_ARGS);

    mBuffer.mHead.mInfo.mOffset += static_cast<int16_t>(aDelta);

    assert(mBuffer.mHead.mInfo.mOffset <= GetLength());

exit:
    return error;
}

vcNetError Message::SetOffset(uint16_t aOffset)
{
    vcNetError error = VC_NET_ERROR_NONE;

    assert(aOffset <= GetLength());

    VerifyOrExit(aOffset <= GetLength(), error = VC_NET_ERROR_INVALID_ARGS);

    mBuffer.mHead.mInfo.mOffset = aOffset;

exit:
    return error;
}

vcNetError Message::SetPriority(uint8_t aPriority)
{
    vcNetError error = VC_NET_ERROR_NONE;

    PriorityQueue *priorityQueue = NULL;

    VerifyOrExit(aPriority < kNumPriorities, error = VC_NET_ERROR_INVALID_ARGS);

    VerifyOrExit(IsInAQueue(), mBuffer.mHead.mInfo.mPriority = aPriority);
    VerifyOrExit(mBuffer.mHead.mInfo.mPriority != aPriority);

    if (mBuffer.mHead.mInfo.mInPriorityQ)
    {
        priorityQueue = mBuffer.mHead.mInfo.mQueue.mPriority;
        priorityQueue->Dequeue(*this);
    }

    mBuffer.mHead.mInfo.mPriority = aPriority;

    if (priorityQueue != NULL)
    {
        priorityQueue->Enqueue(*this);
    }

exit:
    return error;
}

vcNetError Message::Append(const void *aBuf, uint16_t aLength)
{
    vcNetError error = VC_NET_ERROR_NONE;

    uint16_t oldLength = GetLength();
    int      bytesWritten;

    SuccessOrExit(error = SetLength(GetLength() + aLength));

    bytesWritten = Write(oldLength, aLength, aBuf);

    assert(bytesWritten == (int)aLength);

    (void)bytesWritten;

exit:
    return error;
}

vcNetError Message::Prepend(const void *aBuf, uint16_t aLength)
{
    vcNetError error     = VC_NET_ERROR_NONE;
    Buffer *   newBuffer = NULL;

    while (aLength > GetReserved())
    {
        VerifyOrExit((newBuffer = GetMessagePool()->NewBuffer(GetPriority())) != NULL, error = VC_NET_ERROR_NO_BUFS);

        newBuffer->SetNextBuffer(GetNextBuffer());
        SetNextBuffer(newBuffer);

        if (GetReserved() < sizeof(mBuffer.mHead.mData))
        {
            // Copy payload from the first buffer.
            memcpy(newBuffer->mBuffer.mHead.mData + GetReserved(), mBuffer.mHead.mData + GetReserved(),
                   sizeof(mBuffer.mHead.mData) - GetReserved());
        }

        SetReserved(GetReserved() + kBufferDataSize);
    }

    SetReserved(GetReserved() - aLength);
    mBuffer.mHead.mInfo.mLength += aLength;
    SetOffset(GetOffset() + aLength);

    if (aBuf != NULL)
    {
        Write(0, aLength, aBuf);
    }

exit:
    return error;
}

void Message::RemoveHeader(uint16_t aLength)
{
    assert(aLength <= mBuffer.mHead.mInfo.mLength);

    mBuffer.mHead.mInfo.mReserved += aLength;
    mBuffer.mHead.mInfo.mLength -= aLength;

    if (mBuffer.mHead.mInfo.mOffset > aLength)
    {
        mBuffer.mHead.mInfo.mOffset -= aLength;
    }
    else
    {
        mBuffer.mHead.mInfo.mOffset = 0;
    }
}

uint16_t Message::Read(uint16_t aOffset, uint16_t aLength, void *aBuf) const
{
    Buffer * curBuffer;
    uint16_t bytesCopied = 0;
    uint16_t bytesToCopy;

    if (aOffset >= GetLength())
    {
        ExitNow();
    }

    if (aOffset + aLength >= GetLength())
    {
        aLength = GetLength() - aOffset;
    }

    aOffset += GetReserved();

    // special case first buffer
    if (aOffset < kHeadBufferDataSize)
    {
        bytesToCopy = kHeadBufferDataSize - aOffset;

        if (bytesToCopy > aLength)
        {
            bytesToCopy = aLength;
        }

        memcpy(aBuf, GetFirstData() + aOffset, bytesToCopy);

        aLength -= bytesToCopy;
        bytesCopied += bytesToCopy;
        aBuf = static_cast<uint8_t *>(aBuf) + bytesToCopy;

        aOffset = 0;
    }
    else
    {
        aOffset -= kHeadBufferDataSize;
    }

    // advance to offset
    curBuffer = GetNextBuffer();

    while (aOffset >= kBufferDataSize)
    {
        assert(curBuffer != NULL);

        curBuffer = curBuffer->GetNextBuffer();
        aOffset -= kBufferDataSize;
    }

    // begin copy
    while (aLength > 0)
    {
        assert(curBuffer != NULL);

        bytesToCopy = kBufferDataSize - aOffset;

        if (bytesToCopy > aLength)
        {
            bytesToCopy = aLength;
        }

        memcpy(aBuf, curBuffer->GetData() + aOffset, bytesToCopy);

        aLength -= bytesToCopy;
        bytesCopied += bytesToCopy;
        aBuf = static_cast<uint8_t *>(aBuf) + bytesToCopy;

        curBuffer = curBuffer->GetNextBuffer();
        aOffset   = 0;
    }

exit:
    return bytesCopied;
}

int Message::Write(uint16_t aOffset, uint16_t aLength, const void *aBuf)
{
    Buffer * curBuffer;
    uint16_t bytesCopied = 0;
    uint16_t bytesToCopy;

    assert(aOffset + aLength <= GetLength());

    if (aOffset + aLength >= GetLength())
    {
        aLength = GetLength() - aOffset;
    }

    aOffset += GetReserved();

    // special case first buffer
    if (aOffset < kHeadBufferDataSize)
    {
        bytesToCopy = kHeadBufferDataSize - aOffset;

        if (bytesToCopy > aLength)
        {
            bytesToCopy = aLength;
        }

        memcpy(GetFirstData() + aOffset, aBuf, bytesToCopy);

        aLength -= bytesToCopy;
        bytesCopied += bytesToCopy;
        aBuf = static_cast<const uint8_t *>(aBuf) + bytesToCopy;

        aOffset = 0;
    }
    else
    {
        aOffset -= kHeadBufferDataSize;
    }

    // advance to offset
    curBuffer = GetNextBuffer();

    while (aOffset >= kBufferDataSize)
    {
        assert(curBuffer != NULL);

        curBuffer = curBuffer->GetNextBuffer();
        aOffset -= kBufferDataSize;
    }

    // begin copy
    while (aLength > 0)
    {
        assert(curBuffer != NULL);

        bytesToCopy = kBufferDataSize - aOffset;

        if (bytesToCopy > aLength)
        {
            bytesToCopy = aLength;
        }

        memcpy(curBuffer->GetData() + aOffset, aBuf, bytesToCopy);

        aLength -= bytesToCopy;
        bytesCopied += bytesToCopy;
        aBuf = static_cast<const uint8_t *>(aBuf) + bytesToCopy;

        curBuffer = curBuffer->GetNextBuffer();
        aOffset   = 0;
    }

    return bytesCopied;
}

int Message::CopyTo(uint16_t aSourceOffset, uint16_t aDestinationOffset, uint16_t aLength, Message &aMessage) const
{
    uint16_t bytesCopied = 0;
    uint16_t bytesToCopy;
    uint8_t  buf[16];

    while (aLength > 0)
    {
        bytesToCopy = (aLength < sizeof(buf)) ? aLength : sizeof(buf);

        Read(aSourceOffset, bytesToCopy, buf);
        aMessage.Write(aDestinationOffset, bytesToCopy, buf);

        aSourceOffset += bytesToCopy;
        aDestinationOffset += bytesToCopy;
        aLength -= bytesToCopy;
        bytesCopied += bytesToCopy;
    }

    return bytesCopied;
}

Message *Message::Clone(uint16_t aLength) const
{
    vcNetError error = VC_NET_ERROR_NONE;
    Message *  messageCopy;
    uint16_t   offset;

    VerifyOrExit((messageCopy = GetMessagePool()->New(GetType(), GetReserved(), GetPriority())) != NULL,
                 error = VC_NET_ERROR_NO_BUFS);
    SuccessOrExit(error = messageCopy->SetLength(aLength));
    CopyTo(0, 0, aLength, *messageCopy);

    // Copy selected message information.
    offset = GetOffset() < aLength ? GetOffset() : aLength;
    messageCopy->SetOffset(offset);

    messageCopy->SetSubType(GetSubType());
    messageCopy->SetLinkSecurityEnabled(IsLinkSecurityEnabled());

exit:

    if (error != VC_NET_ERROR_NONE && messageCopy != NULL)
    {
        messageCopy->Free();
        messageCopy = NULL;
    }

    return messageCopy;
}

uint16_t Message::UpdateChecksum(uint16_t aChecksum, uint16_t aValue)
{
    uint16_t result = aChecksum + aValue;
    return result + (result < aChecksum);
}

uint16_t Message::UpdateChecksum(uint16_t aChecksum, const void *aBuf, uint16_t aLength)
{
    const uint8_t *bytes = reinterpret_cast<const uint8_t *>(aBuf);

    for (int i = 0; i < aLength; i++)
    {
        aChecksum = UpdateChecksum(aChecksum, (i & 1) ? bytes[i] : static_cast<uint16_t>(bytes[i] << 8));
    }

    return aChecksum;
}

uint16_t Message::UpdateChecksum(uint16_t aChecksum, uint16_t aOffset, uint16_t aLength) const
{
    Buffer * curBuffer;
    uint16_t bytesCovered = 0;
    uint16_t bytesToCover;

    assert(aOffset + aLength <= GetLength());

    aOffset += GetReserved();

    // special case first buffer
    if (aOffset < kHeadBufferDataSize)
    {
        bytesToCover = kHeadBufferDataSize - aOffset;

        if (bytesToCover > aLength)
        {
            bytesToCover = aLength;
        }

        aChecksum = Message::UpdateChecksum(aChecksum, GetFirstData() + aOffset, bytesToCover);

        aLength -= bytesToCover;
        bytesCovered += bytesToCover;

        aOffset = 0;
    }
    else
    {
        aOffset -= kHeadBufferDataSize;
    }

    // advance to offset
    curBuffer = GetNextBuffer();

    while (aOffset >= kBufferDataSize)
    {
        assert(curBuffer != NULL);

        curBuffer = curBuffer->GetNextBuffer();
        aOffset -= kBufferDataSize;
    }

    // begin copy
    while (aLength > 0)
    {
        assert(curBuffer != NULL);

        bytesToCover = kBufferDataSize - aOffset;

        if (bytesToCover > aLength)
        {
            bytesToCover = aLength;
        }

        aChecksum = Message::UpdateChecksum(aChecksum, curBuffer->GetData() + aOffset, bytesToCover);

        aLength -= bytesToCover;
        bytesCovered += bytesToCover;

        curBuffer = curBuffer->GetNextBuffer();
        aOffset   = 0;
    }

    return aChecksum;
}

void Message::SetMessageQueue(MessageQueue *aMessageQueue)
{
    mBuffer.mHead.mInfo.mQueue.mMessage = aMessageQueue;
    mBuffer.mHead.mInfo.mInPriorityQ    = false;
}

void Message::SetPriorityQueue(PriorityQueue *aPriorityQueue)
{
    mBuffer.mHead.mInfo.mQueue.mPriority = aPriorityQueue;
    mBuffer.mHead.mInfo.mInPriorityQ     = true;
}

MessageQueue::MessageQueue(void)
{
    SetTail(NULL);
}

Message *MessageQueue::GetHead(void) const
{
    return (GetTail() == NULL) ? NULL : GetTail()->Next();
}

vcNetError MessageQueue::Enqueue(Message &aMessage, QueuePosition aPosition)
{
    vcNetError error = VC_NET_ERROR_NONE;

    VerifyOrExit(!aMessage.IsInAQueue(), error = VC_NET_ERROR_ALREADY);

    aMessage.SetMessageQueue(this);

    assert((aMessage.Next() == NULL) && (aMessage.Prev() == NULL));

    if (GetTail() == NULL)
    {
        aMessage.Next() = &aMessage;
        aMessage.Prev() = &aMessage;

        SetTail(&aMessage);
    }
    else
    {
        Message *head = GetTail()->Next();

        aMessage.Next() = head;
        aMessage.Prev() = GetTail();

        head->Prev()      = &aMessage;
        GetTail()->Next() = &aMessage;

        if (aPosition == kQueuePositionTail)
        {
            SetTail(&aMessage);
        }
    }

exit:
    return error;
}

vcNetError MessageQueue::Dequeue(Message &aMessage)
{
    vcNetError error = VC_NET_ERROR_NONE;

    VerifyOrExit(aMessage.GetMessageQueue() == this, error = VC_NET_ERROR_NOT_FOUND);

    assert((aMessage.Next() != NULL) && (aMessage.Prev() != NULL));

    if (&aMessage == GetTail())
    {
        SetTail(GetTail()->Prev());

        if (&aMessage == GetTail())
        {
            SetTail(NULL);
        }
    }

    aMessage.Prev()->Next() = aMessage.Next();
    aMessage.Next()->Prev() = aMessage.Prev();

    aMessage.Prev() = NULL;
    aMessage.Next() = NULL;

    aMessage.SetMessageQueue(NULL);

exit:
    return error;
}

void MessageQueue::GetInfo(uint16_t &aMessageCount, uint16_t &aBufferCount) const
{
    aMessageCount = 0;
    aBufferCount  = 0;

    for (const Message *message = GetHead(); message != NULL; message = message->GetNext())
    {
        aMessageCount++;
        aBufferCount += message->GetBufferCount();
    }
}

PriorityQueue::PriorityQueue(void)
{
    for (int priority = 0; priority < Message::kNumPriorities; priority++)
    {
        mTails[priority] = NULL;
    }
}

Message *PriorityQueue::FindFirstNonNullTail(uint8_t aStartPriorityLevel) const
{
    Message *tail = NULL;
    uint8_t  priority;

    priority = aStartPriorityLevel;

    do
    {
        if (mTails[priority] != NULL)
        {
            tail = mTails[priority];
            break;
        }

        priority = PrevPriority(priority);
    } while (priority != aStartPriorityLevel);

    return tail;
}

Message *PriorityQueue::GetHead(void) const
{
    Message *tail;

    tail = FindFirstNonNullTail(0);

    return (tail == NULL) ? NULL : tail->Next();
}

Message *PriorityQueue::GetHeadForPriority(uint8_t aPriority) const
{
    Message *head;
    Message *previousTail;

    if (mTails[aPriority] != NULL)
    {
        previousTail = FindFirstNonNullTail(PrevPriority(aPriority));

        assert(previousTail != NULL);

        head = previousTail->Next();
    }
    else
    {
        head = NULL;
    }

    return head;
}

Message *PriorityQueue::GetTail(void) const
{
    return FindFirstNonNullTail(0);
}

vcNetError PriorityQueue::Enqueue(Message &aMessage)
{
    vcNetError error = VC_NET_ERROR_NONE;
    uint8_t    priority;
    Message *  tail;
    Message *  next;

    VerifyOrExit(!aMessage.IsInAQueue(), error = VC_NET_ERROR_ALREADY);

    aMessage.SetPriorityQueue(this);

    priority = aMessage.GetPriority();

    tail = FindFirstNonNullTail(priority);

    if (tail != NULL)
    {
        next = tail->Next();

        aMessage.Next() = next;
        aMessage.Prev() = tail;
        next->Prev()    = &aMessage;
        tail->Next()    = &aMessage;
    }
    else
    {
        aMessage.Next() = &aMessage;
        aMessage.Prev() = &aMessage;
    }

    mTails[priority] = &aMessage;

exit:
    return error;
}

vcNetError PriorityQueue::Dequeue(Message &aMessage)
{
    vcNetError error = VC_NET_ERROR_NONE;
    uint8_t    priority;
    Message *  tail;

    VerifyOrExit(aMessage.GetPriorityQueue() == this, error = VC_NET_ERROR_NOT_FOUND);

    priority = aMessage.GetPriority();

    tail = mTails[priority];

    if (&aMessage == tail)
    {
        tail = tail->Prev();

        if ((&aMessage == tail) || (tail->GetPriority() != priority))
        {
            tail = NULL;
        }

        mTails[priority] = tail;
    }

    aMessage.Next()->Prev() = aMessage.Prev();
    aMessage.Prev()->Next() = aMessage.Next();
    aMessage.Next()         = NULL;
    aMessage.Prev()         = NULL;

    aMessage.SetMessageQueue(NULL);

exit:
    return error;
}

void PriorityQueue::GetInfo(uint16_t &aMessageCount, uint16_t &aBufferCount) const
{
    aMessageCount = 0;
    aBufferCount  = 0;

    for (const Message *message = GetHead(); message != NULL; message = message->GetNext())
    {
        aMessageCount++;
        aBufferCount += message->GetBufferCount();
    }
}

} // namespace Net
} // namespace vc
