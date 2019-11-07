#include <assert.h>

#include "cpu.h"

#include "periph/gpio.h"
#include "periph/uart.h"

/**
 * Number of UART peripheral interface.
 */
#define UART_NUMOF (7U)

typedef struct {
    uint8_t port;
    uint8_t pin;
} uart_io_t;

static const uart_io_t uart_rxio[UART_NUMOF] = {
    { .port = PORTA, .pin = 12 },
    { .port = PORTA, .pin = 13 },
    { .port = PORTA, .pin = 14 },
    { .port = PORTA, .pin = 15 },
    { .port = PORTB, .pin = 0  },
    { .port = PORTB, .pin = 1  },
    { .port = PORTB, .pin = 15 }
};

static const IRQn_Type uart_irqn[UART_NUMOF] = {
    Uart0_IRQn,
    Uart1_IRQn,
    Uart2_IRQn,
    Uart3_IRQn,
    Uart4_IRQn,
    Uart5_IRQn,
    Uart6_IRQn
};

static uart_isr_ctx_t isr_uart_ctx[UART_NUMOF];

static void uart_sfio_enable(uart_t uart)
{
    uint32_t temp;

    if (uart == UART_DEV(0)) {
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
    } else if (uart == UART_DEV(1)) {
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
    } else if (uart == UART_DEV(2)) {
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
    } else if (uart == UART_DEV(3)) {
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
    } else if (uart == UART_DEV(4)) {
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
    } else if (uart == UART_DEV(5)) {
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
    } else if (uart == UART_DEV(6)) {
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

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    assert(uart < UART_NUMOF);

    /* enable uart clock */
    uart_poweron(uart);

    /* set uart mode as TX/RX */
    uint32_t temp = VC_UART(uart)->CTRL;

    temp &= ~UART_CTRL_TXEN_Msk;
    temp &= ~UART_CTRL_RXEN_Msk;

    temp |= UART_CTRL_TXEN_Enabled;
    temp |= UART_CTRL_RXEN_Enabled;

    VC_UART(uart)->CTRL = temp;

    /* set uart first-bit */
    temp = VC_UART(uart)->CTRL2;
    temp &= ~UART_CTRL2_MSB_Msk;
    temp |= UART_CTRL2_MSB_LSB;
    VC_UART(uart)->CTRL2 = temp;

    /* set uart baudrate */
    uint32_t ahb_div = ((VC_MISC2->CLKDIVH & MISC2_CLKDIVH_CLKDIVH_Msk) >> MISC2_CLKDIVH_CLKDIVH_Pos) + 1;
    uint32_t apb_div = ((VC_MISC2->CLKDIVP & MISC2_CLKDIVP_CLKDIVP_Msk) >> MISC2_CLKDIVP_CLKDIVP_Pos) + 1;
    uint32_t apb_clk = (SystemCoreClock / ahb_div) / apb_div;
    uint32_t baud_div = apb_clk / baudrate;

    /* round-up baud div */
    if ((apb_clk % baudrate) > (baudrate / 2)) baud_div++;

    VC_UART(uart)->BAUDDIV = baud_div;

    /* setup default uart mode */
    uart_mode(uart, UART_DATA_BITS_8, UART_PARITY_NONE, UART_STOP_BITS_1);

    /* config uartfifo */
    temp = VC_UART(uart)->FIFOCTRL;

    temp &= ~UART_FIFOCTRL_SFTRST_Msk;
    temp &= ~UART_FIFOCTRL_OVMODE_Msk;
    temp &= ~UART_FIFOCTRL_TXFIFOLVL_Msk;
    temp &= ~UART_FIFOCTRL_RXFIFOLVL_Msk;

    temp |= UART_FIFOCTRL_SFTRST_Disabled;
    temp |= UART_FIFOCTRL_OVMODE_Discard;
    temp |= UART_FIFOCTRL_TXFIFOLVL_7;
    temp |= UART_FIFOCTRL_RXFIFOLVL_0;

    VC_UART(uart)->FIFOCTRL = temp;

    /* save isr context */
    isr_uart_ctx[uart].rx_cb = rx_cb;
    isr_uart_ctx[uart].arg = arg;

    /* enable uart rx interrupt if applicable */
    if (isr_uart_ctx[uart].rx_cb) {
        /* set uart rx gpio input pull high */
        gpio_init(GPIO_PIN(uart_rxio[uart].port, uart_rxio[uart].pin), GPIO_IN_PU);
        /* enable uart rx interrupt */
        temp = VC_UART(uart)->CTRL;
        temp &= ~UART_CTRL_RXIE_Msk;
        temp |= UART_CTRL_RXIE_Enabled;
        VC_UART(uart)->CTRL = temp;
        NVIC_EnableIRQ(uart_irqn[uart]);
    }

    /* enable uart special function gpio */
    uart_sfio_enable(uart);

    return UART_OK;
}

int uart_mode(uart_t uart, uart_data_bits_t data_bits, uart_parity_t parity, uart_stop_bits_t stop_bits)
{
    assert(uart < UART_NUMOF);

    uint32_t temp;

    temp = VC_UART(uart)->CTRL2;

    temp &= ~UART_CTRL2_LENGTH_Msk;
    temp &= ~UART_CTRL2_STOPLEN_Msk;
    temp &= ~UART_CTRL2_PEN_Msk;
    temp &= ~UART_CTRL2_PMODE_Msk;

    temp |= (data_bits == UART_DATA_BITS_7) ? UART_CTRL2_LENGTH_7BITS : UART_CTRL2_LENGTH_8BITS;
    temp |= (stop_bits == UART_STOP_BITS_1) ? UART_CTRL2_STOPLEN_1BITS : UART_CTRL2_STOPLEN_2BITS;

    if (parity == UART_PARITY_NONE) {
        temp |= UART_CTRL2_PEN_Disabled;
    } else {
        temp |= UART_CTRL2_PEN_Enabled;
        if (parity == UART_PARITY_ODD) {
            temp |= UART_CTRL2_PMODE_1; /* odd parity */
        } else {
            temp |= UART_CTRL2_PMODE_0; /* even parity */
        }
    }

    VC_UART(uart)->CTRL2 = temp;

    return UART_OK;
}

static void send_byte(uart_t uart, uint8_t byte)
{
    VC_UART(uart)->DATA = ((byte & UART_DATA_DATA_Msk) << UART_DATA_DATA_Pos);

    while ((VC_UART(uart)->STATE & UART_STATE_TXDONE_Msk) == 0);

    /* clear TXDONE state */
    uint32_t temp = VC_UART(uart)->STATE;
    temp &= UART_STATE_TXDONE_Msk;
    VC_UART(uart)->STATE = temp;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    assert(uart < UART_NUMOF);

    for (size_t i = 0; i < len; i++) {
        send_byte(uart, data[i]);
    }
}

void uart_poweron(uart_t uart)
{
    assert(uart < UART_NUMOF);

    uint32_t temp = VC_MISC2->PCLKEN;
    temp |= MISC2_PCLKEN_UART_Enabled(uart);
    VC_MISC2->CLKEN_UNLOCK = MISC2_CLKEN_UNLOCK_UNLOCK_KEY;
    VC_MISC2->PCLKEN = temp;
}

void uart_poweroff(uart_t uart)
{
    assert(uart < UART_NUMOF);

    uint32_t temp = VC_MISC2->PCLKEN;
    temp &= ~MISC2_PCLKEN_UART_Msk(uart);
    temp |= MISC2_PCLKEN_UART_Disabled(uart);
    VC_MISC2->CLKEN_UNLOCK = MISC2_CLKEN_UNLOCK_UNLOCK_KEY;
    VC_MISC2->PCLKEN = temp;
}

static void irq_uart_handler(uart_t uart)
{
    if ((VC_UART(uart)->INTSTS & UART_INTSTS_RXIF_Msk) != 0) {
        /* clear RXIF interrupt status */ 
        uint32_t temp = VC_UART(uart)->INTSTS;
        temp |= UART_INTSTS_RXIF_Msk;
        VC_UART(uart)->INTSTS = temp;
        /* call uart rx callback if any */
        if (isr_uart_ctx[uart].rx_cb != NULL) {
            isr_uart_ctx[uart].rx_cb(isr_uart_ctx[uart].arg, VC_UART(uart)->DATA);
        }
        /* check if context switch was requested */
        cortexm_isr_end();
    }
}

/* UART interrupt function entry  */

void isr_uart0(void)
{
    irq_uart_handler(UART_DEV(0));
}

void isr_uart1(void)
{
    irq_uart_handler(UART_DEV(1));
}

void isr_uart2(void)
{
    irq_uart_handler(UART_DEV(2));
}

void isr_uart3(void)
{
    irq_uart_handler(UART_DEV(3));
}

void isr_uart4(void)
{
    irq_uart_handler(UART_DEV(4));
}

void isr_uart5(void)
{
    irq_uart_handler(UART_DEV(5));
}

void isr_uart6(void)
{
    irq_uart_handler(UART_DEV(6));
}
