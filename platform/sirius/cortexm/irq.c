#include <stdint.h>

#include "irq.h"
#include "cpu.h"

unsigned int irqDisable(void)
{
    uint32_t mask = __get_PRIMASK();
    __disable_irq();
    return mask;
}

__attribute__((used)) unsigned int irqEnable(void)
{
    __enable_irq();
    return __get_PRIMASK();
}

void irqRestore(unsigned int aState)
{
    __set_PRIMASK(aState);
}

/**
 * See if the current context is inside an ISR
 */
int irqIsIn(void)
{
    return (__get_IPSR() & 0xFF);
}
