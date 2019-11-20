#ifndef VCOS_CPU_H
#define VCOS_CPU_H

#include <stdio.h>

#include <vcos/irq.h>

#include "cpu_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Interrupt stack canary value 0xe7fe is the ARM Thumb machine code equivalent
 * of asm("bl #-2\n") or 'while (1);', i.e. an infinite loop
 */
#define STACK_CANARY_WORD   (0xE7FEE7FEu)

/**
 * Stack start and end address defined in linker script.
 */
extern uint32_t _estack;
extern uint32_t _sstack;

/**
 * Interrupt vector base address, defined by the linker
 */
extern const void *_isr_vectors;

/**
 * Global pointer to current active thread
 */
extern volatile void *gSchedActiveThread;

void vcCpuInit(void);

void vcCpuIsrEnd(void);

void vcThreadSchedulerRun(void);

static inline void vcCpuPrintLastInstruction(void)
{
    uint32_t *lr_ptr;
    __asm__ __volatile__("mov %0, lr" : "=r"(lr_ptr));
    printf("%p\n", (void*) lr_ptr);
}

static inline void vcCpuSleepUntilEvent(void)
{
    __WFE();
}

static inline void vcCpuSleep(int aDeep)
{
    if (aDeep) {
        SCB->SCR |=  (SCB_SCR_SLEEPDEEP_Msk);
    } else {
        SCB->SCR &= ~(SCB_SCR_SLEEPDEEP_Msk);
    }

    /* ensure that all memory accesses have completed and trigger sleeping */
    unsigned state = vcIrqDisable();
    __DSB();
    __WFI();
    vcIrqRestore(state);
}

static inline void vcCpuJumpToImage(uint32_t aImageAddress)
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

static inline uint32_t vcCpuGetImageBaseAddr(void)
{
    return SCB->VTOR;
}

#ifdef __cplusplus
}
#endif

#endif /* VCOS_CPU_H */
