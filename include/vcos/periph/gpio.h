#ifndef PERIPH_GPIO_H
#define PERIPH_GPIO_H

#include <limits.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int vcGpio;

#define GPIO_UNDEF ((vcGpio)(UINT_MAX))

#define GPIO_PIN(port, pin) ((vcGpio)((port & 0xf) | ((pin & 0xf) << 4)))

/**
 * GPIO port definition.
 */
enum
{
    PORTA,
    PORTB,
    PORTC,
    PORTD,
    PORTE,
    PORTF
};

typedef enum
{
    GPIO_IN,    /* configure as input without pull resistor */
    GPIO_IN_PD, /* configure as input with pull-down resistor */
    GPIO_IN_PU, /* configure as input with pull-up resistor */
    GPIO_OUT,   /* configure as output in push-pull mode */
    GPIO_OD,    /* configure as output in open-drain mode without pull resistor */
    GPIO_OD_PU  /* configure as output in open-drain mode with pull resistor enabled */
} vcGpioMode;

/**
 * Definition of possible active flanks for external interrupt mode.
 */
typedef enum
{
    GPIO_FALLING = 0, /* emit interrupt on falling flank */
    GPIO_RISING  = 1, /* emit interrupt on rising flank */
    GPIO_BOTH    = 2  /* emit interrupt on both flank */
} vcGpioFlank;

/**
 * Signature of event callback functions triggered from interrupts.
 */
typedef void (*vcGpioCallback)(void *aArg);

typedef struct
{
    vcGpioCallback mCallback;
    void *         mArg;
} vcGpioIsrContext;

int vcGpioInit(vcGpio aPin, vcGpioMode aMode);

int vcGpioInitInt(vcGpio aPin, vcGpioMode aMode, vcGpioFlank aFlank, vcGpioCallback aCallback, void *aArg);

void vcGpioIrqEnable(vcGpio aPin);

void vcGpioIrqDisable(vcGpio aPin);

int vcGpioRead(vcGpio aPin);

void vcGpioSet(vcGpio aPin);

void vcGpioClear(vcGpio aPin);

void vcGpioToggle(vcGpio apin);

void vcGpioWrite(vcGpio aPin, int aValue);

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_GPIO_H */
