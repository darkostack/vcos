#include <assert.h>

#include <vcos/periph/gpio.h>

#include "cpu.h"

/**
 * Sirius has 32 external interupt lines, 16 pins of PORTA and 16 pins of PORTC.
 */
#define EXTI_NUMOF (32U)

/**
 * isr_gpio_ctx index 0-15 will be used to store PORTA pins and the rest will
 * be used for PORTC pins.
 */
static vcGpioIsrContext sIsrGpioContext[EXTI_NUMOF];

static inline uint8_t _port(vcGpio aPin)
{
    /* get the port number from given GPIO_PIN() */
    return (aPin & 0xf);
}

static inline uint8_t _pin(vcGpio aPin)
{
    /* get the pin number from given GPIO_PIN() */
    return ((aPin >> 4) & 0xf);
}

int vcGpioInit(vcGpio aPin, vcGpioMode aMode)
{
    uint32_t temp;

    VC_GPIO_Type *gpio = NULL;

    if (_port(aPin) == PORTA) {
        gpio = (VC_GPIO_Type *)VC_GPIOA;
    } else {
        gpio = (VC_GPIO_Type *)VC_GPIO(_port(aPin));
    }

    assert(gpio != NULL);

    /* set gpio mode */
    if (aMode == GPIO_IN) {
        /* input */
        temp = gpio->OEN;
        temp &= ~GPIO_IOX_OEN_IOXOEN_Msk(_pin(aPin));
        temp |= GPIO_IOX_OEN_IOXOEN_Disabled(_pin(aPin));
        gpio->OEN = temp;

        temp = gpio->IE;
        temp &= ~GPIO_IOX_IE_IOXIE_Msk(_pin(aPin));
        temp |= GPIO_IOX_IE_IOXIE_Enabled(_pin(aPin));
        gpio->IE = temp;

        temp = gpio->ATT;
        temp &= ~GPIO_IOX_ATT_IOXATT_Msk(_pin(aPin));
        temp |= GPIO_IOX_ATT_IOXATT_CMOS(_pin(aPin));
        gpio->ATT = temp;

    } else if (aMode == GPIO_IN_PD) {
        /* input pull-down */
        temp = gpio->OEN;
        temp &= ~GPIO_IOX_OEN_IOXOEN_Msk(_pin(aPin));
        temp |= GPIO_IOX_OEN_IOXOEN_Disabled(_pin(aPin));
        gpio->OEN = temp;

        temp = gpio->IE;
        temp &= ~GPIO_IOX_IE_IOXIE_Msk(_pin(aPin));
        temp |= GPIO_IOX_IE_IOXIE_Enabled(_pin(aPin));
        gpio->IE = temp;

        temp = gpio->ATT;
        temp &= ~GPIO_IOX_ATT_IOXATT_Msk(_pin(aPin));
        temp |= GPIO_IOX_ATT_IOXATT_OPEN_DRAIN(_pin(aPin));
        gpio->ATT = temp;

        temp = gpio->DAT;
        temp &= ~GPIO_IOX_DAT_IOXDAT_Msk(_pin(aPin));
        temp |= GPIO_IOX_DAT_IOXDAT_Disabled(_pin(aPin));
        gpio->DAT = temp;

    } else if (aMode == GPIO_IN_PU) {
        /* input pull-up */
        temp = gpio->OEN;
        temp &= ~GPIO_IOX_OEN_IOXOEN_Msk(_pin(aPin));
        temp |= GPIO_IOX_OEN_IOXOEN_Disabled(_pin(aPin));
        gpio->OEN = temp;

        temp = gpio->IE;
        temp &= ~GPIO_IOX_IE_IOXIE_Msk(_pin(aPin));
        temp |= GPIO_IOX_IE_IOXIE_Enabled(_pin(aPin));
        gpio->IE = temp;

        temp = gpio->ATT;
        temp &= ~GPIO_IOX_ATT_IOXATT_Msk(_pin(aPin));
        temp |= GPIO_IOX_ATT_IOXATT_OPEN_DRAIN(_pin(aPin));
        gpio->ATT = temp;

        temp = gpio->DAT;
        temp &= ~GPIO_IOX_DAT_IOXDAT_Msk(_pin(aPin));
        temp |= GPIO_IOX_DAT_IOXDAT_Enabled(_pin(aPin));
        gpio->DAT = temp;

    } else if (aMode == GPIO_OUT) {
        /* output */
        temp = gpio->OEN;
        temp &= ~GPIO_IOX_OEN_IOXOEN_Msk(_pin(aPin));
        temp |= GPIO_IOX_OEN_IOXOEN_Enabled(_pin(aPin));
        gpio->OEN = temp;

        temp = gpio->IE;
        temp &= ~GPIO_IOX_IE_IOXIE_Msk(_pin(aPin));
        temp |= GPIO_IOX_IE_IOXIE_Disabled(_pin(aPin));
        gpio->IE = temp;

        temp = gpio->ATT;
        temp &= ~GPIO_IOX_ATT_IOXATT_Msk(_pin(aPin));
        temp |= GPIO_IOX_ATT_IOXATT_CMOS(_pin(aPin));
        gpio->ATT = temp;

    } else if (aMode == GPIO_OD) {
        /* open-drain output low */
        temp = gpio->OEN;
        temp &= ~GPIO_IOX_OEN_IOXOEN_Msk(_pin(aPin));
        temp |= GPIO_IOX_OEN_IOXOEN_Enabled(_pin(aPin));
        gpio->OEN = temp;

        temp = gpio->IE;
        temp &= ~GPIO_IOX_IE_IOXIE_Msk(_pin(aPin));
        temp |= GPIO_IOX_IE_IOXIE_Disabled(_pin(aPin));
        gpio->IE = temp;

        temp = gpio->ATT;
        temp &= ~GPIO_IOX_ATT_IOXATT_Msk(_pin(aPin));
        temp |= GPIO_IOX_ATT_IOXATT_OPEN_DRAIN(_pin(aPin));
        gpio->ATT = temp;

        temp = gpio->DAT;
        temp &= ~GPIO_IOX_DAT_IOXDAT_Msk(_pin(aPin));
        temp |= GPIO_IOX_DAT_IOXDAT_Disabled(_pin(aPin));
        gpio->DAT = temp;

    } else if (aMode == GPIO_OD_PU) {
        /* open-drain output high */
        temp = gpio->OEN;
        temp &= ~GPIO_IOX_OEN_IOXOEN_Msk(_pin(aPin));
        temp |= GPIO_IOX_OEN_IOXOEN_Enabled(_pin(aPin));
        gpio->OEN = temp;

        temp = gpio->IE;
        temp &= ~GPIO_IOX_IE_IOXIE_Msk(_pin(aPin));
        temp |= GPIO_IOX_IE_IOXIE_Disabled(_pin(aPin));
        gpio->IE = temp;

        temp = gpio->ATT;
        temp &= ~GPIO_IOX_ATT_IOXATT_Msk(_pin(aPin));
        temp |= GPIO_IOX_ATT_IOXATT_OPEN_DRAIN(_pin(aPin));
        gpio->ATT = temp;

        temp = gpio->DAT;
        temp &= ~GPIO_IOX_DAT_IOXDAT_Msk(_pin(aPin));
        temp |= GPIO_IOX_DAT_IOXDAT_Enabled(_pin(aPin));
        gpio->DAT = temp;

    } else {
        assert(0); /* unknown gpio mode */
    }

    /* reset PORTx pin special function as gpio */
    if (_port(aPin) == PORTA) {
        if (_pin(aPin) < 8) {
            temp = VC_GPIOA->SEL0;
            temp &= ~GPIO_IOA_SEL0_IOAx_SEL_Msk(_pin(aPin));
            temp |= GPIO_SEL0_IOAx_GPIO(_pin(aPin));
            VC_GPIOA->SEL0 = temp;
        } else {
            temp = VC_GPIOA->SEL1;
            temp &= ~GPIO_IOA_SEL1_IOAx_SEL_Msk(_pin(aPin));
            temp |= GPIO_SEL1_IOAx_GPIO(_pin(aPin));
            VC_GPIOA->SEL1 = temp;
        }
    } else if (_port(aPin) == PORTB || _port(aPin) == PORTC) {
        if (_port(aPin) == PORTB) {
            if (_pin(aPin) < 8) {
                temp = VC_GPIOB->SEL0;
                temp &= ~GPIO_IOB_SEL0_IOBx_SEL_Msk(_pin(aPin));
                VC_GPIOB->SEL0 = temp;
            } else {
                temp = VC_GPIOB->SEL1;
                temp &= ~GPIO_IOB_SEL1_IOBx_SEL_Msk(_pin(aPin));
                VC_GPIOB->SEL1 = temp;
            }
        } else {
            if (_pin(aPin) < 8) {
                temp = VC_GPIOC->SEL0;
                temp &= ~GPIO_IOC_SEL0_IOCx_SEL_Msk(_pin(aPin));
                VC_GPIOC->SEL0 = temp;
            } else {
                temp = VC_GPIOC->SEL1;
                temp &= ~GPIO_IOC_SEL1_IOCx_SEL_Msk(_pin(aPin));
                VC_GPIOC->SEL1 = temp;
            }
        }
    } else {
        if (_port(aPin) == PORTD) {
            temp = VC_GPIOD->SEL;
            temp &= ~GPIO_IOD_SEL_IODx_SEL_Msk(_pin(aPin));
            VC_GPIOD->SEL = temp;
        } else if (_port(aPin) == PORTE) {
            temp = VC_GPIOE->SEL;
            temp &= ~GPIO_IOE_SEL_IOEx_SEL_Msk(_pin(aPin));
            VC_GPIOE->SEL = temp;
        } else {
            assert(_pin(aPin) < 4); /* PORTF only support pin (0 - 3) */
            temp = VC_GPIOF->SEL;
            temp &= ~GPIO_IOF_SEL_IOFx_SEL_Msk(_pin(aPin));
            VC_GPIOF->SEL = temp;
        }
    }

    return 0;
}

int vcGpioInitInt(vcGpio aPin,
                  vcGpioMode aMode,
                  vcGpioFlank aFlank,
                  vcGpioCallback aCallback,
                  void *aArg)
{
    uint32_t temp, temp1 = 0;

    assert((_port(aPin) == PORTA) || (_port(aPin) == PORTC));
    assert(aCallback != NULL);

    /* store callback information */
    sIsrGpioContext[(8 * _port(aPin)) + _pin(aPin)].mCallback = aCallback;
    sIsrGpioContext[(8 * _port(aPin)) + _pin(aPin)].mArg = aArg;

    /* initialize pin as input */
    vcGpioInit(aPin, aMode);

    /* enable gpio interrupt */

    if (_port(aPin) == PORTA) {
        /* disable IOA de-glitch circuit */
        temp = VC_PMU->IOANODEG;
        temp |= (1 << _pin(aPin));
        VC_PMU->IOANODEG = temp;

        /* enable pmu interrupt */
        temp = VC_PMU->CONTROL;
        temp |= PMU_CONTROL_INTEN_Enabled;
        VC_PMU->CONTROL = temp;

        /* set wake-up signal */
        temp = VC_GPIOA->WKUEN;
        temp1 = VC_GPIOA->DAT;

        temp &= ~GPIO_IOx_WKUEN_Msk(_pin(aPin));
        temp1 &= ~GPIO_IOX_DAT_IOXDAT_Msk(_pin(aPin));

        if (aFlank == GPIO_FALLING) {
            temp |= (1UL << GPIO_IOx_WKUEN_Pos(_pin(aPin)));
            temp1 |= (1UL << _pin(aPin));
        } else if (aFlank == GPIO_RISING) {
            temp |= (1UL << GPIO_IOx_WKUEN_Pos(_pin(aPin)));
        } else if (aFlank == GPIO_BOTH) {
            temp |= (3UL << GPIO_IOx_WKUEN_Pos(_pin(aPin)));    
        } else {
            assert(0); /* unsupported gpio aFlank signal */
        }

        VC_GPIOA->WKUEN = temp;
        VC_GPIOA->DAT = temp1;

        /* set pin to EINT special function */
        if (_pin(aPin) < 8) {
            temp = VC_GPIOA->SEL0;
            temp |= GPIO_SEL0_IOAx_EINTx(_pin(aPin));
            VC_GPIOA->SEL0 = temp;
        } else {
            temp = VC_GPIOA->SEL1;
            temp |= GPIO_SEL1_IOAx_EINTx(_pin(aPin));
            VC_GPIOA->SEL1 = temp;
        }

    } else {

        /* set wake-up signal */
        temp = VC_GPIOC->WKUEN;
        temp1 = VC_GPIOC->DAT;

        temp &= ~GPIO_IOx_WKUEN_Msk(_pin(aPin));
        temp1 &= ~GPIO_IOX_DAT_IOXDAT_Msk(_pin(aPin));

        if (aFlank == GPIO_FALLING) {
            temp |= (1UL << GPIO_IOx_WKUEN_Pos(_pin(aPin)));
            temp1 |= (1UL << _pin(aPin));
        } else if (aFlank == GPIO_RISING) {
            temp |= (1UL << GPIO_IOx_WKUEN_Pos(_pin(aPin)));
        } else if (aFlank == GPIO_BOTH) {
            temp |= (3UL << GPIO_IOx_WKUEN_Pos(_pin(aPin)));    
        } else {
            assert(0); /* unsupported gpio flank signal */
        }

        VC_GPIOC->WKUEN = temp;
        VC_GPIOC->DAT = temp1;

        /* set pin to EINT special function */
        if (_pin(aPin) < 8) {
            temp = VC_GPIOC->SEL0;
            temp |= GPIO_SEL0_IOCx_EINTx(_pin(aPin));
            VC_GPIOC->SEL0 = temp;
        } else {
            temp = VC_GPIOC->SEL1;
            temp |= GPIO_SEL1_IOCx_EINTx(_pin(aPin));
            VC_GPIOC->SEL1 = temp;
        }
    }

    /* enable global pin interrupt */
    vcGpioIrqEnable(aPin);

    return 0;
}

void vcGpioIrqEnable(vcGpio aPin)
{
    /* enable global pin interrupt */
    if (_port(aPin) == PORTA) {
        NVIC_EnableIRQ(Pmu_IRQn);
    } else {
        NVIC_EnableIRQ(Gpio_IRQn);
    }
}

void vcGpioIrqDisable(vcGpio aPin)
{
    assert((_port(aPin) == PORTA) || (_port(aPin) == PORTC));

    uint32_t temp, temp1 = 0;

    if (_port(aPin) == PORTA) {
        /* enable back IOA de-glitch circuit as default config */
        temp = VC_PMU->IOANODEG;
        temp &= ~(1 << _pin(aPin));
        VC_PMU->IOANODEG = temp;

        /* disable wake-up function */
        temp = VC_GPIOA->WKUEN;
        temp1 = VC_GPIOA->DAT;

        temp &= ~GPIO_IOx_WKUEN_Msk(_pin(aPin));
        temp1 &= ~GPIO_IOX_DAT_IOXDAT_Msk(_pin(aPin));

        VC_GPIOA->WKUEN = temp;
        VC_GPIOA->DAT = temp1;

        /* disable output and input function */
        temp = VC_GPIOA->OEN;
        temp &= ~GPIO_IOX_OEN_IOXOEN_Msk(_pin(aPin));
        temp |= GPIO_IOX_OEN_IOXOEN_Disabled(_pin(aPin));
        VC_GPIOA->OEN = temp;

        temp = VC_GPIOA->IE;
        temp &= ~GPIO_IOX_IE_IOXIE_Msk(_pin(aPin));
        temp |= GPIO_IOX_IE_IOXIE_Disabled(_pin(aPin));
        VC_GPIOA->IE = temp;

        temp = VC_GPIOA->ATT;
        temp &= ~GPIO_IOX_ATT_IOXATT_Msk(_pin(aPin));
        temp |= GPIO_IOX_ATT_IOXATT_CMOS(_pin(aPin));
        VC_GPIOA->ATT = temp;

        /* set to gpio special function */
        if (_pin(aPin) < 8) {
            temp = VC_GPIOA->SEL0;
            temp &= ~GPIO_IOA_SEL0_IOAx_SEL_Msk(_pin(aPin));
            temp |= GPIO_SEL0_IOAx_GPIO(_pin(aPin));
            VC_GPIOA->SEL0 = temp;
        } else {
            temp = VC_GPIOA->SEL1;
            temp &= ~GPIO_IOA_SEL1_IOAx_SEL_Msk(_pin(aPin));
            temp |= GPIO_SEL1_IOAx_GPIO(_pin(aPin));
            VC_GPIOA->SEL1 = temp;
        }
   } else {
        /* disable wake-up function */
        temp = VC_GPIOC->WKUEN;
        temp1 = VC_GPIOC->DAT;

        temp &= ~GPIO_IOx_WKUEN_Msk(_pin(aPin));
        temp1 &= ~GPIO_IOX_DAT_IOXDAT_Msk(_pin(aPin));

        VC_GPIOC->WKUEN = temp;
        VC_GPIOC->DAT = temp1;

        /* disable output and input function */
        temp = VC_GPIOC->OEN;
        temp &= ~GPIO_IOX_OEN_IOXOEN_Msk(_pin(aPin));
        temp |= GPIO_IOX_OEN_IOXOEN_Disabled(_pin(aPin));
        VC_GPIOC->OEN = temp;

        temp = VC_GPIOC->IE;
        temp &= ~GPIO_IOX_IE_IOXIE_Msk(_pin(aPin));
        temp |= GPIO_IOX_IE_IOXIE_Disabled(_pin(aPin));
        VC_GPIOC->IE = temp;

        temp = VC_GPIOC->ATT;
        temp &= ~GPIO_IOX_ATT_IOXATT_Msk(_pin(aPin));
        temp |= GPIO_IOX_ATT_IOXATT_CMOS(_pin(aPin));
        VC_GPIOC->ATT = temp;

        /* set to gpio special function */
        if (_pin(aPin) < 8) {
            temp = VC_GPIOC->SEL0;
            temp &= ~GPIO_IOC_SEL0_IOCx_SEL_Msk(_pin(aPin));
            temp |= GPIO_SEL0_IOCx_GPIO(_pin(aPin));
            VC_GPIOC->SEL0 = temp;
        } else {
            temp = VC_GPIOC->SEL1;
            temp &= ~GPIO_IOC_SEL1_IOCx_SEL_Msk(_pin(aPin));
            temp |= GPIO_SEL1_IOCx_GPIO(_pin(aPin));
            VC_GPIOC->SEL1 = temp;
        }
   }
}

int vcGpioRead(vcGpio aPin)
{
    if (_port(aPin) == PORTF) {
        assert(_pin(aPin) < 4);
    }

    VC_GPIO_Type *gpio = NULL;

    if (_port(aPin) == PORTA) {
        gpio = (VC_GPIO_Type *)VC_GPIOA;
    } else {
        gpio = (VC_GPIO_Type *)VC_GPIO(_port(aPin));
    }

    assert(gpio != NULL);

    return (gpio->DAT & GPIO_IOX_DAT_IOXDAT_Msk(_pin(aPin)));
}

void vcGpioSet(vcGpio aPin)
{
    if (_port(aPin) == PORTF) {
        assert(_pin(aPin) < 4);
    }

    VC_GPIO_Type *gpio = NULL;

    if (_port(aPin) == PORTA) {
        gpio = (VC_GPIO_Type *)VC_GPIOA;
    } else {
        gpio = (VC_GPIO_Type *)VC_GPIO(_port(aPin));
    }

    assert(gpio != NULL);

    uint32_t temp = gpio->DAT;
    temp &= ~GPIO_IOX_DAT_IOXDAT_Msk(_pin(aPin));
    temp |= GPIO_IOX_DAT_IOXDAT_Enabled(_pin(aPin));
    gpio->DAT = temp;
}

void vcGpioClear(vcGpio aPin)
{
    if (_port(aPin) == PORTF) {
        assert(_pin(aPin) < 4);
    }

    VC_GPIO_Type *gpio = NULL;

    if (_port(aPin) == PORTA) {
        gpio = (VC_GPIO_Type *)VC_GPIOA;
    } else {
        gpio = (VC_GPIO_Type *)VC_GPIO(_port(aPin));
    }

    assert(gpio != NULL);

    uint32_t temp = gpio->DAT;
    temp &= ~GPIO_IOX_DAT_IOXDAT_Msk(_pin(aPin));
    temp |= GPIO_IOX_DAT_IOXDAT_Disabled(_pin(aPin));
    gpio->DAT = temp;
}

void vcGpioToggle(vcGpio aPin)
{
    if (vcGpioRead(aPin)) {
        vcGpioClear(aPin);
    } else {
        vcGpioSet(aPin);
    }
}

void vcGpioWrite(vcGpio aPin, int aValue)
{
    if (aValue) {
        vcGpioSet(aPin);
    } else {
        vcGpioClear(aPin);
    }
}

static void _irqGpioHandler(vcGpio aPin)
{
    uint8_t index = (8 * _port(aPin) + _pin(aPin));
    if (sIsrGpioContext[index].mCallback != NULL) {
        sIsrGpioContext[index].mCallback(sIsrGpioContext[index].mArg);
        /* check if context switch was requested */
        // TODO: cortexm_isr_end();
    }
}

/* GPIO interrupt function entry */
void isrPmu(void)
{
    for (int i = 0; i < 16; i++) {
        if ((VC_GPIOA->INT & GPIO_IOX_INT_IOXINT_Msk(i)) != 0) {
            /* clear interrupt status */
            uint32_t temp = VC_GPIOA->INT;
            temp |= GPIO_IOX_INT_IOXINT_Msk(i);
            VC_GPIOA->INT = temp;
            _irqGpioHandler(GPIO_PIN(PORTA, i));
            return;
        }
    }
}

void isrGpio(void)
{
    for (int i = 0; i < 16; i++) {
        if ((VC_GPIOC->INT & GPIO_IOX_INT_IOXINT_Msk(i)) != 0) {
            /* clear interrupt status */
            uint32_t temp = VC_GPIOC->INT;
            temp |= GPIO_IOX_INT_IOXINT_Msk(i);
            VC_GPIOC->INT = temp;
            _irqGpioHandler(GPIO_PIN(PORTC, i));
            return;
        }
    }
}
