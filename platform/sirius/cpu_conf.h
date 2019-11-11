#ifndef CPU_CONF_H
#define CPU_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* CPU interrupt priority configuration */
#define ISR_STACKSIZE (1024)

#define CPU_DEFAULT_IRQ_PRIO (63U)
#define CPU_IRQ_NUMOF (64U)

#include "sirius.h"
#include "sirius_bitfields.h"

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
