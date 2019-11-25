#ifndef VCOS_NET_ERROR_H
#define VCOS_NET_ERROR_H

#include <vcos/net/types.h>

#ifdef __cplusplus
extern "C" {
#endif

const char *vcNetErrorToString(vcNetError aError);

#ifdef __cplusplus
}
#endif

#endif /* VCOS_NET_ERROR_H */
