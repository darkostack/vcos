#include "cpu.h"

uint32_t SystemCoreClock = 0;

void cpuInit(void)
{
    /* initialize the Cortex-M core */
    cortexmInit();

    /* initialize sirius using internal HFRC0 */
    uint32_t temp = VC_MISC2->CLKSEL;
    temp &= ~MISC2_CLKSEL_CLKSEL_Msk;
    temp |= MISC2_CLKSEL_CLKSEL_INT_HFRCO;
    VC_MISC2->CLKSEL = temp;

    /* Update system core clock reference */
    SystemCoreClock = 39321600; /* 39.3216 MHz */

    /* set current selection to 200nA */
    temp = VC_ANA->CLKCTRL1;
    temp &= ~ANA_CLKCTRL1_LFXO_CURSEL_Msk;
    temp |= ANA_CLKCTRL1_LFXO_CURSEL_200nA;
    VC_ANA->CLKCTRL1 = temp;

    /* set HFRCO high frequency mode */
    temp = VC_ANA->CLKCTRL2;
    temp &= ~ANA_CLKCTRL2_HFRCO_HF_Msk;
    temp |= ANA_CLKCTRL2_HFRCO_HF_HFMODE;
    VC_ANA->CLKCTRL2 = temp;

    /* wait until clock is stable */
    while ((VC_PMU->STS & PMU_STS_EXIST_32K_Msk) == 0);
    while ((VC_ANA->CLKCTRL4 & ANA_CLKCTRL4_AC_LOCK_Msk) == 0);

    /* set AHB clock divider */
    temp = VC_MISC2->CLKDIVH;
    temp &= ~MISC2_CLKDIVH_CLKDIVH_Msk;
    temp |= MISC2_CLKDIVH_CLKDIVH_DIV(1);
    VC_MISC2->CLKDIVH = temp;

    /* set APB clock divider */
    temp = VC_MISC2->CLKDIVP;
    temp &= ~MISC2_CLKDIVP_CLKDIVP_Msk;
    temp |= MISC2_CLKDIVP_CLKDIVP_DIV(2);
    VC_MISC2->CLKDIVP = temp;

    /* config analog LFXO capacitor selction */
    temp = VC_ANA->CLKCTRL1;
    temp &= ~ANA_CLKCTRL1_LFXO_CSEL_Msk;
    temp |= ANA_CLKCTRL1_LFXO_CSEL_4; /* 12pF */
    VC_ANA->CLKCTRL1 = temp;

    /* config ANA_LDOCTRL */
    temp = VC_ANA->LDOCTRL;

    temp &= ~ANA_LDOCTRL_VDD3_FON_Msk;
    temp &= ~ANA_LDOCTRL_LPLDO15_VSEL_SLP_Msk;

    temp |= ANA_LDOCTRL_VDD3_FON_Enabled;
    temp |= ANA_LDOCTRL_LPLDO15_VSEL_SLP_2;

    VC_ANA->LDOCTRL = temp;

    /* config powerup delay */
    temp = VC_MISC2->PWRUPDLY;
    temp &= ~MISC2_PWRUPDLY_PWRUPDLY_Msk;
    temp |= MISC2_PWRUPDLY_PWRUPDLY_DELAY(0x3F); /* 63 PCLKs delay */
    VC_MISC2->PWRUPDLY = temp;

    /* disable watchdog timer */
    VC_PMU->WDTPASS = PMU_WDTPASS_UNLOCK_KEY;
    VC_PMU->WDTEN = PMU_WDTEN_WDTEN_Disabled;
    
    /* enable default ahb periph clock */
    temp = VC_MISC2->HCLKEN;

    temp &= ~MISC2_HCLKEN_HCLKEN_Msk;

    temp |= MISC2_HCLKEN_GPIO_Enabled;
    temp |= MISC2_HCLKEN_MEM_Enabled;

    VC_MISC2->CLKEN_UNLOCK = MISC2_CLKEN_UNLOCK_UNLOCK_KEY;
    VC_MISC2->HCLKEN = temp;

    /* enable default apb periph clock */
    temp = VC_MISC2->PCLKEN;

    temp &= ~MISC2_PCLKEN_PCLKEN_Msk;

    temp |= MISC2_PCLKEN_MISC_Enabled;
    temp |= MISC2_PCLKEN_MISC2_Enabled;
    temp |= MISC2_PCLKEN_PMU_Enabled;
    temp |= MISC2_PCLKEN_ANA_Enabled;

    VC_MISC2->CLKEN_UNLOCK = MISC2_CLKEN_UNLOCK_UNLOCK_KEY;
    VC_MISC2->PCLKEN = temp;

    /* wait until clock is stable */
    while ((VC_PMU->STS & PMU_STS_EXIST_32K_Msk) == 0);
    while ((VC_ANA->CLKCTRL4 & ANA_CLKCTRL4_AC_LOCK_Msk) == 0);
}
