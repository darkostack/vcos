#ifndef ARDUINO_BASE_HPP
#define ARDUINO_BASE_HPP

#include <vcos/periph/gpio.h>

#include <arduino/serialport.hpp>

#include "arduino_board.h"

enum {
    INPUT,       /* configure pin as input */
    OUTPUT,      /* configure pin as output */
    INPUT_PULLUP /* configure pin as input with pull-up resistor */
};

enum {
    LOW = 0, /* pin is cleared */
    HIGH = 1 /* pin is set */
};

#ifndef ARDUINO_UART_DEV
#define ARDUINO_UART_DEV  UART_DEV(1)
#endif

static SerialPort Serial(ARDUINO_UART_DEV);

void pinMode(int pin, int mode);

void digitalWrite(int pin, int state);

int digitalRead(int pin);

void delay(unsigned long msec);

void delayMicroseconds(unsigned long usec);

unsigned long micros();

int analogRead(int pin);

#endif /* ARDUINO_BASE_HPP */
