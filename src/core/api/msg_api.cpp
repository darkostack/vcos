#include <stdio.h>

#include <vcos/irq.h>
#include <vcos/msg.h>

#include "common/locator-getters.hpp"
#include "common/msg.hpp"

using namespace vc;

int vcMsgReceive(vcMsg *aMsg)
{
    return (static_cast<Msg *>(aMsg))->Receive();
}

int vcMsgSend(vcMsg *aMsg, vcKernelPid aPid)
{
    return (static_cast<Msg *>(aMsg))->Send(aPid);
}

int vcMsgSendReceive(vcMsg *aMsg, vcMsg *aReply, vcKernelPid aPid)
{
    return (static_cast<Msg *>(aMsg))->SendReceive(static_cast<Msg *>(aReply), aPid);
}

int vcMsgReply(vcMsg *aMsg, vcMsg *aReply)
{
    return (static_cast<Msg *>(aMsg))->Reply(static_cast<Msg *>(aReply));
}

void vcMsgActiveThreadQueuePrint(void)
{
    unsigned state = vcIrqDisable();

    Thread *thread   = Instance::Get().Get<ThreadScheduler>().GetSchedActiveThread();
    Cib *   msgQueue = &thread->GetMsgQueue();
    Msg *   msgArray = &thread->GetMsgArray(0);

    unsigned int i = msgQueue->GetReadCount() & msgQueue->GetMask();

    printf("Message queue of thread %" PRIkernel_pid "\r\n", thread->mPid);
    printf("    size: %u (avail: %d)\r\n", msgQueue->GetMask() + 1, msgQueue->Avail());

    for (; i != (msgQueue->GetWriteCount() & msgQueue->GetMask()); i = (i + 1) & msgQueue->GetMask())
    {
        Msg *m = &msgArray[i];
        printf("    * %u: sender: %" PRIkernel_pid ", type: 0x%04" PRIu16 ", content: %" PRIu32 " (%p)\r\n", i,
               m->mSenderPid, m->mType, m->mContent.mValue, m->mContent.mPtr);
    }

    vcIrqRestore(state);
}
