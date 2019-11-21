#ifndef PLATFORM_ARDUINO_BOARD_H
#define PLATFORM_ARDUINO_BOARD_H

#include "arduino_pinmap.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Look-up table for the Arduino's digital pins.
 */
static const vcGpio arduinoPinMap[] = {
    GPIO_PIN(PORTC, 1),
    GPIO_PIN(PORTC, 2)
};

/**
 * Look-up table for the Arduino's analog pins.
 */
static const vcGpio arduinoAnalogMap[] = {
    GPIO_PIN(PORTA, 0),
    GPIO_PIN(PORTA, 1)
};

#ifdef __cplusplus
}
#endif

#endif /* PLATFORM_ARDUINO_BOARD_H */
