#ifndef CPU_CONF_H
#define CPU_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Default stack size configurations
 */
#define THREAD_EXTRA_STACKSIZE_PRINTF (512)
#define THREAD_STACKSIZE_DEFAULT (1024)
#define THREAD_STACKSIZE_IDLE (256)
#define THREAD_STACKSIZE_MAIN (THREAD_STACKSIZE_DEFAULT + THREAD_EXTRA_STACKSIZE_PRINTF)
#define ISR_STACKSIZE (1024)

/* CPU interrupt priority configuration */
#define CPU_DEFAULT_IRQ_PRIO (63U)
#define CPU_IRQ_NUMOF (64U)

#include "sirius.h"
#include "sirius_bitfields.h"

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
