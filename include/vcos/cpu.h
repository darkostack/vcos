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
#define STACK_CANARY_WORD (0xE7FEE7FEu)

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

void vcCpuPrintLastInstruction(void);

void vcCpuSleepUntilEvent(void);

void vcCpuSleep(int aDeep);

void vcCpuJumpToImage(uint32_t aImageAddress);

uint32_t vcCpuGetImageBaseAddr(void);

#ifdef __cplusplus
}
#endif

#endif /* VCOS_CPU_H */
