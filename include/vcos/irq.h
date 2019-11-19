#ifndef IRQ_H
#define IRQ_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

unsigned vcIrqDisable(void);

unsigned vcIrqEnable(void);

void vcIrqRestore(unsigned aState);

int vcIrqIsIn(void);

#ifdef __cplusplus
}
#endif

#endif /* IRQ_H */
