#include <stdint.h>

#include <vcos/cpu.h>
#include <vcos/irq.h>

unsigned int vcIrqDisable(void)
{
    uint32_t mask = __get_PRIMASK();
    __disable_irq();
    return mask;
}

__attribute__((used)) unsigned int vcIrqEnable(void)
{
    __enable_irq();
    return __get_PRIMASK();
}

void vcIrqRestore(unsigned int aState)
{
    __set_PRIMASK(aState);
}

/**
 * See if the current context is inside an ISR
 */
int vcIrqIsIn(void)
{
    return (__get_IPSR() & 0xFF);
}
