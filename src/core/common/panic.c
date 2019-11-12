#include <stdio.h>
#include <string.h>

#include <vcos/panic.h>
#include <vcos/log.h>
#include <vcos/irq.h>

#include "cpu.h"

const char assertCrashMessage[] = "FAILED ASSERTION.";

/* flag preventing "recursive crash printing loop" */
static int crashed = 0;

void __attribute__((weak)) vcPanicArch(void) {}

/* WARNING: this function NEVER returns! */
__attribute__((noreturn)) void vcCorePanic(vcPanicType aType, const char *aMessage)
{
    if (crashed == 0) {
        crashed = 1;
        if (aType == PANIC_ASSERT_FAIL) {
            cpuPrintLastInstruction();
        }

        LOG_ERROR("*** kernel panic:\r\n%s\r\n\r\n", aMessage);
        LOG_ERROR("*** halted.\r\n\r\n");
    }

    irqDisable();
    vcPanicArch();

    while (1);
}
