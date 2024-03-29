#include <inttypes.h>

#include <vcos/irq.h>

#include "common/locator-getters.hpp"
#include "common/msg.hpp"

#define ENABLE_DEBUG (0)
#include "common/debug.h"

namespace vc {

int Msg::Send(vcKernelPid aTargetPid, bool aBlock, unsigned aState)
{
    if (!vcThreadPidIsValid(aTargetPid))
    {
        DEBUG("Msg::Send() aTargetPid is invalid, continuing anyways\r\n");
    }

    Thread *target = Get<ThreadScheduler>().GetSchedThreads(aTargetPid);

    this->mSenderPid = Get<ThreadScheduler>().GetSchedActivePid();

    if (target == NULL)
    {
        DEBUG("Msg::Send() target thread does not exist\r\n");
        vcIrqRestore(aState);
        return -1;
    }

    Thread *me = Get<ThreadScheduler>().GetSchedActiveThread();

    DEBUG("Msg::Send() Sending from %" PRIkernel_pid " to %" PRIkernel_pid
          ". block=%i src->state=%i target->state=%i\r\n",
          Get<ThreadScheduler>().GetSchedActivePid(), aTargetPid, aBlock, me->mStatus, target->mStatus);

    if (target->mStatus != THREAD_STATUS_RECEIVE_BLOCKED)
    {
        DEBUG("Msg::Send() Target %" PRIkernel_pid " is not RECEIVE_BLOCKED.\r\n", aTargetPid);

        if (this->QueueMsg(target))
        {
            DEBUG("Msg::Send() Target %" PRIkernel_pid " has a mMsgQueue. Queueing message.\r\n", aTargetPid);

            vcIrqRestore(aState);

            if (me->mStatus == THREAD_STATUS_REPLY_BLOCKED)
            {
                ThreadScheduler::YieldHigher();
            }
            return 1;
        }

        if (!aBlock)
        {
            DEBUG("Msg::Send() %" PRIkernel_pid ": Receiver not waiting, block=%u\r\n", me->mPid, aBlock);
            vcIrqRestore(aState);
            return 0;
        }

        DEBUG("Msg::Send() %" PRIkernel_pid ": going send blocked.\r\n", me->mPid);

        me->mWaitData = static_cast<void *>(this);

        int newStatus;

        if (me->mStatus == THREAD_STATUS_REPLY_BLOCKED)
        {
            newStatus = THREAD_STATUS_REPLY_BLOCKED;
        }
        else
        {
            newStatus = THREAD_STATUS_SEND_BLOCKED;
        }

        Get<ThreadScheduler>().SetStatus(me, static_cast<vcThreadStatus>(newStatus));

        me->AddToList(&target->GetMsgWaiters());

        vcIrqRestore(aState);

        ThreadScheduler::YieldHigher();

        DEBUG("Msg::Send() %" PRIkernel_pid ": Back from send block.\r\n", me->mPid);
    }
    else
    {
        DEBUG("Msg::Send() %" PRIkernel_pid ": Directy msg copy from %" PRIkernel_pid " to %" PRIkernel_pid ".\r\n",
              me->mPid, Get<ThreadScheduler>().GetSchedActivePid(), aTargetPid);

        /* copy msg to target */
        Msg *targetMessage = static_cast<Msg *>(target->mWaitData);

        *targetMessage = *this;

        Get<ThreadScheduler>().SetStatus(target, THREAD_STATUS_PENDING);

        vcIrqRestore(aState);

        ThreadScheduler::YieldHigher();
    }

    return 1;
}

int Msg::Receive(int aBlock)
{
    unsigned state = vcIrqDisable();

    Thread *me = Get<ThreadScheduler>().GetSchedActiveThread();

    int queueIndex = -1;

    if (me->HasMsgQueued())
    {
        queueIndex = me->GetMsgQueue().Get();
    }

    /* no message, fail */
    if ((!aBlock) && ((!me->GetMsgWaitersNext()) && (queueIndex == -1)))
    {
        vcIrqRestore(state);
        return -1;
    }

    if (queueIndex >= 0)
    {
        DEBUG("Msg::Receive() %" PRIkernel_pid ": We've got a queued message.\r\n",
              Get<ThreadScheduler>().GetSchedActivePid());
        *this = me->GetMsgArray(queueIndex);
    }
    else
    {
        me->mWaitData = static_cast<void *>(this);
    }

    List *next = (me->GetMsgWaiters()).RemoveHead();

    if (next == NULL)
    {
        DEBUG("Msg::Receive() %" PRIkernel_pid ": No thread in waiting list.\r\n",
              Get<ThreadScheduler>().GetSchedActivePid());

        if (queueIndex < 0)
        {
            DEBUG("Msg::Receive() %" PRIkernel_pid ": No msg in queue. Going blocked.\r\n",
                  Get<ThreadScheduler>().GetSchedActivePid());

            Get<ThreadScheduler>().SetStatus(me, THREAD_STATUS_RECEIVE_BLOCKED);

            vcIrqRestore(state);

            ThreadScheduler::YieldHigher();

            /* sender copied message */
            assert(Get<ThreadScheduler>().GetSchedActiveThread()->mStatus != THREAD_STATUS_RECEIVE_BLOCKED);
        }
        else
        {
            vcIrqRestore(state);
        }

        return 1;
    }
    else
    {
        DEBUG("Msg::Receive() %" PRIkernel_pid ": waking up waiting thread.\r\n",
              Get<ThreadScheduler>().GetSchedActivePid());

        Thread *sender = Get<ThreadScheduler>().GetThreadPointerFromList(next);

        Msg *tmp = NULL;

        if (queueIndex >= 0)
        {
            /* We've already got a message from the queue. As there is a
             * waiter, take it's message into the just freed queue space.
             */
            tmp = &(me->GetMsgArray(me->GetMsgQueue().Put()));
        }

        /* copy msg */
        Msg *senderMsg = static_cast<Msg *>(sender->mWaitData);

        if (tmp != NULL)
        {
            *tmp  = *senderMsg;
            *this = *tmp;
        }
        else
        {
            *this = *senderMsg;
        }

        /* remove sender from queue */
        uint16_t senderPrio = VCOS_CONFIG_THREAD_PRIORITY_IDLE;

        if (sender->mStatus != THREAD_STATUS_REPLY_BLOCKED)
        {
            sender->mWaitData = NULL;

            Get<ThreadScheduler>().SetStatus(sender, THREAD_STATUS_PENDING);

            senderPrio = sender->mPriority;
        }

        vcIrqRestore(state);

        if (senderPrio < VCOS_CONFIG_THREAD_PRIORITY_IDLE)
        {
            Get<ThreadScheduler>().Switch(senderPrio);
        }

        return 1;
    }

    DEBUG("This should have never been reached!\r\n");
}

int Msg::QueueMsg(Thread *aTarget)
{
    int n = aTarget->GetMsgQueue().Put();

    if (n < 0)
    {
        DEBUG("Msg::QueueMsg() message queue is full (or there is none)\r\n");
        return 0;
    }

    DEBUG("Msg::QueueMsg() queuing message\r\n");

    Msg *dest = &aTarget->GetMsgArray(n);

    *dest = *this;

    return 1;
}

int Msg::Send(vcKernelPid aTargetPid)
{
    if (vcIrqIsIn())
    {
        return SendInt(aTargetPid);
    }

    if (Get<ThreadScheduler>().GetSchedActivePid() == aTargetPid)
    {
        return SendToSelf();
    }

    return Send(aTargetPid, true, vcIrqDisable());
}

int Msg::TrySend(vcKernelPid aTargetPid)
{
    if (vcIrqIsIn())
    {
        return SendInt(aTargetPid);
    }

    if (Get<ThreadScheduler>().GetSchedActivePid() == aTargetPid)
    {
        return SendToSelf();
    }

    return Send(aTargetPid, false, vcIrqDisable());
}

int Msg::SendToSelf(void)
{
    unsigned state = vcIrqDisable();

    mSenderPid = Get<ThreadScheduler>().GetSchedActivePid();

    int res = this->QueueMsg(Get<ThreadScheduler>().GetSchedActiveThread());

    vcIrqRestore(state);

    return res;
}

int Msg::SendInt(vcKernelPid aTargetPid)
{
    if (!vcThreadPidIsValid(aTargetPid))
    {
        DEBUG("Msg::SendInt() aTargetPid is invalid, continuing anyways.\r\n");
    }

    Thread *target = Get<ThreadScheduler>().GetSchedActiveThread();

    if (target == NULL)
    {
        DEBUG("Msg::SendInt() target thread does not exist\r\n");
        return -1;
    }

    mSenderPid = KERNEL_PID_ISR;

    if (target->mStatus == THREAD_STATUS_RECEIVE_BLOCKED)
    {
        DEBUG("Msg::SendInt() direct msg copy from %" PRIkernel_pid " to %" PRIkernel_pid ".\r\n",
              Get<ThreadScheduler>().GetSchedActivePid(), aTargetPid);

        /* copy msg to target */
        Msg *targetMessage = static_cast<Msg *>(target->mWaitData);

        *targetMessage = *this;

        Get<ThreadScheduler>().SetStatus(target, THREAD_STATUS_PENDING);
        Get<ThreadScheduler>().SetContexSwitchRequest(1);

        return 1;
    }
    else
    {
        DEBUG("Msg::SendInt() receiver not waiting.\r\n");
        return this->QueueMsg(target);
    }
}

int Msg::SendReceive(Msg *aReply, vcKernelPid aTargetPid)
{
    assert(Get<ThreadScheduler>().GetSchedActivePid() != aTargetPid);

    unsigned state = vcIrqDisable();

    Thread *me = Get<ThreadScheduler>().GetSchedActiveThread();

    Get<ThreadScheduler>().SetStatus(me, THREAD_STATUS_REPLY_BLOCKED);

    me->mWaitData = static_cast<void *>(aReply);

    /* we re-use (abuse) reply for sending, because wait_data might be
     * overwritten if the target is not in RECEIVE_BLOCKED */

    *aReply = *this;

    /* Send() blocks until reply received */
    return aReply->Send(aTargetPid, true, state);
}

int Msg::Reply(Msg *aReply)
{
    unsigned state = vcIrqDisable();

    Thread *target = Get<ThreadScheduler>().GetSchedThreads(this->mSenderPid);

    assert(target != NULL);

    if (target->mStatus != THREAD_STATUS_REPLY_BLOCKED)
    {
        DEBUG("Msg::Reply() %" PRIkernel_pid ": Target %" PRIkernel_pid " not waiting for reply.\r\n",
              Get<ThreadScheduler>().GetSchedActivePid(), target->mPid);

        vcIrqRestore(state);

        return -1;
    }

    DEBUG("Msg::Reply() %" PRIkernel_pid ": Direct msg copy.\r\n", Get<ThreadScheduler>().GetSchedActivePid());

    /* copy msg to target */
    Msg *targetMessage = static_cast<Msg *>(target->mWaitData);

    *targetMessage = *aReply;

    Get<ThreadScheduler>().SetStatus(target, THREAD_STATUS_PENDING);

    uint16_t targetPrio = target->mPriority;

    vcIrqRestore(state);

    Get<ThreadScheduler>().Switch(targetPrio);

    return 1;
}

int Msg::ReplyInt(Msg *aReply)
{
    Thread *target = Get<ThreadScheduler>().GetSchedThreads(this->mSenderPid);

    if (target->mStatus != THREAD_STATUS_REPLY_BLOCKED)
    {
        DEBUG("Msg::ReplyInt() %" PRIkernel_pid ": target %" PRIkernel_pid " not waiting for reply.\r\n",
              Get<ThreadScheduler>().GetSchedActivePid(), target->mPid);

        return -1;
    }

    Msg *targetMessage = static_cast<Msg *>(target->mWaitData);

    *targetMessage = *aReply;

    Get<ThreadScheduler>().SetStatus(target, THREAD_STATUS_PENDING);

    Get<ThreadScheduler>().SetContexSwitchRequest(1);

    return 1;
}

int Msg::TryReceive(void)
{
    return Receive(0);
}

int Msg::Receive(void)
{
    return Receive(1);
}

int Msg::Avail(void)
{
    DEBUG("Msg::Avail() %" PRIkernel_pid "\r\n", Get<ThreadScheduler>().GetSchedActivePid());

    Thread *me = Get<ThreadScheduler>().GetSchedActiveThread();

    int queueIndex = -1;

    if (me->HasMsgQueued())
    {
        queueIndex = me->GetMsgQueue().Avail();
    }

    return queueIndex;
}

void Msg::InitQueue(int aNum)
{
    Thread *me = Get<ThreadScheduler>().GetSchedActiveThread();

    me->mMsgArray = this;

    me->GetMsgQueue().Init(aNum);
}

} // namespace vc
