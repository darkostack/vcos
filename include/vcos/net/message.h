#ifndef VCOS_NET_MESSAGE_H
#define VCOS_NET_MESSAGE_H

#include <vcos/types.h>
#include <vcos/net/types.h>

#ifdef __cplusplus
extern "C" {
#endif

void vcNetMessageFree(vcNetMessage *aMessage);

uint16_t vcNetMessageGetLength(const vcNetMessage *aMessage);

vcNetError vcNetMessageSetLength(vcNetMessage *aMessage, uint16_t aLength);

uint16_t vcNetMessageGetOffset(const vcNetMessage *aMessage);

vcNetError vcNetMessageSetOffset(vcNetMessage *aMessage, uint16_t aOffset);

bool vcNetMessageIsLinkSecurityEnabled(const vcNetMessage *aMessage);

void vcNetMessageSetDirectTransmission(vcNetMessage *aMessage, bool aEnabled);

int8_t vcNetMessageGetRss(const vcNetMessage *aMessage);

vcNetError vcNetMessageAppend(vcNetMessage *aMessage, const void *aBuf, uint16_t aLength);

uint16_t vcNetMessageRead(const vcNetMessage *aMessage, uint16_t aOffset, void *aBuf, uint16_t aLength);

int vcNetMessageWrite(vcNetMessage *aMessage, uint16_t aOffset, const void *aBuf, uint16_t aLength);

void vcNetMessageQueueInit(vcNetMessageQueue *aQueue);

vcNetError vcNetMessageQueueEnqueue(vcNetMessageQueue *aQueue, vcNetMessage *aMessage);

vcNetError vcNetMessageQueueEnqueueAtHead(vcNetMessageQueue *aQueue, vcNetMessage *aMessage);

vcNetError vcNetMessageQueueDequeue(vcNetMessageQueue *aQueue, vcNetMessage *aMessage);

vcNetMessage *vcNetMessageQueueGetHead(vcNetMessageQueue *aQueue);

vcNetMessage *vcNetMessageQueueGetNext(vcNetMessageQueue *aQueue, const vcNetMessage *aMessage);

void vcNetMessageBufferInfo(vcInstance *aInstance, vcNetBufferInfo *aBufferInfo);

#ifdef __cplusplus
}
#endif

#endif /* VCOS_NET_MESSAGE_H */
