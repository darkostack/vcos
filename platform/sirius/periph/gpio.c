#include <assert.h>

#include "cpu.h"

#include "periph/gpio.h"

/**
 * Sirius has 32 external interupt lines, 16 pins of PORTA and 16 pins of PORTC.
 */
#define EXTI_NUMOF (32U)

/**
 * isr_gpio_ctx index 0-15 will be used to store PORTA pins and the rest will
 * be used for PORTC pins.
 */
static gpio_isr_ctx_t isr_gpio_ctx[EXTI_NUMOF];

static inline uint8_t _port(gpio_t pin)
{
    /* get the port number from given GPIO_PIN() */
    return (pin & 0xf);
}

static inline uint8_t _pin(gpio_t pin)
{
    /* get the pin number from given GPIO_PIN() */
    return ((pin >> 4) & 0xf);
}

int gpio_init(gpio_t pin, gpio_mode_t mode)
{
    uint32_t temp;

    VC_GPIO_Type *gpio = NULL;

    if (_port(pin) == PORTA) {
        gpio = (VC_GPIO_Type *)VC_GPIOA;
    } else {
        gpio = (VC_GPIO_Type *)VC_GPIO(_port(pin));
    }

    assert(gpio != NULL);

    /* set gpio mode */
    if (mode == GPIO_IN) {
        /* input */
        temp = gpio->OEN;
        temp &= ~GPIO_IOX_OEN_IOXOEN_Msk(_pin(pin));
        temp |= GPIO_IOX_OEN_IOXOEN_Disabled(_pin(pin));
        gpio->OEN = temp;

        temp = gpio->IE;
        temp &= ~GPIO_IOX_IE_IOXIE_Msk(_pin(pin));
        temp |= GPIO_IOX_IE_IOXIE_Enabled(_pin(pin));
        gpio->IE = temp;

        temp = gpio->ATT;
        temp &= ~GPIO_IOX_ATT_IOXATT_Msk(_pin(pin));
        temp |= GPIO_IOX_ATT_IOXATT_CMOS(_pin(pin));
        gpio->ATT = temp;

    } else if (mode == GPIO_IN_PD) {
        /* input pull-down */
        temp = gpio->OEN;
        temp &= ~GPIO_IOX_OEN_IOXOEN_Msk(_pin(pin));
        temp |= GPIO_IOX_OEN_IOXOEN_Disabled(_pin(pin));
        gpio->OEN = temp;

        temp = gpio->IE;
        temp &= ~GPIO_IOX_IE_IOXIE_Msk(_pin(pin));
        temp |= GPIO_IOX_IE_IOXIE_Enabled(_pin(pin));
        gpio->IE = temp;

        temp = gpio->ATT;
        temp &= ~GPIO_IOX_ATT_IOXATT_Msk(_pin(pin));
        temp |= GPIO_IOX_ATT_IOXATT_OPEN_DRAIN(_pin(pin));
        gpio->ATT = temp;

        temp = gpio->DAT;
        temp &= ~GPIO_IOX_DAT_IOXDAT_Msk(_pin(pin));
        temp |= GPIO_IOX_DAT_IOXDAT_Disabled(_pin(pin));
        gpio->DAT = temp;

    } else if (mode == GPIO_IN_PU) {
        /* input pull-up */
        temp = gpio->OEN;
        temp &= ~GPIO_IOX_OEN_IOXOEN_Msk(_pin(pin));
        temp |= GPIO_IOX_OEN_IOXOEN_Disabled(_pin(pin));
        gpio->OEN = temp;

        temp = gpio->IE;
        temp &= ~GPIO_IOX_IE_IOXIE_Msk(_pin(pin));
        temp |= GPIO_IOX_IE_IOXIE_Enabled(_pin(pin));
        gpio->IE = temp;

        temp = gpio->ATT;
        temp &= ~GPIO_IOX_ATT_IOXATT_Msk(_pin(pin));
        temp |= GPIO_IOX_ATT_IOXATT_OPEN_DRAIN(_pin(pin));
        gpio->ATT = temp;

        temp = gpio->DAT;
        temp &= ~GPIO_IOX_DAT_IOXDAT_Msk(_pin(pin));
        temp |= GPIO_IOX_DAT_IOXDAT_Enabled(_pin(pin));
        gpio->DAT = temp;

    } else if (mode == GPIO_OUT) {
        /* output */
        temp = gpio->OEN;
        temp &= ~GPIO_IOX_OEN_IOXOEN_Msk(_pin(pin));
        temp |= GPIO_IOX_OEN_IOXOEN_Enabled(_pin(pin));
        gpio->OEN = temp;

        temp = gpio->IE;
        temp &= ~GPIO_IOX_IE_IOXIE_Msk(_pin(pin));
        temp |= GPIO_IOX_IE_IOXIE_Disabled(_pin(pin));
        gpio->IE = temp;

        temp = gpio->ATT;
        temp &= ~GPIO_IOX_ATT_IOXATT_Msk(_pin(pin));
        temp |= GPIO_IOX_ATT_IOXATT_CMOS(_pin(pin));
        gpio->ATT = temp;

    } else if (mode == GPIO_OD) {
        /* open-drain output low */
        temp = gpio->OEN;
        temp &= ~GPIO_IOX_OEN_IOXOEN_Msk(_pin(pin));
        temp |= GPIO_IOX_OEN_IOXOEN_Enabled(_pin(pin));
        gpio->OEN = temp;

        temp = gpio->IE;
        temp &= ~GPIO_IOX_IE_IOXIE_Msk(_pin(pin));
        temp |= GPIO_IOX_IE_IOXIE_Disabled(_pin(pin));
        gpio->IE = temp;

        temp = gpio->ATT;
        temp &= ~GPIO_IOX_ATT_IOXATT_Msk(_pin(pin));
        temp |= GPIO_IOX_ATT_IOXATT_OPEN_DRAIN(_pin(pin));
        gpio->ATT = temp;

        temp = gpio->DAT;
        temp &= ~GPIO_IOX_DAT_IOXDAT_Msk(_pin(pin));
        temp |= GPIO_IOX_DAT_IOXDAT_Disabled(_pin(pin));
        gpio->DAT = temp;

    } else if (mode == GPIO_OD_PU) {
        /* open-drain output high */
        temp = gpio->OEN;
        temp &= ~GPIO_IOX_OEN_IOXOEN_Msk(_pin(pin));
        temp |= GPIO_IOX_OEN_IOXOEN_Enabled(_pin(pin));
        gpio->OEN = temp;

        temp = gpio->IE;
        temp &= ~GPIO_IOX_IE_IOXIE_Msk(_pin(pin));
        temp |= GPIO_IOX_IE_IOXIE_Disabled(_pin(pin));
        gpio->IE = temp;

        temp = gpio->ATT;
        temp &= ~GPIO_IOX_ATT_IOXATT_Msk(_pin(pin));
        temp |= GPIO_IOX_ATT_IOXATT_OPEN_DRAIN(_pin(pin));
        gpio->ATT = temp;

        temp = gpio->DAT;
        temp &= ~GPIO_IOX_DAT_IOXDAT_Msk(_pin(pin));
        temp |= GPIO_IOX_DAT_IOXDAT_Enabled(_pin(pin));
        gpio->DAT = temp;

    } else {
        assert(0); /* unknown gpio mode */
    }

    /* reset PORTx pin special function as gpio */
    if (_port(pin) == PORTA) {
        if (_pin(pin) < 8) {
            temp = VC_GPIOA->SEL0;
            temp &= ~GPIO_IOA_SEL0_IOAx_SEL_Msk(_pin(pin));
            temp |= GPIO_SEL0_IOAx_GPIO(_pin(pin));
            VC_GPIOA->SEL0 = temp;
        } else {
            temp = VC_GPIOA->SEL1;
            temp &= ~GPIO_IOA_SEL1_IOAx_SEL_Msk(_pin(pin));
            temp |= GPIO_SEL1_IOAx_GPIO(_pin(pin));
            VC_GPIOA->SEL1 = temp;
        }
    } else if (_port(pin) == PORTB || _port(pin) == PORTC) {
        if (_port(pin) == PORTB) {
            if (_pin(pin) < 8) {
                temp = VC_GPIOB->SEL0;
                temp &= ~GPIO_IOB_SEL0_IOBx_SEL_Msk(_pin(pin));
                VC_GPIOB->SEL0 = temp;
            } else {
                temp = VC_GPIOB->SEL1;
                temp &= ~GPIO_IOB_SEL1_IOBx_SEL_Msk(_pin(pin));
                VC_GPIOB->SEL1 = temp;
            }
        } else {
            if (_pin(pin) < 8) {
                temp = VC_GPIOC->SEL0;
                temp &= ~GPIO_IOC_SEL0_IOCx_SEL_Msk(_pin(pin));
                VC_GPIOC->SEL0 = temp;
            } else {
                temp = VC_GPIOC->SEL1;
                temp &= ~GPIO_IOC_SEL1_IOCx_SEL_Msk(_pin(pin));
                VC_GPIOC->SEL1 = temp;
            }
        }
    } else {
        if (_port(pin) == PORTD) {
            temp = VC_GPIOD->SEL;
            temp &= ~GPIO_IOD_SEL_IODx_SEL_Msk(_pin(pin));
            VC_GPIOD->SEL = temp;
        } else if (_port(pin) == PORTE) {
            temp = VC_GPIOE->SEL;
            temp &= ~GPIO_IOE_SEL_IOEx_SEL_Msk(_pin(pin));
            VC_GPIOE->SEL = temp;
        } else {
            assert(_pin(pin) < 4); /* PORTF only support pin (0 - 3) */
            temp = VC_GPIOF->SEL;
            temp &= ~GPIO_IOF_SEL_IOFx_SEL_Msk(_pin(pin));
            VC_GPIOF->SEL = temp;
        }
    }

    return 0;
}

int gpio_init_int(gpio_t pin,
                  gpio_mode_t mode,
                  gpio_flank_t flank,
                  gpio_cb_t cb,
                  void *arg)
{
    uint32_t temp, temp1 = 0;

    assert((_port(pin) == PORTA) || (_port(pin) == PORTC));
    assert(cb != NULL);

    /* store callback information */
    isr_gpio_ctx[(8 * _port(pin)) + _pin(pin)].cb = cb;
    isr_gpio_ctx[(8 * _port(pin)) + _pin(pin)].arg = arg;

    /* initialize pin as input */
    gpio_init(pin, mode);

    /* enable gpio interrupt */

    if (_port(pin) == PORTA) {
        /* disable IOA de-glitch circuit */
        temp = VC_PMU->IOANODEG;
        temp |= (1 << _pin(pin));
        VC_PMU->IOANODEG = temp;

        /* enable pmu interrupt */
        temp = VC_PMU->CONTROL;
        temp |= PMU_CONTROL_INTEN_Enabled;
        VC_PMU->CONTROL = temp;

        /* set wake-up signal */
        temp = VC_GPIOA->WKUEN;
        temp1 = VC_GPIOA->DAT;

        temp &= ~GPIO_IOx_WKUEN_Msk(_pin(pin));
        temp1 &= ~GPIO_IOX_DAT_IOXDAT_Msk(_pin(pin));

        if (flank == GPIO_FALLING) {
            temp |= (1UL << GPIO_IOx_WKUEN_Pos(_pin(pin)));
            temp1 |= (1UL << _pin(pin));
        } else if (flank == GPIO_RISING) {
            temp |= (1UL << GPIO_IOx_WKUEN_Pos(_pin(pin)));
        } else if (flank == GPIO_BOTH) {
            temp |= (3UL << GPIO_IOx_WKUEN_Pos(_pin(pin)));    
        } else {
            assert(0); /* unsupported gpio flank signal */
        }

        VC_GPIOA->WKUEN = temp;
        VC_GPIOA->DAT = temp1;

        /* set pin to EINT special function */
        if (_pin(pin) < 8) {
            temp = VC_GPIOA->SEL0;
            temp |= GPIO_SEL0_IOAx_EINTx(_pin(pin));
            VC_GPIOA->SEL0 = temp;
        } else {
            temp = VC_GPIOA->SEL1;
            temp |= GPIO_SEL1_IOAx_EINTx(_pin(pin));
            VC_GPIOA->SEL1 = temp;
        }

    } else {

        /* set wake-up signal */
        temp = VC_GPIOC->WKUEN;
        temp1 = VC_GPIOC->DAT;

        temp &= ~GPIO_IOx_WKUEN_Msk(_pin(pin));
        temp1 &= ~GPIO_IOX_DAT_IOXDAT_Msk(_pin(pin));

        if (flank == GPIO_FALLING) {
            temp |= (1UL << GPIO_IOx_WKUEN_Pos(_pin(pin)));
            temp1 |= (1UL << _pin(pin));
        } else if (flank == GPIO_RISING) {
            temp |= (1UL << GPIO_IOx_WKUEN_Pos(_pin(pin)));
        } else if (flank == GPIO_BOTH) {
            temp |= (3UL << GPIO_IOx_WKUEN_Pos(_pin(pin)));    
        } else {
            assert(0); /* unsupported gpio flank signal */
        }

        VC_GPIOC->WKUEN = temp;
        VC_GPIOC->DAT = temp1;

        /* set pin to EINT special function */
        if (_pin(pin) < 8) {
            temp = VC_GPIOC->SEL0;
            temp |= GPIO_SEL0_IOCx_EINTx(_pin(pin));
            VC_GPIOC->SEL0 = temp;
        } else {
            temp = VC_GPIOC->SEL1;
            temp |= GPIO_SEL1_IOCx_EINTx(_pin(pin));
            VC_GPIOC->SEL1 = temp;
        }
    }

    /* enable global pin interrupt */
    gpio_irq_enable(pin);

    return 0;
}

void gpio_irq_set_prio(gpio_t pin, uint32_t prio)
{
    if (_port(pin) == PORTA) {
        NVIC_SetPriority(Pmu_IRQn, prio);
    } else {
        NVIC_SetPriority(Gpio_IRQn, prio);
    }
}

void gpio_irq_enable(gpio_t pin)
{
    /* enable global pin interrupt */
    if (_port(pin) == PORTA) {
        NVIC_EnableIRQ(Pmu_IRQn);
    } else {
        NVIC_EnableIRQ(Gpio_IRQn);
    }
}

void gpio_irq_disable(gpio_t pin)
{
    assert((_port(pin) == PORTA) || (_port(pin) == PORTC));

    uint32_t temp, temp1 = 0;

    if (_port(pin) == PORTA) {
        /* enable back IOA de-glitch circuit as default config */
        temp = VC_PMU->IOANODEG;
        temp &= ~(1 << _pin(pin));
        VC_PMU->IOANODEG = temp;

        /* disable wake-up function */
        temp = VC_GPIOA->WKUEN;
        temp1 = VC_GPIOA->DAT;

        temp &= ~GPIO_IOx_WKUEN_Msk(_pin(pin));
        temp1 &= ~GPIO_IOX_DAT_IOXDAT_Msk(_pin(pin));

        VC_GPIOA->WKUEN = temp;
        VC_GPIOA->DAT = temp1;

        /* disable output and input function */
        temp = VC_GPIOA->OEN;
        temp &= ~GPIO_IOX_OEN_IOXOEN_Msk(_pin(pin));
        temp |= GPIO_IOX_OEN_IOXOEN_Disabled(_pin(pin));
        VC_GPIOA->OEN = temp;

        temp = VC_GPIOA->IE;
        temp &= ~GPIO_IOX_IE_IOXIE_Msk(_pin(pin));
        temp |= GPIO_IOX_IE_IOXIE_Disabled(_pin(pin));
        VC_GPIOA->IE = temp;

        temp = VC_GPIOA->ATT;
        temp &= ~GPIO_IOX_ATT_IOXATT_Msk(_pin(pin));
        temp |= GPIO_IOX_ATT_IOXATT_CMOS(_pin(pin));
        VC_GPIOA->ATT = temp;

        /* set to gpio special function */
        if (_pin(pin) < 8) {
            temp = VC_GPIOA->SEL0;
            temp &= ~GPIO_IOA_SEL0_IOAx_SEL_Msk(_pin(pin));
            temp |= GPIO_SEL0_IOAx_GPIO(_pin(pin));
            VC_GPIOA->SEL0 = temp;
        } else {
            temp = VC_GPIOA->SEL1;
            temp &= ~GPIO_IOA_SEL1_IOAx_SEL_Msk(_pin(pin));
            temp |= GPIO_SEL1_IOAx_GPIO(_pin(pin));
            VC_GPIOA->SEL1 = temp;
        }
   } else {
        /* disable wake-up function */
        temp = VC_GPIOC->WKUEN;
        temp1 = VC_GPIOC->DAT;

        temp &= ~GPIO_IOx_WKUEN_Msk(_pin(pin));
        temp1 &= ~GPIO_IOX_DAT_IOXDAT_Msk(_pin(pin));

        VC_GPIOC->WKUEN = temp;
        VC_GPIOC->DAT = temp1;

        /* disable output and input function */
        temp = VC_GPIOC->OEN;
        temp &= ~GPIO_IOX_OEN_IOXOEN_Msk(_pin(pin));
        temp |= GPIO_IOX_OEN_IOXOEN_Disabled(_pin(pin));
        VC_GPIOC->OEN = temp;

        temp = VC_GPIOC->IE;
        temp &= ~GPIO_IOX_IE_IOXIE_Msk(_pin(pin));
        temp |= GPIO_IOX_IE_IOXIE_Disabled(_pin(pin));
        VC_GPIOC->IE = temp;

        temp = VC_GPIOC->ATT;
        temp &= ~GPIO_IOX_ATT_IOXATT_Msk(_pin(pin));
        temp |= GPIO_IOX_ATT_IOXATT_CMOS(_pin(pin));
        VC_GPIOC->ATT = temp;

        /* set to gpio special function */
        if (_pin(pin) < 8) {
            temp = VC_GPIOC->SEL0;
            temp &= ~GPIO_IOC_SEL0_IOCx_SEL_Msk(_pin(pin));
            temp |= GPIO_SEL0_IOCx_GPIO(_pin(pin));
            VC_GPIOC->SEL0 = temp;
        } else {
            temp = VC_GPIOC->SEL1;
            temp &= ~GPIO_IOC_SEL1_IOCx_SEL_Msk(_pin(pin));
            temp |= GPIO_SEL1_IOCx_GPIO(_pin(pin));
            VC_GPIOC->SEL1 = temp;
        }
   }
}

int gpio_read(gpio_t pin)
{
    if (_port(pin) == PORTF) {
        assert(_pin(pin) < 4);
    }

    VC_GPIO_Type *gpio = NULL;

    if (_port(pin) == PORTA) {
        gpio = (VC_GPIO_Type *)VC_GPIOA;
    } else {
        gpio = (VC_GPIO_Type *)VC_GPIO(_port(pin));
    }

    assert(gpio != NULL);

    return (gpio->DAT & GPIO_IOX_DAT_IOXDAT_Msk(_pin(pin)));
}

void gpio_set(gpio_t pin)
{
    if (_port(pin) == PORTF) {
        assert(_pin(pin) < 4);
    }

    VC_GPIO_Type *gpio = NULL;

    if (_port(pin) == PORTA) {
        gpio = (VC_GPIO_Type *)VC_GPIOA;
    } else {
        gpio = (VC_GPIO_Type *)VC_GPIO(_port(pin));
    }

    assert(gpio != NULL);

    uint32_t temp = gpio->DAT;
    temp &= ~GPIO_IOX_DAT_IOXDAT_Msk(_pin(pin));
    temp |= GPIO_IOX_DAT_IOXDAT_Enabled(_pin(pin));
    gpio->DAT = temp;
}

void gpio_clear(gpio_t pin)
{
    if (_port(pin) == PORTF) {
        assert(_pin(pin) < 4);
    }

    VC_GPIO_Type *gpio = NULL;

    if (_port(pin) == PORTA) {
        gpio = (VC_GPIO_Type *)VC_GPIOA;
    } else {
        gpio = (VC_GPIO_Type *)VC_GPIO(_port(pin));
    }

    assert(gpio != NULL);

    uint32_t temp = gpio->DAT;
    temp &= ~GPIO_IOX_DAT_IOXDAT_Msk(_pin(pin));
    temp |= GPIO_IOX_DAT_IOXDAT_Disabled(_pin(pin));
    gpio->DAT = temp;
}

void gpio_toggle(gpio_t pin)
{
    if (gpio_read(pin)) {
        gpio_clear(pin);
    } else {
        gpio_set(pin);
    }
}

void gpio_write(gpio_t pin, int value)
{
    if (value) {
        gpio_set(pin);
    } else {
        gpio_clear(pin);
    }
}

static void irq_gpio_handler(gpio_t pin)
{
    uint8_t index = (8 * _port(pin) + _pin(pin));
    if (isr_gpio_ctx[index].cb != NULL) {
        isr_gpio_ctx[index].cb(isr_gpio_ctx[index].arg);
        /* check if context switch was requested */
        cortexm_isr_end();
    }
}

/* GPIO interrupt function entry */
void isr_pmu(void)
{
    for (int i = 0; i < 16; i++) {
        if ((VC_GPIOA->INT & GPIO_IOX_INT_IOXINT_Msk(i)) != 0) {
            /* clear interrupt status */
            uint32_t temp = VC_GPIOA->INT;
            temp |= GPIO_IOX_INT_IOXINT_Msk(i);
            VC_GPIOA->INT = temp;
            irq_gpio_handler(GPIO_PIN(PORTA, i));
            return;
        }
    }
}

void isr_gpio(void)
{
    for (int i = 0; i < 16; i++) {
        if ((VC_GPIOC->INT & GPIO_IOX_INT_IOXINT_Msk(i)) != 0) {
            /* clear interrupt status */
            uint32_t temp = VC_GPIOC->INT;
            temp |= GPIO_IOX_INT_IOXINT_Msk(i);
            VC_GPIOC->INT = temp;
            irq_gpio_handler(GPIO_PIN(PORTC, i));
            return;
        }
    }
}
