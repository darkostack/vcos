#ifndef CPU_H
#define CPU_H

#include <stdio.h>

#include "irq.h"
#include "cpu_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Interrupt stack canary value 0xe7fe is the ARM Thumb machine code equivalent
 * of asm("bl #-2\n") or 'while (1);', i.e. an infinite loop
 */
#define STACK_CANARY_WORD   (0xE7FEE7FEu)

void cpuInit(void);

void cortexmInit(void);

static inline void cpuPrintLastInstruction(void)
{
    uint32_t *lr_ptr;
    __asm__ __volatile__("mov %0, lr" : "=r"(lr_ptr));
    printf("%p\n", (void*) lr_ptr);
}

static inline void cortexmSleepUntilEvent(void)
{
    __WFE();
}

static inline void cortexmSleep(int aDeep)
{
    if (aDeep) {
        SCB->SCR |=  (SCB_SCR_SLEEPDEEP_Msk);
    } else {
        SCB->SCR &= ~(SCB_SCR_SLEEPDEEP_Msk);
    }

    /* ensure that all memory accesses have completed and trigger sleeping */
    unsigned state = irqDisable();
    __DSB();
    __WFI();
    irqRestore(state);
}

/**
 * Trigger a conditional context scheduler run / context switch
 */
static inline void cortexmIsrEnd(void)
{
    /* TODO */
}

static inline void cpuJumpToImage(uint32_t aImageAddress)
{
    /* Disable IRQ */
    __disable_irq();

    /* set MSP */
    __set_MSP(*(uint32_t*)aImageAddress);

    /* skip stack pointer */
    aImageAddress += 4;

    /* load the images reset_vector address */
    uint32_t destination_address = *(uint32_t*)aImageAddress;

    /* Make sure the Thumb State bit is set. */
    destination_address |= 0x1;

    /* Branch execution */
    __asm("BX %0" :: "r" (destination_address));
}

static inline uint32_t cpuGetImageBaseAddr(void)
{
    return SCB->VTOR;
}

#ifdef __cplusplus
}
#endif

#endif /* CPU_H */
