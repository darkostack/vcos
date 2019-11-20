#include <assert.h>

#include <vcos/periph/tim.h>

#include "cpu.h"

#define TIM_NUMOF      (4U)
#define TIM_CHAN_NUMOF (3U)

static int sTimIrqn[TIM_NUMOF] = {
    Pwm0_IRQn,
    Pwm1_IRQn,
    Pwm2_IRQn,
    Pwm3_IRQn
};

static vcTimIsrContext sIsrTimCtx[TIM_NUMOF];

int vcTimInit(vcTim aDev, unsigned long aFreq, vcTimCallback aCallback, void *aArg)
{
    assert(aDev < TIM_NUMOF);

    /* remember the interrupt context */
    sIsrTimCtx[aDev].mCallback = aCallback;
    sIsrTimCtx[aDev].mArg = aArg;

    /* enable the peripheral clock */
    uint32_t temp = VC_MISC2->PCLKEN;
    temp |= MISC2_PCLKEN_TIMER_Enabled;
    VC_MISC2->CLKEN_UNLOCK = MISC2_CLKEN_UNLOCK_UNLOCK_KEY;
    VC_MISC2->PCLKEN = temp;

    /* only support 1 MHz frequency base */
    assert(aFreq == 1000000ul);

    /* config pwm control register */
    temp = VC_PWM(aDev)->CTL;

    temp &= ~PWM_CTL_TSEL_Msk;
    temp &= ~PWM_CTL_ID_Msk;

    temp |= PWM_CTL_TSEL_PCLK;
    temp |= PWM_CTL_ID_CLKDIV16; /* (SystemCoreClock / 2 (APBDIV)) / 16 = (39.3216 / 2) / 16 = ~1MHz */

    VC_PWM(aDev)->CTL = temp;

    /* start pwm timer */
    vcTimStart(aDev);

    return 0;
}

int vcTimSetAbsolute(vcTim aDev, int aChannel, unsigned int aValue)
{
    assert(aDev < TIM_NUMOF && aChannel < TIM_CHAN_NUMOF);

    /* config capture/compare register (ccr) period */
    VC_PWM(aDev)->CCR[aChannel] = aValue;

    /* enable capture/compare interrupt */
    uint32_t temp = VC_PWM(aDev)->CCTL[aChannel];

    temp &= ~PWM_CCTL_CCIE_Msk;
    temp |= PWM_CCTL_CCIE_Enabled;

    VC_PWM(aDev)->CCTL[aChannel] = temp;

    /* enable global timer interrupt */
    NVIC_EnableIRQ(sTimIrqn[aDev]);

    return 0;

}

int vcTimClear(vcTim aDev, int aChannel)
{
    assert(aDev < TIM_NUMOF && aChannel < TIM_CHAN_NUMOF);

    /* clear capture/compare register period to it's default value */
    VC_PWM(aDev)->CCR[aChannel] = (PWM_CCR_CCR_Msk);

    /* clear pwm counter */
    uint32_t temp = VC_PWM(aDev)->CTL;

    temp &= ~PWM_CTL_CLR_Msk;
    temp |= PWM_CTL_CLR_Enabled;

    VC_PWM(aDev)->CTL = temp;

    return 0;

}

uint32_t vcTimRead(vcTim aDev)
{
    return VC_PWM(aDev)->TAR;
}

void vcTimStart(vcTim aDev)
{
    assert(aDev < TIM_NUMOF);

    uint32_t temp = VC_PWM(aDev)->CTL;

    temp &= ~PWM_CTL_MC_Msk;
    temp |= PWM_CTL_MC_Continous;

    VC_PWM(aDev)->CTL = temp;
}

void vcTimStop(vcTim aDev)
{
    assert(aDev < TIM_NUMOF);

    uint32_t temp = VC_PWM(aDev)->CTL;

    temp &= ~PWM_CTL_MC_Msk;
    temp |= PWM_CTL_MC_Stop;

    VC_PWM(aDev)->CTL = temp;
}

static void _irqTimHandler(vcTim aDev)
{
    for (uint8_t ch = 0; ch < TIM_CHAN_NUMOF; ch++) {
        if ((VC_PWM(aDev)->CCTL[ch] & PWM_CCTL_CCIFG_Msk) != 0) {
            /* clear CCIFG interrupt status & disable CC interupt*/
            uint32_t temp = VC_PWM(aDev)->CCTL[ch];
            temp &= ~PWM_CCTL_CCIE_Msk;
            temp |= PWM_CCTL_CCIFG_Msk;
            temp |= PWM_CCTL_CCIE_Disabled;
            VC_PWM(aDev)->CCTL[ch] = temp;
            /* disable timer global interrupt */
            NVIC_DisableIRQ(sTimIrqn[aDev]);
            if (sIsrTimCtx[aDev].mCallback != NULL && VC_PWM(aDev)->CCR[ch] != 0) {
                sIsrTimCtx[aDev].mCallback(sIsrTimCtx[aDev].mArg, ch);
                /* check if context switch was requested */
                vcCpuIsrEnd();
                break;
            }
        }
    }
}

/* PWM TIMER interrupt function entry */
void isrPwm0(void)
{
    _irqTimHandler(TIM_DEV(0));
}

void isrPwm1(void)
{
    _irqTimHandler(TIM_DEV(1));
}

void isrPwm2(void)
{
    _irqTimHandler(TIM_DEV(2));
}

void isrPwm3(void)
{
    _irqTimHandler(TIM_DEV(3));
}
