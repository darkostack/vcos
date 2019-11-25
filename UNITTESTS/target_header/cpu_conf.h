#ifndef CPU_CONF_H
#define CPU_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

#define CPU_ISR_STACKSIZE (1024)
#define CPU_DEFAULT_IRQ_PRIO (63U)
#define CPU_IRQ_NUMOF (64U)

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
