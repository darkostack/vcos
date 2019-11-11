#ifndef PERIPH_UART_H
#define PERIPH_UART_H

#include <stddef.h>
#include <stdint.h>
#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int vcUart;

#define UART_UNDEF ((vcUart)(UINT_MAX))

#define UART_DEV(x) ((vcUart)(x))

typedef void (*vcUartRxCallback)(void *aArg, uint8_t aData);

typedef struct {
    vcUartRxCallback mCallback;
    void *mArg;
} vcUartIsrContext;

enum {
    UART_OK         = 0,
    UART_NODEV      = -1,
    UART_NOBAUD     = -2,
    UART_INTERR     = -3,
    UART_NOMODE     = -4,
};

typedef enum {
    UART_PARITY_NONE,
    UART_PARITY_EVEN,
    UART_PARITY_ODD,
    UART_PARITY_MARK,
    UART_PARITY_SPACE,
} vcUartParity;

typedef enum {
    UART_DATA_BITS_5,
    UART_DATA_BITS_6,
    UART_DATA_BITS_7,
    UART_DATA_BITS_8,
} vcUartDataBits;

typedef enum {
    UART_STOP_BITS_1,
    UART_STOP_BITS_2,
} vcUartStopBits;

int vcUartInit(vcUart aUart, uint32_t aBaudrate, vcUartRxCallback aCallback, void *aArg);

int vcUartMode(vcUart aUart,
               vcUartDataBits aDataBits,
               vcUartParity aParity,
               vcUartStopBits aStopBits);

size_t vcUartWrite(vcUart aUart, const uint8_t *aData, size_t aLen);

void vcUartPowerOn(vcUart aUart);

void vcUartPowerOff(vcUart aUart);

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_UART_H */
