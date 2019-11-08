#ifndef IRQ_H
#define IRQ_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

unsigned irqDisable(void);

unsigned irqEnable(void);

void irqRestore(unsigned aState);

int irqIsIn(void);

#ifdef __cplusplus
}
#endif

#endif /* IRQ_H */
