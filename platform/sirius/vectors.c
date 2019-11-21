#include "vectors_cortexm.h"

void dummyHandler(void)
{
    dummyHandlerDefault();
}

/* vc7300x specific interrupt vectors */
WEAK_DEFAULT void isrPmu(void);
WEAK_DEFAULT void isrRtc(void);
WEAK_DEFAULT void isrU32k0(void);
WEAK_DEFAULT void isrU32k1(void);
WEAK_DEFAULT void isrI2c0(void);
WEAK_DEFAULT void isrSpi0(void);
WEAK_DEFAULT void isrUart0(void);
WEAK_DEFAULT void isrUart1(void);
WEAK_DEFAULT void isrUart2(void);
WEAK_DEFAULT void isrUart3(void);
WEAK_DEFAULT void isrUart4(void);
WEAK_DEFAULT void isrUart5(void);
WEAK_DEFAULT void isrIso78160(void);
WEAK_DEFAULT void isrIso78161(void);
WEAK_DEFAULT void isrTim0(void);
WEAK_DEFAULT void isrTim1(void);
WEAK_DEFAULT void isrTim2(void);
WEAK_DEFAULT void isrTim3(void);
WEAK_DEFAULT void isrPwm0(void);
WEAK_DEFAULT void isrPwm1(void);
WEAK_DEFAULT void isrPwm2(void);
WEAK_DEFAULT void isrPwm3(void);
WEAK_DEFAULT void isrDma(void);
WEAK_DEFAULT void isrEmbflash(void);
WEAK_DEFAULT void isrAna(void);
WEAK_DEFAULT void isrSpi1(void);
WEAK_DEFAULT void isrSpi2(void);
WEAK_DEFAULT void isrKeyscan(void);
WEAK_DEFAULT void isrUart6(void);
WEAK_DEFAULT void isrCrypto(void);
WEAK_DEFAULT void isrI2c1(void);
WEAK_DEFAULT void isrGpio(void);

ISR_VECTOR(1)
const isrCallback vectorCpu[CPU_IRQ_NUMOF] = {
    [0] = isrPmu,       [1] = isrRtc,       [2] = isrU32k0, [3] = isrU32k1,    [4] = isrI2c0,   [5] = isrSpi0,
    [6] = isrUart0,     [7] = isrUart1,     [8] = isrUart2, [9] = isrUart3,    [10] = isrUart4, [11] = isrUart5,
    [12] = isrIso78160, [13] = isrIso78161, [14] = isrTim0, [15] = isrTim1,    [16] = isrTim2,  [17] = isrTim3,
    [18] = isrPwm0,     [19] = isrPwm1,     [20] = isrPwm2, [21] = isrPwm3,    [22] = isrDma,   [23] = isrEmbflash,
    [24] = isrAna,      [27] = isrSpi1,     [28] = isrSpi2, [29] = isrKeyscan, [30] = isrUart6, [31] = isrCrypto,
    [36] = isrI2c1,     [37] = isrGpio,
};
