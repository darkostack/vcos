#include <vcos/net/message.h>

#include "common/instance.hpp"
#include "common/locator-getters.hpp"

using namespace vc;

void vcNetMessageFree(vcNetMessage *aMessage)
{
    static_cast<Net::Message *>(aMessage)->Free();
}

uint16_t vcNetMessageGetLength(const vcNetMessage *aMessage)
{
    const Net::Message &message = *static_cast<const Net::Message *>(aMessage);
    return message.GetLength();
}

vcNetError vcNetMessageSetLength(vcNetMessage *aMessage, uint16_t aLength)
{
    Net::Message &message = *static_cast<Net::Message *>(aMessage);
    return message.SetLength(aLength);
}

uint16_t vcNetMessageGetOffset(const vcNetMessage *aMessage)
{
    const Net::Message &message = *static_cast<const Net::Message *>(aMessage);
    return message.GetOffset();
}

vcNetError vcNetMessageSetOffset(vcNetMessage *aMessage, uint16_t aOffset)
{
    Net::Message &message = *static_cast<Net::Message *>(aMessage);
    return message.SetOffset(aOffset);
}

bool vcNetMessageIsLinkSecurityEnabled(const vcNetMessage *aMessage)
{
    const Net::Message &message = *static_cast<const Net::Message *>(aMessage);
    return message.IsLinkSecurityEnabled();
}

void vcNetMessageSetDirectTransmission(vcNetMessage *aMessage, bool aEnabled)
{
    Net::Message &message = *static_cast<Net::Message *>(aMessage);

    if (aEnabled)
    {
        message.SetDirectTransmission();
    }
    else
    {
        message.ClearDirectTransmission();
    }
}

vcNetError vcNetMessageAppend(vcNetMessage *aMessage, const void *aBuf, uint16_t aLength)
{
    Net::Message &message = *static_cast<Net::Message *>(aMessage);
    return message.Append(aBuf, aLength);
}

uint16_t vcNetMessageRead(const vcNetMessage *aMessage, uint16_t aOffset, void *aBuf, uint16_t aLength)
{
    const Net::Message &message = *static_cast<const Net::Message *>(aMessage);
    return message.Read(aOffset, aLength, aBuf);
}

int vcNetMessageWrite(vcNetMessage *aMessage, uint16_t aOffset, const void *aBuf, uint16_t aLength)
{
    Net::Message &message = *static_cast<Net::Message *>(aMessage);
    return message.Write(aOffset, aLength, aBuf);
}

void vcNetMessageQueueInit(vcNetMessageQueue *aQueue)
{
    aQueue->mData = NULL;
}

vcNetError vcNetMessageQueueEnqueue(vcNetMessageQueue *aQueue, vcNetMessage *aMessage)
{
    Net::Message &message = *static_cast<Net::Message *>(aMessage);
    Net::MessageQueue &queue = *static_cast<Net::MessageQueue *>(aQueue);
    return queue.Enqueue(message);
}

vcNetError vcNetMessageQueueEnqueueAtHead(vcNetMessageQueue *aQueue, vcNetMessage *aMessage)
{
    Net::Message &message = *static_cast<Net::Message *>(aMessage);
    Net::MessageQueue &queue = *static_cast<Net::MessageQueue *>(aQueue);
    return queue.Enqueue(message, Net::MessageQueue::kQueuePositionHead);
}

vcNetError vcNetMessageQueueDequeue(vcNetMessageQueue *aQueue, vcNetMessage *aMessage)
{
    Net::Message &message = *static_cast<Net::Message *>(aMessage);
    Net::MessageQueue &queue = *static_cast<Net::MessageQueue *>(aQueue);
    return queue.Dequeue(message);
}

vcNetMessage *vcNetMessageQueueGetHead(vcNetMessageQueue *aQueue)
{
    Net::MessageQueue &queue = *static_cast<Net::MessageQueue *>(aQueue);
    return queue.GetHead();
}

vcNetMessage *vcNetMessageQueueGetNext(vcNetMessageQueue *aQueue, const vcNetMessage *aMessage)
{
    Net::Message *next;

    VerifyOrExit(aMessage != NULL, next = NULL);

    {
        const Net::Message &message = *static_cast<const Net::Message *>(aMessage);
        Net::MessageQueue &queue = *static_cast<Net::MessageQueue *>(aQueue);

        VerifyOrExit(message.GetMessageQueue() == &queue, next = NULL);
        next = message.GetNext();
    }

exit:
    return next;
}

void vcNetMessageGetBufferInfo(vcInstance *aInstance, vcNetBufferInfo *aBufferInfo)
{
    Instance &instance = *static_cast<Instance *>(aInstance);

    aBufferInfo->mTotalBuffers = VCOS_NET_CONFIG_NUM_MESSAGE_BUFFERS;
    aBufferInfo->mFreeBuffers  = instance.Get<Net::MessagePool>().GetFreeBufferCount();
}
