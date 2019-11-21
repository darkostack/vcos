#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <vcos/irq.h>
#include <vcos/stdiobase.h>

#include <arduino/serialport.hpp>

SerialPort::SerialPort(vcUart aDev)
{
    this->mDev = aDev;
}

int SerialPort::available(void)
{
    return vcStdioReadAvailable();
}

void SerialPort::begin(long aBaudrate)
{
    (void)aBaudrate; /* Uart serial already initialized in vcStdioInit */
}

void SerialPort::end(void)
{
    /* can't end the uart serial, it use in stdio */
}

size_t SerialPort::print(int aVal)
{
    return print(aVal, DEC);
}

size_t SerialPort::print(int aVal, SerialFormat aFormat)
{
    char   buf[64];
    size_t len;

    switch (aFormat)
    {
    case BIN:
        /* TODO */
        return 0;

    case OCT:
        len = sprintf(buf, "%o", (unsigned)aVal);
        break;

    case DEC:
        len = sprintf(buf, "%i", aVal);
        break;

    case HEX:
        len = sprintf(buf, "%x", (unsigned)aVal);
        break;

    default:
        return 0;
    }

    write(buf, len);

    return len;
}

size_t SerialPort::print(float aVal)
{
    return print(aVal, 2);
}

size_t SerialPort::print(float aVal, int aFormat)
{
    char   buf[64];
    size_t len = sprintf(buf, "%.*f", aFormat, (double)aVal);
    write(buf, len);
    return len;
}

size_t SerialPort::print(char aVal)
{
    return (size_t)write((int)aVal);
}

size_t SerialPort::print(const char *aVal)
{
    return (size_t)write(aVal);
}

size_t SerialPort::println(int aVal)
{
    size_t res = print(aVal);
    write("\r\n");
    return (res + 2);
}

size_t SerialPort::println(int aVal, SerialFormat aFormat)
{
    size_t res = print(aVal, aFormat);
    write("\r\n");
    return (res + 2);
}

size_t SerialPort::println(float aVal)
{
    size_t res = print(aVal);
    write("\r\n");
    return (res + 2);
}

size_t SerialPort::println(float aVal, int aFormat)
{
    size_t res = print(aVal, aFormat);
    write("\r\n");
    return (res + 2);
}

size_t SerialPort::println(char aVal)
{
    size_t res = print(aVal);
    write("\r\n");
    return (res + 2);
}

size_t SerialPort::println(const char *aVal)
{
    size_t res = print(aVal);
    write("\r\n");
    return (res + 2);
}

int SerialPort::read(void)
{
    int tmp;
    vcStdioRead(static_cast<void *>(&tmp), 1);
    return tmp;
}

int SerialPort::write(int aVal)
{
    vcStdioWrite((const void *)&aVal, 1);
    return 1;
}

int SerialPort::write(const char *aStr)
{
    vcStdioWrite((const void *)aStr, strlen(aStr));
    return strlen(aStr);
}

int SerialPort::write(char *aBuf, int aLen)
{
    vcStdioWrite((const void *)aBuf, aLen);
    return aLen;
}
