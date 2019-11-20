#ifndef ARDUINO_SERIALPORT_HPP
#define ARDUINO_SERIALPORT_HPP

#include <vcos/periph/uart.h>

#define SERIAL_RX_BUFSIZE (64)

enum SerialFormat {
    BIN,
    OCT,
    DEC,
    HEX
};

class SerialPort
{
public:
    explicit SerialPort(vcUart aDev);

    int available(void);

    void begin(long aBaudrate);

    void end(void);

    size_t print(int aVal);

    size_t print(int aVal, SerialFormat aFormat);

    size_t print(float aVal);

    size_t print(float aVal, int aFormat);

    size_t print(char aVal);

    size_t print(const char *aVal);

    size_t println(int aVal);

    size_t println(int aVal, SerialFormat aFormat);

    size_t println(float aVal);

    size_t println(float aVal, int aFormat);

    size_t println(char aVal);

    size_t println(const char *aVal);

    int read(void);

    int write(int aVal);

    int write(const char *aStr);

    int write(char *aBuf, int aLen);

private:
    vcUart mDev;
};

#endif /* ARDUINO_SERIALPORT_HPP */
