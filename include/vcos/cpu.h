#ifndef VCOS_CPU_H
#define VCOS_CPU_H

#include <stdio.h>

#include <vcos/irq.h>

#include "cpu_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This marker is used e.g. by 'Thread::SwitchContextExit()' to identify the stacks
 * beginning.
 */
#define STACK_MARKER (0x77777777)

/**
 * Initial program status register value for a newly created thread
 */
#define INITIAL_XPSR (0x01000000)

/**
 * ARM Cortex-M specific exception return value, that triggers the return to the
 * task mode stack pointer.
 */
#define EXCEPT_RET_TASK_MODE (0xfffffffd)

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

void vcCpuPrintLastInstruction(void);

void vcCpuSleepUntilEvent(void);

void vcCpuSleep(int aDeep);

void vcCpuJumpToImage(uint32_t aImageAddress);

uint32_t vcCpuGetImageBaseAddr(void);

void *vcCpuGetMsp(void);

void vcCpuTriggerPendSV(void);

void vcCpuSwitchContextExit(void);

#ifdef __cplusplus
}
#endif

#endif /* VCOS_CPU_H */
