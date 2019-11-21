#include <vcos/timer.h>
#include <vcos/periph/gpio.h>

#include <arduino/base.hpp>

#define ANALOG_PIN_NUMOF  (ARRAY_SIZE(arduinoAnalogMap))

void pinMode(int aPin, int aMode)
{
    vcGpioMode m = GPIO_OUT;

    if (aMode == INPUT)
    {
        m = GPIO_IN;
    }
    else if (aMode == INPUT_PULLUP)
    {
        m = GPIO_IN_PU;
    }

    vcGpioInit(arduinoPinMap[aPin], m);
}

void digitalWrite(int aPin, int aState)
{
    vcGpioWrite(arduinoPinMap[aPin], aState);
}

int digitalRead(int aPin)
{
    if (vcGpioRead(arduinoPinMap[aPin]))
    {
        return HIGH;
    }
    else
    {
        return LOW;
    }
}

void delay(unsigned long aMsec)
{
    vcTimerSleepUsec(1000 * aMsec);
}

void delayMicroseconds(unsigned long aUsec)
{
    vcTimerSleepUsec(aUsec);
}

unsigned long micros()
{
    return vcTimerNowUsec();
}

int analogRead(int aPin)
{
    (void) aPin;
    return 0;
}
