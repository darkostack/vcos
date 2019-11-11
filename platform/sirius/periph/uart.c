#include <assert.h>

#include <vcos/periph/gpio.h>
#include <vcos/periph/uart.h>

#include "cpu.h"

/**
 * Number of UART peripheral interface.
 */
#define UART_NUMOF (7U)

typedef struct {
    uint8_t mPort;
    uint8_t mPin;
} vcUartIo;

static const vcUartIo sUartIo[UART_NUMOF] = {
    { .mPort = PORTA, .mPin = 12 },
    { .mPort = PORTA, .mPin = 13 },
    { .mPort = PORTA, .mPin = 14 },
    { .mPort = PORTA, .mPin = 15 },
    { .mPort = PORTB, .mPin = 0  },
    { .mPort = PORTB, .mPin = 1  },
    { .mPort = PORTB, .mPin = 15 }
};

static const IRQn_Type sUartIrqn[UART_NUMOF] = {
    Uart0_IRQn,
    Uart1_IRQn,
    Uart2_IRQn,
    Uart3_IRQn,
    Uart4_IRQn,
    Uart5_IRQn,
    Uart6_IRQn
};

static vcUartIsrContext sIsrUartContext[UART_NUMOF];

static void _uartSfioEnable(vcUart aUart)
{
    uint32_t temp;

    if (aUart == UART_DEV(0)) {
        /* rx */
        temp = VC_GPIOA->SEL1;
        temp &= ~GPIO_IOA_SEL1_IOAx_SEL_Msk(12);
        temp |= GPIO_SEL1_IOA12_UART0_RX;
        VC_GPIOA->SEL1 = temp;
        /* tx */
        temp = VC_GPIOB->SEL0;
        temp &= ~GPIO_IOB_SEL0_IOBx_SEL_Msk(2);
        temp |= GPIO_SEL0_IOB2_UART0_TX;
        VC_GPIOB->SEL0 = temp;
    } else if (aUart == UART_DEV(1)) {
        /* rx */
        temp = VC_GPIOA->SEL1;
        temp &= ~GPIO_IOA_SEL1_IOAx_SEL_Msk(13);
        temp |= GPIO_SEL1_IOA13_UART1_RX;
        VC_GPIOA->SEL1 = temp;
        /* tx */
        temp = VC_GPIOB->SEL0;
        temp &= ~GPIO_IOB_SEL0_IOBx_SEL_Msk(3);
        temp |= GPIO_SEL0_IOB3_UART1_TX;
        VC_GPIOB->SEL0 = temp;
    } else if (aUart == UART_DEV(2)) {
        /* rx */
        temp = VC_GPIOA->SEL1;
        temp &= ~GPIO_IOA_SEL1_IOAx_SEL_Msk(14);
        temp |= GPIO_SEL1_IOA14_UART2_RX;
        VC_GPIOA->SEL1 = temp;
        /* tx */
        temp = VC_GPIOB->SEL0;
        temp &= ~GPIO_IOB_SEL0_IOBx_SEL_Msk(4);
        temp |= GPIO_SEL0_IOB4_UART2_TX;
        VC_GPIOB->SEL0 = temp;
    } else if (aUart == UART_DEV(3)) {
        /* rx */
        temp = VC_GPIOA->SEL1;
        temp &= ~GPIO_IOA_SEL1_IOAx_SEL_Msk(15);
        temp |= GPIO_SEL1_IOA15_UART3_RX;
        VC_GPIOA->SEL1 = temp;
        /* tx */
        temp = VC_GPIOB->SEL0;
        temp |= GPIO_IOB_SEL0_IOBx_SEL_Msk(5);
        temp &= GPIO_SEL0_IOB5_UART3_TX;
        VC_GPIOB->SEL0 = temp;
    } else if (aUart == UART_DEV(4)) {
        /* rx */
        temp = VC_GPIOB->SEL0;
        temp &= ~GPIO_IOB_SEL0_IOBx_SEL_Msk(0);
        temp |= GPIO_SEL0_IOB0_UART4_RX;
        VC_GPIOB->SEL0 = temp;
        /* tx */
        temp = VC_GPIOB->SEL0;
        temp &= ~GPIO_IOB_SEL0_IOBx_SEL_Msk(6);
        temp |= GPIO_SEL0_IOB6_UART4_TX;
        VC_GPIOB->SEL0 = temp;
    } else if (aUart == UART_DEV(5)) {
        /* rx */
        temp = VC_GPIOB->SEL0;
        temp &= ~GPIO_IOB_SEL0_IOBx_SEL_Msk(1);
        temp |= GPIO_SEL0_IOB1_UART5_RX;
        VC_GPIOB->SEL0 = temp;
        /* tx */
        temp = VC_GPIOB->SEL0;
        temp &= ~GPIO_IOB_SEL0_IOBx_SEL_Msk(7);
        temp |= GPIO_SEL0_IOB7_UART5_TX;
        VC_GPIOB->SEL0 = temp;
    } else if (aUart == UART_DEV(6)) {
        /* rx */
        temp = VC_GPIOB->SEL1;
        temp &= ~GPIO_IOB_SEL1_IOBx_SEL_Msk(15);
        temp |= GPIO_SEL1_IOB15_UART6_RX;
        VC_GPIOB->SEL1 = temp;
        /* tx */
        temp = VC_GPIOB->SEL1;
        temp &= ~GPIO_IOB_SEL1_IOBx_SEL_Msk(8);
        temp |= GPIO_SEL1_IOB8_UART6_TX;
        VC_GPIOB->SEL1 = temp;
    } else {
        assert(0);
    }
}

int vcUartInit(vcUart aUart, uint32_t aBaudrate, vcUartRxCallback aCallback, void *aArg)
{
    assert(aUart < UART_NUMOF);

    /* enable uart clock */
    vcUartPowerOn(aUart);

    /* set uart mode as TX/RX */
    uint32_t temp = VC_UART(aUart)->CTRL;

    temp &= ~UART_CTRL_TXEN_Msk;
    temp &= ~UART_CTRL_RXEN_Msk;

    temp |= UART_CTRL_TXEN_Enabled;
    temp |= UART_CTRL_RXEN_Enabled;

    VC_UART(aUart)->CTRL = temp;

    /* set uart first-bit */
    temp = VC_UART(aUart)->CTRL2;
    temp &= ~UART_CTRL2_MSB_Msk;
    temp |= UART_CTRL2_MSB_LSB;
    VC_UART(aUart)->CTRL2 = temp;

    /* set uart baudrate */
    uint32_t ahb_div = ((VC_MISC2->CLKDIVH & MISC2_CLKDIVH_CLKDIVH_Msk) >> MISC2_CLKDIVH_CLKDIVH_Pos) + 1;
    uint32_t apb_div = ((VC_MISC2->CLKDIVP & MISC2_CLKDIVP_CLKDIVP_Msk) >> MISC2_CLKDIVP_CLKDIVP_Pos) + 1;
    uint32_t apb_clk = (SystemCoreClock / ahb_div) / apb_div;
    uint32_t baud_div = apb_clk / aBaudrate;

    /* round-up baud div */
    if ((apb_clk % aBaudrate) > (aBaudrate / 2)) baud_div++;

    VC_UART(aUart)->BAUDDIV = baud_div;

    /* setup default uart mode */
    vcUartMode(aUart, UART_DATA_BITS_8, UART_PARITY_NONE, UART_STOP_BITS_1);

    /* config uartfifo */
    temp = VC_UART(aUart)->FIFOCTRL;

    temp &= ~UART_FIFOCTRL_SFTRST_Msk;
    temp &= ~UART_FIFOCTRL_OVMODE_Msk;
    temp &= ~UART_FIFOCTRL_TXFIFOLVL_Msk;
    temp &= ~UART_FIFOCTRL_RXFIFOLVL_Msk;

    temp |= UART_FIFOCTRL_SFTRST_Disabled;
    temp |= UART_FIFOCTRL_OVMODE_Discard;
    temp |= UART_FIFOCTRL_TXFIFOLVL_7;
    temp |= UART_FIFOCTRL_RXFIFOLVL_0;

    VC_UART(aUart)->FIFOCTRL = temp;

    /* save isr context */
    sIsrUartContext[aUart].mCallback = aCallback;
    sIsrUartContext[aUart].mArg = aArg;

    /* enable uart rx interrupt if applicable */
    if (sIsrUartContext[aUart].mCallback) {
        /* set uart rx gpio input pull high */
        vcGpioInit(GPIO_PIN(sUartIo[aUart].mPort, sUartIo[aUart].mPin), GPIO_IN_PU);
        /* enable uart rx interrupt */
        temp = VC_UART(aUart)->CTRL;
        temp &= ~UART_CTRL_RXIE_Msk;
        temp |= UART_CTRL_RXIE_Enabled;
        VC_UART(aUart)->CTRL = temp;
        NVIC_EnableIRQ(sUartIrqn[aUart]);
    }

    /* enable uart special function gpio */
    _uartSfioEnable(aUart);

    return UART_OK;
}

int vcUartMode(vcUart aUart, vcUartDataBits aDataBits, vcUartParity aParity, vcUartStopBits aStopBits)
{
    assert(aUart < UART_NUMOF);

    uint32_t temp;

    temp = VC_UART(aUart)->CTRL2;

    temp &= ~UART_CTRL2_LENGTH_Msk;
    temp &= ~UART_CTRL2_STOPLEN_Msk;
    temp &= ~UART_CTRL2_PEN_Msk;
    temp &= ~UART_CTRL2_PMODE_Msk;

    temp |= (aDataBits == UART_DATA_BITS_7) ? UART_CTRL2_LENGTH_7BITS : UART_CTRL2_LENGTH_8BITS;
    temp |= (aStopBits == UART_STOP_BITS_1) ? UART_CTRL2_STOPLEN_1BITS : UART_CTRL2_STOPLEN_2BITS;

    if (aParity == UART_PARITY_NONE) {
        temp |= UART_CTRL2_PEN_Disabled;
    } else {
        temp |= UART_CTRL2_PEN_Enabled;
        if (aParity == UART_PARITY_ODD) {
            temp |= UART_CTRL2_PMODE_1; /* odd parity */
        } else {
            temp |= UART_CTRL2_PMODE_0; /* even parity */
        }
    }

    VC_UART(aUart)->CTRL2 = temp;

    return UART_OK;
}

static void _sendByte(vcUart aUart, uint8_t aByte)
{
    VC_UART(aUart)->DATA = ((aByte & UART_DATA_DATA_Msk) << UART_DATA_DATA_Pos);

    while ((VC_UART(aUart)->STATE & UART_STATE_TXDONE_Msk) == 0);

    /* clear TXDONE state */
    uint32_t temp = VC_UART(aUart)->STATE;
    temp &= UART_STATE_TXDONE_Msk;
    VC_UART(aUart)->STATE = temp;
}

size_t vcUartWrite(vcUart aUart, const uint8_t *aData, size_t aLen)
{
    assert(aUart < UART_NUMOF);

    for (size_t i = 0; i < aLen; i++) {
        _sendByte(aUart, aData[i]);
    }

    return aLen;
}

void vcUartPowerOn(vcUart aUart)
{
    assert(aUart < UART_NUMOF);

    uint32_t temp = VC_MISC2->PCLKEN;
    temp |= MISC2_PCLKEN_UART_Enabled(aUart);
    VC_MISC2->CLKEN_UNLOCK = MISC2_CLKEN_UNLOCK_UNLOCK_KEY;
    VC_MISC2->PCLKEN = temp;
}

void vcUartPowerOff(vcUart aUart)
{
    assert(aUart < UART_NUMOF);

    uint32_t temp = VC_MISC2->PCLKEN;
    temp &= ~MISC2_PCLKEN_UART_Msk(aUart);
    temp |= MISC2_PCLKEN_UART_Disabled(aUart);
    VC_MISC2->CLKEN_UNLOCK = MISC2_CLKEN_UNLOCK_UNLOCK_KEY;
    VC_MISC2->PCLKEN = temp;
}

static void _irqUartHandler(vcUart aUart)
{
    if ((VC_UART(aUart)->INTSTS & UART_INTSTS_RXIF_Msk) != 0) {
        /* clear RXIF interrupt status */ 
        uint32_t temp = VC_UART(aUart)->INTSTS;
        temp |= UART_INTSTS_RXIF_Msk;
        VC_UART(aUart)->INTSTS = temp;
        /* call uart rx callback if any */
        if (sIsrUartContext[aUart].mCallback != NULL) {
            sIsrUartContext[aUart].mCallback(sIsrUartContext[aUart].mArg, VC_UART(aUart)->DATA);
        }
        /* check if context switch was requested */
        cortexmIsrEnd();
    }
}

/* UART interrupt function entry  */

void isrUart0(void)
{
    _irqUartHandler(UART_DEV(0));
}

void isrUart1(void)
{
    _irqUartHandler(UART_DEV(1));
}

void isrUart2(void)
{
    _irqUartHandler(UART_DEV(2));
}

void isrUart3(void)
{
    _irqUartHandler(UART_DEV(3));
}

void isrUart4(void)
{
    _irqUartHandler(UART_DEV(4));
}

void isrUart5(void)
{
    _irqUartHandler(UART_DEV(5));
}

void isrUart6(void)
{
    _irqUartHandler(UART_DEV(6));
}
