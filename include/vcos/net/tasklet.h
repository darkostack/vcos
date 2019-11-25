#ifndef VCOS_NET_TASKLET_H
#define VCOS_NET_TASKLET_H

#include <vcos/instance.h>

#ifdef __cplusplus
extern "C" {
#endif

void vcNetTaskletProcess(vcInstance *aInstance);

bool vcNetTaskletsArePending(vcInstance *aInstance);

extern void vcNetTaskletsSignalPending(vcInstance *aInstance);

#ifdef __cplusplus
}
#endif

#endif /* VCOS_NET_TASKLET_H */
