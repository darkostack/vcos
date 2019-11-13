#ifndef MSG_HPP
#define MSG_HPP

#include <stdint.h>
#include <stdbool.h>

#include "vcos-core-config.h"

#include "common/locator.hpp"
#include "common/list.hpp"
#include "common/kernel_types.h"

#define KERNEL_PID_ISR (KERNEL_PID_LAST + 1)

namespace vc {

class Thread;

class Msg : public InstanceLocator, public List
{
public:
    Msg(Instance &aInstance)
        : InstanceLocator(aInstance)
    {
    }

    int Send(KernelPid aTargetPid);

    int TrySend(KernelPid aTargetPid);

    int SendToSelf(void);

    int SendInt(KernelPid aTargetPid);

    int SentByInt(void) { return mSenderPid == KERNEL_PID_ISR; }

    int Receive(void);

    int TryReceive(void);

    int SendReceive(Msg *aReply, KernelPid aTargetPid);

    int Reply(Msg *aReply);

    int ReplyInt(Msg *aReply);

    int Avail(void);

    void InitQueue(int aNum);

    void SetSenderPid(KernelPid aTargetPid) { mSenderPid = aTargetPid; }

    KernelPid GetSenderPid(void) { return mSenderPid; }

    uint16_t GetType(void) { return mType; }

    void SetType(uint16_t aType) { mType = aType; }

    void *GetContentPtr(void) { return mContent.mPtr; }

    void SetContentPtr(void *aPtr) { mContent.mPtr = aPtr; }

    uint32_t GetContentValue(void) { return mContent.mValue; }

    void SetContentValue(uint32_t aValue) { mContent.mValue = aValue; }

private:
    int Send(Msg *aMsg, KernelPid aTargetPid, bool aBlock, unsigned aState);

    int Receive(Msg *aMsg, int aBlock);

    int QueueMsg(Thread *aTarget, const Msg *aMsg);

    KernelPid mSenderPid;
    uint16_t mType;
    union {
        void *mPtr;
        uint32_t mValue;
    } mContent;
};

} //namespace vc

#endif /* MSG_HPP */
