#ifndef VCOS_MSG_H
#define VCOS_MSG_H

#include <stdint.h>

#include <vcos/types.h>

#ifdef __cplusplus
extern "C" {
#endif

int vcMsgReceive(vcMsg *aMsg);

int vcMsgSend(vcMsg *aMsg, vcKernelPid aPid);

int vcMsgSendReceive(vcMsg *aMsg, vcMsg *aReply, vcKernelPid aPid);

int vcMsgReply(vcMsg *aMsg, vcMsg *aReply);

void vcMsgActiveThreadQueuePrint(void);

#ifdef __cplusplus
}
#endif

#endif /* VCOS_MSG_H */
