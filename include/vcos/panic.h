#ifndef VCOS_PANIC_H
#define VCOS_PANIC_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    PANIC_GENERAL_ERROR,
    PANIC_SOFT_REBOOT,
    PANIC_HARD_REBOOT,
    PANIC_ASSERT_FAIL,
    PANIC_NMI_HANDLER,
    PANIC_HARD_FAULT,
    PANIC_MEM_MANAGE,
    PANIC_BUS_FAULT,
    PANIC_USAGE_FAULT,
    PANIC_DEBUG_MON,
    PANIC_DUMMY_HANDLER,
    PANIC_SSP, /* stack smashing protector failure */
    PANIC_UNDEFINED
} vcPanicType;

void vcCorePanic(vcPanicType aType, const char *aMessage);

extern void vcPanicArch(void);

#ifdef __cplusplus
}
#endif

#endif /* VCOS_PANIC_H */
