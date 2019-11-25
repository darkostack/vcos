#include "gtest/gtest.h"
#include "net/message.hpp"

using namespace vc;

class TestMessage : public testing::Test
{
protected:
    Net::MessagePool *obj;

    enum
    {
        kNumTestMessages = 5,
    };

    virtual void SetUp()
    {
        obj = new Net::MessagePool;
    }

    virtual void TearDown()
    {
        delete obj;
    }
};

TEST_F(TestMessage, constructor)
{
    EXPECT_TRUE(obj);

    EXPECT_EQ(obj->GetFreeBufferCount(), Net::kNumBuffers);
}

TEST_F(TestMessage, write_read)
{
    Net::Message *message;

    uint8_t writeBuffer[1024];

    uint8_t readBuffer[1024];

    for (unsigned i = 0; i < sizeof(writeBuffer); i++)
    {
        writeBuffer[i] = static_cast<uint8_t>(rand());
    }

    message = obj->New(Net::Message::kTypeIp6, 0);

    EXPECT_TRUE(message);

    EXPECT_EQ(message->SetLength(sizeof(writeBuffer)), VC_NET_ERROR_NONE);

    EXPECT_EQ(message->Write(0, sizeof(writeBuffer), writeBuffer), sizeof(writeBuffer));

    EXPECT_EQ(message->Read(0, sizeof(readBuffer), readBuffer), sizeof(readBuffer));

    EXPECT_EQ(memcmp(writeBuffer, readBuffer, sizeof(writeBuffer)), 0);

    EXPECT_EQ(message->GetLength(), sizeof(writeBuffer));

    message->Free();

    EXPECT_EQ(obj->GetFreeBufferCount(), Net::kNumBuffers);
}

bool VerifyMessageQueueContent(Net::MessageQueue &aMessageQueue, int aExpectedLength, ...)
{
    if (aExpectedLength < 0)
    {
        return false;
    }

    va_list args;
    bool res = true;
    Net::Message *message;
    Net::Message *msgArg;

    va_start(args, aExpectedLength);

    if (aExpectedLength == 0)
    {
        message = aMessageQueue.GetHead();

        if (message != NULL)
        {
            res = false; /* message queue is not empty when expected len is zero */
        }
    }
    else
    {
        for (message = aMessageQueue.GetHead(); message != NULL; message = message->GetNext())
        {
            msgArg = va_arg(args, Net::Message *);

            if (msgArg != message)
            {
                res = false; /* message queue content does not match what is expected */
                break;
            }

            aExpectedLength--;
        }

        if (aExpectedLength != 0)
        {
            res = false; /* message queue contains less entries than expected */
        }
    }

    va_end(args);

    return res;
}

TEST_F(TestMessage, message_queue)
{
    Net::MessageQueue messageQueue;
    Net::Message *msg[kNumTestMessages];
    uint16_t msgCount, bufferCount;

    for (int i = 0; i < kNumTestMessages; i++)
    {
        msg[i] = obj->New(Net::Message::kTypeIp6, 0);
        EXPECT_TRUE(msg[i]);
    }

    EXPECT_TRUE(VerifyMessageQueueContent(messageQueue, 0));

    // Enqueue 1 message and remove it
    EXPECT_EQ(messageQueue.Enqueue(*msg[0]), VC_NET_ERROR_NONE);
    EXPECT_TRUE(VerifyMessageQueueContent(messageQueue, 1, msg[0]));
    EXPECT_EQ(messageQueue.Dequeue(*msg[0]), VC_NET_ERROR_NONE);
    EXPECT_TRUE(VerifyMessageQueueContent(messageQueue, 0));

    // Enqueue 1 message at head and remove it
    EXPECT_EQ(messageQueue.Enqueue(*msg[0], Net::MessageQueue::kQueuePositionHead), VC_NET_ERROR_NONE);
    EXPECT_TRUE(VerifyMessageQueueContent(messageQueue, 1, msg[0]));
    EXPECT_EQ(messageQueue.Dequeue(*msg[0]), VC_NET_ERROR_NONE);
    EXPECT_TRUE(VerifyMessageQueueContent(messageQueue, 0));

    // Enqueue 5 messages
    EXPECT_EQ(messageQueue.Enqueue(*msg[0]), VC_NET_ERROR_NONE);
    EXPECT_TRUE(VerifyMessageQueueContent(messageQueue, 1, msg[0]));
    EXPECT_EQ(messageQueue.Enqueue(*msg[1]), VC_NET_ERROR_NONE);
    EXPECT_TRUE(VerifyMessageQueueContent(messageQueue, 2, msg[0], msg[1]));
    EXPECT_EQ(messageQueue.Enqueue(*msg[2]), VC_NET_ERROR_NONE);
    EXPECT_TRUE(VerifyMessageQueueContent(messageQueue, 3, msg[0], msg[1], msg[2]));
    EXPECT_EQ(messageQueue.Enqueue(*msg[3]), VC_NET_ERROR_NONE);
    EXPECT_TRUE(VerifyMessageQueueContent(messageQueue, 4, msg[0], msg[1], msg[2], msg[3]));
    EXPECT_EQ(messageQueue.Enqueue(*msg[4]), VC_NET_ERROR_NONE);
    EXPECT_TRUE(VerifyMessageQueueContent(messageQueue, 5, msg[0], msg[1], msg[2], msg[3], msg[4]));

    // Check the GetInfo()
    messageQueue.GetInfo(msgCount, bufferCount);
    EXPECT_EQ(msgCount, 5);

    // Remove from head
    EXPECT_EQ(messageQueue.Dequeue(*msg[0]), VC_NET_ERROR_NONE);
    EXPECT_TRUE(VerifyMessageQueueContent(messageQueue, 4, msg[1], msg[2], msg[3], msg[4]));

    // Remove a message in middle
    EXPECT_EQ(messageQueue.Dequeue(*msg[3]), VC_NET_ERROR_NONE);
    EXPECT_TRUE(VerifyMessageQueueContent(messageQueue, 3, msg[1], msg[2], msg[4]));

    // Remove from tail
    EXPECT_EQ(messageQueue.Dequeue(*msg[4]), VC_NET_ERROR_NONE);
    EXPECT_TRUE(VerifyMessageQueueContent(messageQueue, 2, msg[1], msg[2]));

    // Add after remove
    EXPECT_EQ(messageQueue.Enqueue(*msg[0]), VC_NET_ERROR_NONE);
    EXPECT_TRUE(VerifyMessageQueueContent(messageQueue, 3, msg[1], msg[2], msg[0]));
    EXPECT_EQ(messageQueue.Enqueue(*msg[3]), VC_NET_ERROR_NONE);
    EXPECT_TRUE(VerifyMessageQueueContent(messageQueue, 4, msg[1], msg[2], msg[0], msg[3]));

    // Remove from middle
    EXPECT_EQ(messageQueue.Dequeue(*msg[2]), VC_NET_ERROR_NONE);
    EXPECT_TRUE(VerifyMessageQueueContent(messageQueue, 3, msg[1], msg[0], msg[3]));

    // Add to head
    EXPECT_EQ(messageQueue.Enqueue(*msg[2], Net::MessageQueue::kQueuePositionHead), VC_NET_ERROR_NONE);
    EXPECT_TRUE(VerifyMessageQueueContent(messageQueue, 4, msg[2], msg[1], msg[0], msg[3]));

    // Remove from head
    EXPECT_EQ(messageQueue.Dequeue(*msg[2]), VC_NET_ERROR_NONE);
    EXPECT_TRUE(VerifyMessageQueueContent(messageQueue, 3, msg[1], msg[0], msg[3]));

    // Remove from head
    EXPECT_EQ(messageQueue.Dequeue(*msg[1]), VC_NET_ERROR_NONE);
    EXPECT_TRUE(VerifyMessageQueueContent(messageQueue, 2, msg[0], msg[3]));

    // Add to head
    EXPECT_EQ(messageQueue.Enqueue(*msg[1], Net::MessageQueue::kQueuePositionHead), VC_NET_ERROR_NONE);
    EXPECT_TRUE(VerifyMessageQueueContent(messageQueue, 3, msg[1], msg[0], msg[3]));

    // Add to tail
    EXPECT_EQ(messageQueue.Enqueue(*msg[2], Net::MessageQueue::kQueuePositionTail), VC_NET_ERROR_NONE);
    EXPECT_TRUE(VerifyMessageQueueContent(messageQueue, 4, msg[1], msg[0], msg[3], msg[2]));

    // Remove all messages
    EXPECT_EQ(messageQueue.Dequeue(*msg[3]), VC_NET_ERROR_NONE);
    EXPECT_TRUE(VerifyMessageQueueContent(messageQueue, 3, msg[1], msg[0], msg[2]));
    EXPECT_EQ(messageQueue.Dequeue(*msg[1]), VC_NET_ERROR_NONE);
    EXPECT_TRUE(VerifyMessageQueueContent(messageQueue, 2, msg[0], msg[2]));
    EXPECT_EQ(messageQueue.Dequeue(*msg[2]), VC_NET_ERROR_NONE);
    EXPECT_TRUE(VerifyMessageQueueContent(messageQueue, 1, msg[0]));
    EXPECT_EQ(messageQueue.Dequeue(*msg[0]), VC_NET_ERROR_NONE);
    EXPECT_TRUE(VerifyMessageQueueContent(messageQueue, 0));

    // Check the failure cases: Enqueue an already queued message, or dequeue
    // a message not in the queue.
    EXPECT_EQ(messageQueue.Enqueue(*msg[0]), VC_NET_ERROR_NONE);
    EXPECT_TRUE(VerifyMessageQueueContent(messageQueue, 1, msg[0]));
    EXPECT_EQ(messageQueue.Enqueue(*msg[0]), VC_NET_ERROR_ALREADY);
    EXPECT_EQ(messageQueue.Dequeue(*msg[1]), VC_NET_ERROR_NOT_FOUND);

    // Remove all messages
    EXPECT_EQ(messageQueue.Dequeue(*msg[0]), VC_NET_ERROR_NONE);
    EXPECT_TRUE(VerifyMessageQueueContent(messageQueue, 0));

    // Free all messages from message pool
    for (int i = 0; i < kNumTestMessages; i++)
    {
        obj->Free(msg[i]);
    }

    EXPECT_EQ(obj->GetFreeBufferCount(), Net::kNumBuffers);
}

bool VerifyMessageQueueContentUsingApi(vcNetMessageQueue *aQueue, int aExpectedLength, ...)
{
    if (aExpectedLength < 0)
    {
        return false;
    }

    va_list args;
    bool res = true;
    vcNetMessage *message;
    vcNetMessage *msgArg;

    va_start(args, aExpectedLength);

    if (aExpectedLength == 0)
    {
        message = vcNetMessageQueueGetHead(aQueue);

        if (message != NULL)
        {
            res = false; /* message queue is not empty when expected len is zero. */
        }
    }
    else
    {
        for (message = vcNetMessageQueueGetHead(aQueue);
             message != NULL;
             message = vcNetMessageQueueGetNext(aQueue, message))
        {
            msgArg = va_arg(args, vcNetMessage *);

            if (msgArg != message)
            {
                res = false; /* message queue content does not match what is expected. */
                break;
            }

            aExpectedLength--;
        }

        if (aExpectedLength != 0)
        {
            res = false; /* message queue contains less entries than expected. */
        }
    }

    va_end(args);

    return res;
}

TEST_F(TestMessage, message_queue_apis)
{
    vcNetMessage *msg[kNumTestMessages];
    vcNetMessage *message;
    vcNetMessageQueue queue, queue2;

    for (int i = 0; i < kNumTestMessages; i++)
    {
        msg[i] = static_cast<vcNetMessage *>(obj->New(Net::Message::kTypeIp6, 0));
        EXPECT_TRUE(msg[i]);
    }

    vcNetMessageQueueInit(&queue);
    vcNetMessageQueueInit(&queue2);

    // Check an empty queue.
    EXPECT_TRUE(VerifyMessageQueueContentUsingApi(&queue, 0));

    // Add message to the queue and check the content
    EXPECT_EQ(vcNetMessageQueueEnqueue(&queue, msg[0]), VC_NET_ERROR_NONE);
    EXPECT_TRUE(VerifyMessageQueueContentUsingApi(&queue, 1, msg[0]));
    EXPECT_EQ(vcNetMessageQueueEnqueue(&queue, msg[1]), VC_NET_ERROR_NONE);
    EXPECT_TRUE(VerifyMessageQueueContentUsingApi(&queue, 2, msg[0], msg[1]));
    EXPECT_EQ(vcNetMessageQueueEnqueueAtHead(&queue, msg[2]), VC_NET_ERROR_NONE);
    EXPECT_TRUE(VerifyMessageQueueContentUsingApi(&queue, 3, msg[2], msg[0], msg[1]));
    EXPECT_EQ(vcNetMessageQueueEnqueue(&queue, msg[3]), VC_NET_ERROR_NONE);
    EXPECT_TRUE(VerifyMessageQueueContentUsingApi(&queue, 4, msg[2], msg[0], msg[1], msg[3]));

    // Remove elements and check the content
    EXPECT_EQ(vcNetMessageQueueDequeue(&queue, msg[1]), VC_NET_ERROR_NONE);
    EXPECT_TRUE(VerifyMessageQueueContentUsingApi(&queue, 3, msg[2], msg[0], msg[3]));
    EXPECT_EQ(vcNetMessageQueueDequeue(&queue, msg[0]), VC_NET_ERROR_NONE);
    EXPECT_TRUE(VerifyMessageQueueContentUsingApi(&queue, 2, msg[2], msg[3]));
    EXPECT_EQ(vcNetMessageQueueDequeue(&queue, msg[3]), VC_NET_ERROR_NONE);
    EXPECT_TRUE(VerifyMessageQueueContentUsingApi(&queue, 1, msg[2]));

    // Check the expected failure cases for the enqueue and dequeue:
    EXPECT_EQ(vcNetMessageQueueEnqueue(&queue, msg[2]), VC_NET_ERROR_ALREADY);
    EXPECT_EQ(vcNetMessageQueueDequeue(&queue, msg[0]), VC_NET_ERROR_NOT_FOUND);

    // Check the failure cases for otMessageQueueGetNext()
    message = vcNetMessageQueueGetNext(&queue, NULL);
    EXPECT_FALSE(message);
    message = vcNetMessageQueueGetNext(&queue, msg[1]);
    EXPECT_FALSE(message);

    // Check the failure case when attempting to do vcNetMessageQueueGetNext()
    // but passing in a wrong queue pointer.
    EXPECT_EQ(vcNetMessageQueueEnqueue(&queue2, msg[0]), VC_NET_ERROR_NONE);
    EXPECT_TRUE(VerifyMessageQueueContentUsingApi(&queue2, 1, msg[0]));
    EXPECT_EQ(vcNetMessageQueueEnqueue(&queue2, msg[1]), VC_NET_ERROR_NONE);
    EXPECT_TRUE(VerifyMessageQueueContentUsingApi(&queue2, 2, msg[0], msg[1]));

    message = vcNetMessageQueueGetNext(&queue2, msg[0]);
    EXPECT_EQ(message, msg[1]);
    message = vcNetMessageQueueGetNext(&queue, msg[0]);
    EXPECT_FALSE(message);

    // Remove all element and make sure queue is empty
    EXPECT_EQ(vcNetMessageQueueDequeue(&queue, msg[2]), VC_NET_ERROR_NONE);
    EXPECT_TRUE(VerifyMessageQueueContentUsingApi(&queue, 0));

    EXPECT_EQ(vcNetMessageQueueDequeue(&queue2, msg[0]), VC_NET_ERROR_NONE);
    EXPECT_TRUE(VerifyMessageQueueContentUsingApi(&queue2, 1, msg[1]));
    EXPECT_EQ(vcNetMessageQueueDequeue(&queue2, msg[1]), VC_NET_ERROR_NONE);
    EXPECT_TRUE(VerifyMessageQueueContentUsingApi(&queue2, 0));

    // Free all messages from message pool
    for (int i = 0; i < kNumTestMessages; i++)
    {
        obj->Free(static_cast<Net::Message *>(msg[i]));
    }

    EXPECT_EQ(obj->GetFreeBufferCount(), Net::kNumBuffers);
}
