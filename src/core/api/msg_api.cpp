#include <stdio.h>

#include <vcos/msg.h>
#include <vcos/irq.h>

#include "common/msg.hpp"
#include "common/locator-getters.hpp"

using namespace vc;

void vcMsgActiveThreadQueuePrint(void)
{
    unsigned state = irqDisable();

    Thread *thread = Instance::Get().Get<ThreadScheduler>().GetSchedActiveThread();
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
