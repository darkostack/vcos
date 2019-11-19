#include <assert.h>

#include <vcos/periph/timer.h>

#include "cpu.h"

#define TIMER_NUMOF      (4U)
#define TIMER_CHAN_NUMOF (3U)


static int sTimerIrqn[TIMER_NUMOF] = {
    Pwm0_IRQn,
    Pwm1_IRQn,
    Pwm2_IRQn,
    Pwm3_IRQn
};

static vcTimerIsrContext sIsrTimerCtx[TIMER_NUMOF];

int vcTimerInit(vcTim aDev, unsigned long aFreq, vcTimerCallback aCallback, void *aArg)
{
    assert(aDev < TIMER_NUMOF);

    /* remember the interrupt context */
    sIsrTimerCtx[aDev].mCallback = aCallback;
    sIsrTimerCtx[aDev].mArg = aArg;

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
    vcTimerStart(aDev);

    return 0;
}

int vcTimerSetAbsolute(vcTim aDev, int aChannel, unsigned int aValue)
{
    assert(aDev < TIMER_NUMOF && aChannel < TIMER_CHAN_NUMOF);

    /* config capture/compare register (ccr) period */
    VC_PWM(aDev)->CCR[aChannel] = aValue;

    /* enable capture/compare interrupt */
    uint32_t temp = VC_PWM(aDev)->CCTL[aChannel];

    temp &= ~PWM_CCTL_CCIE_Msk;
    temp |= PWM_CCTL_CCIE_Enabled;

    VC_PWM(aDev)->CCTL[aChannel] = temp;

    /* enable global timer interrupt */
    NVIC_EnableIRQ(sTimerIrqn[aDev]);

    return 0;

}

int vcTimerClear(vcTim aDev, int aChannel)
{
    assert(aDev < TIMER_NUMOF && aChannel < TIMER_CHAN_NUMOF);

    /* clear capture/compare register period to it's default value */
    VC_PWM(aDev)->CCR[aChannel] = (PWM_CCR_CCR_Msk);

    /* clear pwm counter */
    uint32_t temp = VC_PWM(aDev)->CTL;

    temp &= ~PWM_CTL_CLR_Msk;
    temp |= PWM_CTL_CLR_Enabled;

    VC_PWM(aDev)->CTL = temp;

    return 0;

}

uint32_t vcTimerRead(vcTim aDev)
{
    return VC_PWM(aDev)->TAR;
}

void vcTimerStart(vcTim aDev)
{
    assert(aDev < TIMER_NUMOF);

    uint32_t temp = VC_PWM(aDev)->CTL;

    temp &= ~PWM_CTL_MC_Msk;
    temp |= PWM_CTL_MC_Continous;

    VC_PWM(aDev)->CTL = temp;
}

void vcTimerStop(vcTim aDev)
{
    assert(aDev < TIMER_NUMOF);

    uint32_t temp = VC_PWM(aDev)->CTL;

    temp &= ~PWM_CTL_MC_Msk;
    temp |= PWM_CTL_MC_Stop;

    VC_PWM(aDev)->CTL = temp;
}

static void _irqTimerHandler(vcTim aDev)
{
    for (uint8_t ch = 0; ch < TIMER_CHAN_NUMOF; ch++) {
        if ((VC_PWM(aDev)->CCTL[ch] & PWM_CCTL_CCIFG_Msk) != 0) {
            /* clear CCIFG interrupt status & disable CC interupt*/
            uint32_t temp = VC_PWM(aDev)->CCTL[ch];
            temp &= ~PWM_CCTL_CCIE_Msk;
            temp |= PWM_CCTL_CCIFG_Msk;
            temp |= PWM_CCTL_CCIE_Disabled;
            VC_PWM(aDev)->CCTL[ch] = temp;
            /* disable timer global interrupt */
            NVIC_DisableIRQ(sTimerIrqn[aDev]);
            if (sIsrTimerCtx[aDev].mCallback != NULL && VC_PWM(aDev)->CCR[ch] != 0) {
                sIsrTimerCtx[aDev].mCallback(sIsrTimerCtx[aDev].mArg, ch);
                /* check if context switch was requested */
                cortexmIsrEnd();
                break;
            }
        }
    }
}

/* PWM TIMER interrupt function entry */
void isrPwm0(void)
{
    _irqTimerHandler(TIMER_DEV(0));
}

void isrPwm1(void)
{
    _irqTimerHandler(TIMER_DEV(1));
}

void isrPwm2(void)
{
    _irqTimerHandler(TIMER_DEV(2));
}

void isrPwm3(void)
{
    _irqTimerHandler(TIMER_DEV(3));
}
