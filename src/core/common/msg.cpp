#include <inttypes.h>

#include <vcos/irq.h>

#include "common/msg.hpp"
#include "common/locator-getters.hpp"

#define ENABLE_DEBUG (0)
#include "common/debug.h"

namespace vc {

int Msg::Send(Msg *aMsg, KernelPid aTargetPid, bool aBlock, unsigned aState)
{
    if (!pidIsValid(aTargetPid)) {
        DEBUG("Msg::Send() aTargetPid is invalid, continuing anyways\r\n");
    }

    Thread *target = Get<ThreadScheduler>().GetSchedThreads(aTargetPid);

    aMsg->SetSenderPid(Get<ThreadScheduler>().GetSchedActivePid());

    if (target == NULL) {
        DEBUG("Msg::Send() target thread does not exist\r\n");
        irqRestore(aState);
        return -1;
    }

    Thread *me = Get<ThreadScheduler>().GetSchedActiveThread();

    DEBUG("Msg::Send() Sending from %" PRIkernel_pid " to %" PRIkernel_pid
          ". block=%i src->state=%i target->state=%i\r\n",
          Get<ThreadScheduler>().GetSchedActivePid(), aTargetPid, aBlock,
          me->GetStatus(), target->GetStatus());

    if (target->GetStatus() != THREAD_STATUS_RECEIVE_BLOCKED) {
        DEBUG("Msg::Send() Target %" PRIkernel_pid " is not RECEIVE_BLOCKED.\r\n", aTargetPid);

        if (QueueMsg(target, aMsg)) {
            DEBUG("Msg::Send() Target %" PRIkernel_pid
                  " has a mMsgQueue. Queueing message.\r\n", aTargetPid);
            irqRestore(aState);
            if (me->GetStatus() == THREAD_STATUS_REPLY_BLOCKED) {
                vcThreadYieldHigher();
            }
            return 1;
        }

        if (!aBlock) {
            DEBUG("Msg::Send() %" PRIkernel_pid ": Receiver not waiting, block=%u\r\n",
                  me->GetPid(), aBlock);
            irqRestore(aState);
            return 0;
        }

        DEBUG("Msg::Send() %" PRIkernel_pid ": going send blocked.\r\n", me->GetPid());

        me->SetWaitData(reinterpret_cast<void *>(aMsg));

        int newStatus;

        if (me->GetStatus() == THREAD_STATUS_REPLY_BLOCKED) {
            newStatus = THREAD_STATUS_REPLY_BLOCKED;
        } else {
            newStatus = THREAD_STATUS_SEND_BLOCKED;
        }

        me->SetStatus((ThreadStatus)newStatus);

        me->AddToList(&target->GetMsgWaiters());

        irqRestore(aState);
        vcThreadYieldHigher();

        DEBUG("Msg::Send() %" PRIkernel_pid ": Back from send block.\r\n",
              me->GetPid());
    } else {
        DEBUG("Msg::Send() %" PRIkernel_pid ": Directy msg copy from %"
              PRIkernel_pid " to %" PRIkernel_pid ".\r\n",
              me->GetPid(), Get<ThreadScheduler>().GetSchedActivePid(), aTargetPid);
        /* copy msg to target */
        Msg *targetMessage = reinterpret_cast<Msg *>(target->GetWaitData());
        *targetMessage = *aMsg;
        target->SetStatus(THREAD_STATUS_PENDING);

        irqRestore(aState);
        vcThreadYieldHigher();
    }

    return 1;
}

int Msg::Receive(Msg *aMsg, int aBlock)
{
    unsigned state = irqDisable();

    DEBUG("Msg::Receive() %" PRIkernel_pid "\r\n", Get<ThreadScheduler>().GetSchedActivePid());

    Thread *me = Get<ThreadScheduler>().GetSchedActiveThread();

    int queueIndex = -1;

    if (me->HasMsgQueued()) {
        queueIndex = me->GetMsgQueue().Get();
    }

    /* no message, fail */
    if ((!aBlock) && ((!me->GetMsgWaitersNext()) && (queueIndex == -1))) {
        irqRestore(state);
        return -1;
    }

    if (queueIndex >= 0) {
        DEBUG("Msg::Receive() %" PRIkernel_pid ": We've got a queued message.\r\n",
              Get<ThreadScheduler>().GetSchedActivePid());
        *aMsg = me->GetMsgArray(queueIndex);
    } else {
        me->SetWaitData(reinterpret_cast<void *>(aMsg));
    }

    ListNode *next = RemoveHead(&me->GetMsgWaiters());

    if (next == NULL) {
        DEBUG("Msg::Receive() %" PRIkernel_pid ": No thread in waiting list.\r\n",
              Get<ThreadScheduler>().GetSchedActivePid());

        if (queueIndex < 0) {
            DEBUG("Msg::Receive() %" PRIkernel_pid ": No msg in queue. Going blocked.\r\n",
                  Get<ThreadScheduler>().GetSchedActivePid());
            me->SetStatus(THREAD_STATUS_RECEIVE_BLOCKED);

            irqRestore(state);
            vcThreadYieldHigher();

            /* sender copied message */
            assert(Get<ThreadScheduler>().GetSchedActiveThread()->GetStatus() != THREAD_STATUS_RECEIVE_BLOCKED);
        } else {
            irqRestore(state);
        }

        return 1;
    } else {
        DEBUG("Msg::Receive() %" PRIkernel_pid ": waking up waiting thread.\r\n",
              Get<ThreadScheduler>().GetSchedActivePid());

        Thread *sender = Get<ThreadScheduler>().GetThreadPointerFromList(next);

        if (queueIndex >= 0) {
            /* We've already got a message from the queue. As there is a
             * waiter, take it's message into the just freed queue space.
             */
            aMsg = &(me->GetMsgArray(me->GetMsgQueue().Put()));
        }

        /* copy msg */
        Msg *senderMsg = reinterpret_cast<Msg *>(sender->GetWaitData());
        *aMsg = *senderMsg;

        /* remove sender from queue */
        uint16_t senderPrio = VCOS_CONFIG_THREAD_PRIORITY_IDLE;
        if (sender->GetStatus() != THREAD_STATUS_REPLY_BLOCKED) {
            sender->SetWaitData(NULL);
            sender->SetStatus(THREAD_STATUS_PENDING);
            senderPrio = sender->GetPriority();
        }

        irqRestore(state);
        if (senderPrio < VCOS_CONFIG_THREAD_PRIORITY_IDLE) {
            Get<ThreadScheduler>().Switch(senderPrio);
        }
        return 1;
    }

    DEBUG("This should have never been reached!\r\n");
}

int Msg::QueueMsg(Thread *aTarget, const Msg *aMsg)
{
    int n = aTarget->GetMsgQueue().Put();
    if (n < 0) {
        DEBUG("Msg::QueueMsg() message queue is full (or there is none)\r\n");
        return 0;
    }
    DEBUG("Msg::QueueMsg() queuing message\r\n");
    Msg *dest = &aTarget->GetMsgArray(n);
    *dest = *aMsg;
    return 1;
}

int Msg::Send(KernelPid aTargetPid)
{
    if (irqIsIn()) {
        return SendInt(aTargetPid);
    }
    if (Get<ThreadScheduler>().GetSchedActivePid() == aTargetPid) {
        return SendToSelf();
    }
    return Send(this, aTargetPid, true, irqDisable());
}

int Msg::TrySend(KernelPid aTargetPid)
{
    if (irqIsIn()) {
        return SendInt(aTargetPid);
    }
    if (Get<ThreadScheduler>().GetSchedActivePid() == aTargetPid) {
        return SendToSelf();
    }
    return Send(this, aTargetPid, false, irqDisable());
}

int Msg::SendToSelf(void)
{
    unsigned state = irqDisable();

    SetSenderPid(Get<ThreadScheduler>().GetSchedActivePid());
    int res = QueueMsg(Get<ThreadScheduler>().GetSchedActiveThread(), this);

    irqRestore(state);
    return res;
}

int Msg::SendInt(KernelPid aTargetPid)
{
    if (!pidIsValid(aTargetPid)) {
        DEBUG("Msg::SendInt() aTargetPid is invalid, continuing anyways.\r\n");
    }

    Thread *target = Get<ThreadScheduler>().GetSchedActiveThread();

    if (target == NULL) {
        DEBUG("Msg::SendInt() target thread does not exist\r\n");
        return -1;
    }

    SetSenderPid(KERNEL_PID_ISR);

    if (target->GetStatus() == THREAD_STATUS_RECEIVE_BLOCKED) {
        DEBUG("Msg::SendInt() direct msg copy from %" PRIkernel_pid " to %"
              PRIkernel_pid ".\r\n", Get<ThreadScheduler>().GetSchedActivePid(), aTargetPid);

        /* copy msg to target */
        Msg *targetMessage = reinterpret_cast<Msg *>(target->GetWaitData());
        *targetMessage = *this;
        target->SetStatus(THREAD_STATUS_PENDING);
        Get<ThreadScheduler>().SetContexSwitchRequest(1);
        return 1;
    } else {
        DEBUG("Msg::SendInt() receiver not waiting.\r\n");
        return (QueueMsg(target, this));
    }
}

int Msg::SendReceive(Msg *aReply, KernelPid aTargetPid)
{
    assert(Get<ThreadScheduler>().GetSchedActivePid() != aTargetPid);
    unsigned state = irqDisable();
    Thread *me = Get<ThreadScheduler>().GetSchedActiveThread();
    me->SetStatus(THREAD_STATUS_REPLY_BLOCKED);
    me->SetWaitData(reinterpret_cast<void *>(aReply));

    /* we re-use (abuse) reply for sending, because wait_data might be
     * overwritten if the target is not in RECEIVE_BLOCKED */
    *aReply = *this;
    /* Send() blocks until reply received */
    return Send(aReply, aTargetPid, true, state);
}

int Msg::Reply(Msg *aReply)
{
    unsigned state = irqDisable();

    Thread *target = Get<ThreadScheduler>().GetSchedThreads(this->GetSenderPid());
    assert(target != NULL);

    if (target->GetStatus() != THREAD_STATUS_REPLY_BLOCKED) {
        DEBUG("Msg::Reply() %" PRIkernel_pid ": Target %" PRIkernel_pid
              " not waiting for reply.\r\n",
              Get<ThreadScheduler>().GetSchedActivePid(), target->GetPid());
        irqRestore(state);
        return -1;
    }

    DEBUG("Msg::Reply() %" PRIkernel_pid ": Direct msg copy.\r\n",
          Get<ThreadScheduler>().GetSchedActivePid());

    /* copy msg to target */
    Msg *targetMessage = reinterpret_cast<Msg *>(target->GetWaitData());
    *targetMessage = *aReply;
    target->SetStatus(THREAD_STATUS_PENDING);
    uint16_t targetPrio = target->GetPriority();
    irqRestore(state);
    Get<ThreadScheduler>().Switch(targetPrio);

    return 1;
}

int Msg::ReplyInt(Msg *aReply)
{
    Thread *target = Get<ThreadScheduler>().GetSchedThreads(this->GetSenderPid());

    if (target->GetStatus() != THREAD_STATUS_REPLY_BLOCKED) {
        DEBUG("Msg::ReplyInt() %" PRIkernel_pid ": target %" PRIkernel_pid
              " not waiting for reply.\r\n",
              Get<ThreadScheduler>().GetSchedActivePid(), target->GetPid());
        return -1;
    }

    Msg *targetMessage = reinterpret_cast<Msg *>(target->GetWaitData());
    *targetMessage = *aReply;
    target->SetStatus(THREAD_STATUS_PENDING);
    Get<ThreadScheduler>().SetContexSwitchRequest(1);

    return 1;
}

int Msg::TryReceive(void)
{
    return Receive(this, 0);
}

int Msg::Receive(void)
{
    return Receive(this, 1);
}

int Msg::Avail(void)
{
    DEBUG("Msg::Avail() %" PRIkernel_pid "\r\n", Get<ThreadScheduler>().GetSchedActivePid());

    Thread *me = Get<ThreadScheduler>().GetSchedActiveThread();

    int queueIndex = -1;

    if (me->HasMsgQueued()) {
        queueIndex = me->GetMsgQueue().Avail();
    }

    return queueIndex;
}

void Msg::InitQueue(int aNum)
{
    Thread *me = Get<ThreadScheduler>().GetSchedActiveThread();
    me->SetMsgArray(this);
    me->GetMsgQueue().Init(aNum);
}

void Msg::QueuePrint(void)
{
    unsigned state = irqDisable();

    Thread *thread = Get<ThreadScheduler>().GetSchedActiveThread();
    Cib *msgQueue = &thread->GetMsgQueue();
    Msg *msgArray = &thread->GetMsgArray(0);

    unsigned int i = msgQueue->GetReadCount() & msgQueue->GetMask();

    printf("Message queue of thread %" PRIkernel_pid "\r\n", thread->GetPid());
    printf("    size: %u (avail: %d)\r\n", msgQueue->GetMask() + 1, msgQueue->Avail());

    for (; i != (msgQueue->GetWriteCount() & msgQueue->GetMask());
         i = (i + 1) & msgQueue->GetMask()) {
        Msg *m = &msgArray[i];
        printf("    * %u: sender: %" PRIkernel_pid ", type: 0x%04" PRIu16
               ", content: %" PRIu32 " (%p)\r\n", i, m->GetSenderPid(), m->GetType(),
               m->GetContentValue(), m->GetContentPtr());
    }

    irqRestore(state);
}

} // namespace vc