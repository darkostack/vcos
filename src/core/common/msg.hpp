#ifndef MSG_HPP
#define MSG_HPP

#include <stdint.h>
#include <stdbool.h>

#include <vcos/types.h>

#include "vcos-core-config.h"

#include "common/locator.hpp"
#include "common/list.hpp"

#define KERNEL_PID_ISR (KERNEL_PID_LAST + 1)

namespace vc {

class Thread;

class Msg : public vcMsg, public InstanceLocator, public List
{
public:
    int Send(vcKernelPid aTargetPid, bool aBlock, unsigned aState);

    int Receive(int aBlock);

    int QueueMsg(Thread *aTarget);

    int Send(vcKernelPid aTargetPid);

    int TrySend(vcKernelPid aTargetPid);

    int SendToSelf(void);

    int SendInt(vcKernelPid aTargetPid);

    int SentByInt(void) { return mSenderPid == KERNEL_PID_ISR; }

    int Receive(void);

    int TryReceive(void);

    int SendReceive(Msg *aReply, vcKernelPid aTargetPid);

    int Reply(Msg *aReply);

    int ReplyInt(Msg *aReply);

    int Avail(void);

    void InitQueue(int aNum);
};

} //namespace vc

#endif /* MSG_HPP */
