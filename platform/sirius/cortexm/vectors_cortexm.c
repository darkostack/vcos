#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <assert.h>

#include "cpu.h"
#include "vectors_cortexm.h"

/**
 * Memory markers, defined in the linker script
 */
extern uint32_t _sfixed;
extern uint32_t _efixed;
extern uint32_t _etext;
extern uint32_t _srelocate;
extern uint32_t _erelocate;
extern uint32_t _szero;
extern uint32_t _ezero;
extern uint32_t _sstack;
extern uint32_t _estack;
extern uint8_t _sram;
extern uint8_t _eram;

/**
 * Allocation of the interrupt stack
 */
__attribute__((used,section(".isr_stack"))) uint8_t isr_stack[ISR_STACKSIZE];

/**
 * Pre-start routine for CPU-specific settings
 */
__attribute__((weak)) void preStartup(void)
{
}

/**
 * Post-start routine for CPU-specific settings
 */
__attribute__((weak)) void postStartup(void)
{
}

void resetHandlerDefault(void)
{
    uint32_t *dst;
    uint32_t *src = &_etext;

    preStartup();

    uint32_t *top;
    /* Fill stack space with canary values up until the current stack pointer */
    /* Read current stack pointer from CPU register */
    __asm__ volatile ("mov %[top], sp" : [top] "=r" (top) : : );
    dst = &_sstack;
    while (dst < top) {
        *(dst++) = STACK_CANARY_WORD;
    }

    /* load data section from flash to ram */
    for (dst = &_srelocate; dst < &_erelocate; ) {
        *(dst++) = *(src++);
    }

    /* default bss section to zero */
    for (dst = &_szero; dst < &_ezero; ) {
        *(dst++) = 0;
    }

    postStartup();

    cpuInit();

    extern void __libc_init_array(void);
    __libc_init_array();

    /* startup the kernel */
    extern void vcKernelInit(void);
    vcKernelInit();
}

void nmiDefault(void)
{
    while (1);
}

void hardFaultDefault(void)
{
    while (1);
}

void memManageDefault(void)
{
    while (1);
}

void busFaultDefault(void)
{
    while (1);
}

void usageFaultDefault(void)
{
    while (1);
}

void debugMonDefault(void)
{
    while (1);
}

void dummyHandlerDefault(void)
{
    while (1);
}

/* Cortex-M common interrupt vectors */
__attribute__((weak,alias("dummyHandlerDefault"))) void isrSvc(void);
__attribute__((weak,alias("dummyHandlerDefault"))) void isrPendsv(void);
__attribute__((weak,alias("dummyHandlerDefault"))) void isrSystick(void);

/* define Cortex-M base interrupt vectors */
ISR_VECTOR(0) const cortexmBase cortexVectorBase = {
    &_estack,
    {
        /* entry point of the program */
        [0] = resetHandlerDefault,
        /* [-14] non maskable interrupt handler */
        [1] = nmiDefault,
        /* [-13] hard fault exception */
        [2] = hardFaultDefault,
        /* [-12] memory manage exception */
        [3] = memManageDefault,
        /* [-11] bus fault exception */
        [4] = busFaultDefault,
        /* [-10] usage fault exception */
        [5] = usageFaultDefault,
        /* [-5] SW interrupt use for triggering context switches */
        [10] = isrSvc,
        /* [-4] debug monitor exception */
        [11] = debugMonDefault,
        /* [-2] pendSV interrupt use to do the actual context switch */
        [13] = isrPendsv,
        /* [-1] SysTick interrupt not used */
        [14] = isrSystick,
    }
};
