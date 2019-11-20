#ifndef ARDUINO_BASE_HPP
#define ARDUINO_BASE_HPP

#include <vcos/stdiobase.h>
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
#define ARDUINO_UART_DEV  STDIOBASE_UART_DEV
#endif

static SerialPort Serial(ARDUINO_UART_DEV);

void pinMode(int aPin, int aMode);

void digitalWrite(int aPin, int aState);

int digitalRead(int aPin);

void delay(unsigned long aMsec);

void delayMicroseconds(unsigned long aUsec);

unsigned long micros();

int analogRead(int aPin);

extern "C" void setup(void);

extern "C" void loop(void);

#endif /* ARDUINO_BASE_HPP */
