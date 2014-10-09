/*
** ###################################################################
**     Processor:           MK60N512MD100
**     Compilers:           ARM Compiler
**                          Freescale C/C++ for Embedded ARM
**                          GNU ARM C Compiler
**                          IAR ANSI C/C++ Compiler for ARM
**     Reference manual:    K60P144M100SF2RM, Rev. 3, 4 Nov 2010
**     Version:             rev. 1.6, 2011-01-14
**
**     Abstract:
**         Provides a system configuration function and a global variable that contains the system frequency.
**         It configures the device and initializes the oscillator (PLL) that is part of the microcontroller device.
**
**     Copyright: 2011 Freescale Semiconductor, Inc. All Rights Reserved.
**
**     http:                 www.freescale.com
**     mail:                 support@freescale.com
**
**     Revisions:
**     - rev. 0.1 (2010-09-29)
**         Initial version
**     - rev. 1.0 (2010-10-15)
**         First public version
**     - rev. 1.1 (2010-10-27)
**         Registers updated according to the new reference manual revision - Rev. 2, 15 Oct 2010
**         ADC - Peripheral register PGA bit definition has been fixed, bits PGALP, PGACHP removed.
**         CAN - Peripheral register MCR bit definition has been fixed, bit WAKSRC removed.
**         CRC - Peripheral register layout structure has been extended with 8/16-bit access to shadow registers.
**         CMP - Peripheral base address macro renamed from HSCMPx_BASE to CMPx_BASE.
**         CMP - Peripheral base pointer macro renamed from HSCMPx to CMPx.
**         DMA - Peripheral base address macro renamed from eDMA_BASE to DMA_BASE.
**         DMA - Peripheral base pointer macro renamed from eDMA to DMA.
**         ENET - Statistic event counter register MASK and SHIFT macros removed (#MTWX43372).
**         GPIO - Port Output Enable Register (POER) has been renamed to Port Data Direction Register (PDDR), all POER related macros fixed to PDDR.
**         PDB - Peripheral register layout structure has been extended for Channel n and DAC n register array access (#MTWX44115).
**         RFSYS - System regfile registers have been added (#MTWX43999)
**         RFVBAT - VBAT  regfile registers have been added (#MTWX43999)
**         RNG - Peripheral base address macro renamed from RNGB_BASE to RNG_BASE.
**         RNG - Peripheral base pointer macro renamed from RNGB to RNG.
**         RTC - Peripheral register CR bit definition has been fixed, bit OTE removed.
**         TSI - Peripheral registers STATUS, SCANC bit definition have been fixed, bit groups CAPTRM, DELVOL and AMCLKDIV added.
**         USB - Peripheral base address macro renamed from USBOTG0_BASE to USB0_BASE.
**         USB - Peripheral base pointer macro renamed from USBOTG0 to USB0.
**         VREF - Peripheral register TRM removed.
**     - rev. 1.2 (2010-11-11)
**         Registers updated according to the new reference manual revision - Rev. 3, 4 Nov 2010
**         CAN - Individual Matching Element Update (IMEU) feature has been removed.
**         CAN - Peripheral register layout structure has been fixed, registers IMEUR, LRFR have been removed.
**         CAN - Peripheral register CTRL2 bit definition has been fixed, bits IMEUMASK, LOSTRMMSK, LOSTRLMSK, IMEUEN have been removed.
**         CAN - Peripheral register ESR2 bit definition has been fixed, bits IMEUF, LOSTRMF, LOSTRLF have been removed.
**         NV - Fixed offset address of BACKKEYx, FPROTx registers.
**         TSI - Peripheral register layout structure has been fixed, register WUCNTR has been removed.
**     - rev. 1.3 (2010-11-19)
**         CAN - Support for CAN0_IMEU_IRQn, CAN0_Lost_Rx_IRQn interrupts has been removed.
**         CAN - Support for CAN1_IMEU_IRQn, CAN1_Lost_Rx_IRQn interrupts has been removed.
**     - rev. 1.4 (2010-11-30)
**         EWM - Peripheral base address EWM_BASE definition has been fixed from 0x4005F000u to 0x40061000u (#MTWX44776).
**     - rev. 1.5 (2010-12-17)
**         AIPS0, AIPS1 - Fixed offset of PACRE-PACRP registers (#MTWX45259).
**         CAU - Fixed register definition.
**     - rev. 1.6 (2011-01-14)
**         Added BITBAND_REG() macro to provide access to register bits using bit band region.
**
** ###################################################################
*/

/*! \file MK60N512MD100 */
/*! \version 1.6 */
/*! \date 2011-01-14 */
/*! \brief Device specific configuration file for MK60N512MD100 (implementation file) */
/*! \detailed Provides a system configuration function and a global variable that contains the system frequency.
      It configures the device and initializes the oscillator (PLL) that is part of the microcontroller device. */

#include <stdint.h>
#include "MK60N512MD100.h"

#define DISABLE_WDOG    1


#define CPU_XTAL_CLK_HZ                 50000000u /* Value of the external crystal or oscillator clock frequency in Hz */
#define CPU_XTAL32k_CLK_HZ              32768u   /* Value of the external 32k crystal or oscillator clock frequency in Hz */
#define CPU_INT_SLOW_CLK_HZ             32768u   /* Value of the slow internal oscillator clock frequency in Hz  */
#define CPU_INT_FAST_CLK_HZ             4000000u /* Value of the fast internal oscillator clock frequency in Hz  */
#define DEFAULT_SYSTEM_CLOCK            100000000u /* Default System clock value */						  




/* ----------------------------------------------------------------------------
   -- Core clock
   ---------------------------------------------------------------------------- */

uint32_t SystemCoreClock = DEFAULT_SYSTEM_CLOCK;
//Now running PEE Mode
void set_sys_dividers(uint32_t outdiv1, uint32_t outdiv2, uint32_t outdiv3, uint32_t outdiv4)
{
  uint32_t temp_reg;
  uint8_t i;
 
  temp_reg = FMC->PFAPR; // store present value of FMC_PFAPR
 
  // set M0PFD through M7PFD to 1 to disable prefetch
  FMC->PFAPR |= FMC_PFAPR_M7PFD_MASK | FMC_PFAPR_M6PFD_MASK | FMC_PFAPR_M5PFD_MASK
             | FMC_PFAPR_M4PFD_MASK | FMC_PFAPR_M3PFD_MASK | FMC_PFAPR_M2PFD_MASK
             | FMC_PFAPR_M1PFD_MASK | FMC_PFAPR_M0PFD_MASK;
 
  // set clock dividers to desired value 
  SIM->CLKDIV1 = SIM_CLKDIV1_OUTDIV1(outdiv1) | SIM_CLKDIV1_OUTDIV2(outdiv2)
              | SIM_CLKDIV1_OUTDIV3(outdiv3) | SIM_CLKDIV1_OUTDIV4(outdiv4);

  // wait for dividers to change
  for (i = 0 ; i < outdiv4 ; i++)
  {}
 
  FMC->PFAPR = temp_reg; // re-store original value of FMC_PFAPR
 
  return;
} // set_sys_dividers


/* ----------------------------------------------------------------------------
   -- SystemInit()
   ---------------------------------------------------------------------------- */
void SystemInit (void) 
{
	
	
#if (DISABLE_WDOG)
	/* Disable the WDOG module */
	/* WDOG_UNLOCK: WDOGUNLOCK=0xC520 */
	WDOG->UNLOCK = (uint16_t)0xC520u;     
	/* WDOG_UNLOCK : WDOGUNLOCK=0xD928 */
	WDOG->UNLOCK  = (uint16_t)0xD928u;   
	/* WDOG_STCTRLH: ??=0,DISTESTWDOG=0,BYTESEL=0,TESTSEL=0,TESTWDOG=0,??=0,STNDBYEN=1,WAITEN=1,STOPEN=1,DBGEN=0,ALLOWUPDATE=1,WINEN=0,IRQRSTEN=0,CLKSRC=1,WDOGEN=0 */
	WDOG->STCTRLH = (uint16_t)0x01D2u;
#endif /* (DISABLE_WDOG) */
	
	/* System clock initialization */
	/* Switch to FBE Mode */
	MCG->C2 = 0;
	// after initialization of oscillator release latched state of oscillator and GPIO
	SIM->SCGC4 |= SIM_SCGC4_LLWU_MASK;
	LLWU->CS |= LLWU_CS_ACKISO_MASK;		
	MCG->C1 = MCG_C1_CLKS(2) | MCG_C1_FRDIV(3); //Ñ¡ÔñÍâ²¿¾§	
	while (MCG->S & MCG_S_IREFST_MASK){}; // wait for Reference clock Status bit to clear
	while (((MCG->S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x2){};	
	/* Switch to PBE Mode */
	/* MCG->C5: PLLCLKEN=0,PLLSTEN=0,PRDIV=19 */
//	MCG->C5 = MCG_C5_PRDIV(15-1); //	50/15=3.3M   
	MCG->C5 = MCG_C5_PRDIV(20-1); //	50/20=2.5M  
	MCG->C6 = 0;
//	set_sys_dividers(0,1,2,6);
	set_sys_dividers(0,1,1,3); 

//	MCG->C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV(54-24);//3.3MX54=180M ¼´system clock=180M	
	MCG->C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV(40-24);//2.5MX40=100M ¼´system clock=100M
	while (!(MCG->S & MCG_S_PLLST_MASK)){}; // wait for PLL status bit to set
	while (!(MCG->S & MCG_S_LOCK_MASK)){}; // Wait for LOCK bit to set
	// Now running PBE Mode	
	// Transition into PEE by setting CLKS to 0
	// CLKS=0, FRDIV=3, IREFS=0, IRCLKEN=0, IREFSTEN=0
	MCG->C1 &= ~MCG_C1_CLKS_MASK;
	
	// Wait for clock status bits to update
	while (((MCG->S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x3){};

}


/* ----------------------------------------------------------------------------
   -- SystemCoreClockUpdate()
   ---------------------------------------------------------------------------- */

void SystemCoreClockUpdate (void) {
  uint32_t MCGOUTClock;                                                        /* Variable to store output clock frequency of the MCG module */
  uint8_t Divider;

  if ((MCG->C1 & MCG_C1_CLKS_MASK) == 0x0u) {
    /* Output of FLL or PLL is selected */
    if ((MCG->C6 & MCG_C6_PLLS_MASK) == 0x0u) {
      /* FLL is selected */
      if ((MCG->C1 & MCG_C1_IREFS_MASK) == 0x0u) {
        /* External reference clock is selected */
        if ((SIM->SOPT2 & SIM_SOPT2_MCGCLKSEL_MASK) == 0x0u) {
          MCGOUTClock = CPU_XTAL_CLK_HZ;                                       /* System oscillator drives MCG clock */
        } else { /* (!((SIM->SOPT2 & SIM_SOPT2_MCGCLKSEL_MASK) == 0x0u)) */
          MCGOUTClock = CPU_XTAL32k_CLK_HZ;                                    /* RTC 32 kHz oscillator drives MCG clock */
        } /* (!((SIM->SOPT2 & SIM_SOPT2_MCGCLKSEL_MASK) == 0x0u)) */
        Divider = (uint8_t)(1u << ((MCG->C1 & MCG_C1_FRDIV_MASK) >> MCG_C1_FRDIV_SHIFT));
        MCGOUTClock = (MCGOUTClock / Divider);  /* Calculate the divided FLL reference clock */
        if ((MCG->C2 & MCG_C2_RANGE_MASK) != 0x0u) {
          MCGOUTClock /= 32u;                                                  /* If high range is enabled, additional 32 divider is active */
        } /* ((MCG->C2 & MCG_C2_RANGE_MASK) != 0x0u) */
      } else { /* (!((MCG->C1 & MCG_C1_IREFS_MASK) == 0x0u)) */
        MCGOUTClock = CPU_INT_SLOW_CLK_HZ;                                     /* The slow internal reference clock is selected */
      } /* (!((MCG->C1 & MCG_C1_IREFS_MASK) == 0x0u)) */
      /* Select correct multiplier to calculate the MCG output clock  */
      switch (MCG->C4 & (MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS_MASK)) {
        case 0x0u:
          MCGOUTClock *= 640u;
          break;
        case 0x20u:
          MCGOUTClock *= 1280u;
          break;
        case 0x40u:
          MCGOUTClock *= 1920u;
          break;
        case 0x60u:
          MCGOUTClock *= 2560u;
          break;
        case 0x80u:
          MCGOUTClock *= 732u;
          break;
        case 0xA0u:
          MCGOUTClock *= 1464u;
          break;
        case 0xC0u:
          MCGOUTClock *= 2197u;
          break;
        case 0xE0u:
          MCGOUTClock *= 2929u;
          break;
        default:
          break;
      }
    } else { /* (!((MCG->C6 & MCG_C6_PLLS_MASK) == 0x0u)) */
      /* PLL is selected */
      Divider = (1u + (MCG->C5 & MCG_C5_PRDIV_MASK));
      MCGOUTClock = (uint32_t)(CPU_XTAL_CLK_HZ / Divider);    /* Calculate the PLL reference clock */
      Divider = ((MCG->C6 & MCG_C6_VDIV_MASK) + 24u);
      MCGOUTClock *= Divider;                       /* Calculate the MCG output clock */
    } /* (!((MCG->C6 & MCG_C6_PLLS_MASK) == 0x0u)) */
  } else if ((MCG->C1 & MCG_C1_CLKS_MASK) == 0x40u) {
    /* Internal reference clock is selected */
    if ((MCG->C2 & MCG_C2_IRCS_MASK) == 0x0u) {
      MCGOUTClock = CPU_INT_SLOW_CLK_HZ;                                       /* Slow internal reference clock selected */
    } else { /* (!((MCG->C2 & MCG_C2_IRCS_MASK) == 0x0u)) */
      MCGOUTClock = CPU_INT_FAST_CLK_HZ;                                       /* Fast internal reference clock selected */
    } /* (!((MCG->C2 & MCG_C2_IRCS_MASK) == 0x0u)) */
  } else if ((MCG->C1 & MCG_C1_CLKS_MASK) == 0x80u) {
    /* External reference clock is selected */
    if ((SIM->SOPT2 & SIM_SOPT2_MCGCLKSEL_MASK) == 0x0u) {
      MCGOUTClock = CPU_XTAL_CLK_HZ;                                           /* System oscillator drives MCG clock */
    } else { /* (!((SIM->SOPT2 & SIM_SOPT2_MCGCLKSEL_MASK) == 0x0u)) */
      MCGOUTClock = CPU_XTAL32k_CLK_HZ;                                        /* RTC 32 kHz oscillator drives MCG clock */
    } /* (!((SIM->SOPT2 & SIM_SOPT2_MCGCLKSEL_MASK) == 0x0u)) */
  } else { /* (!((MCG->C1 & MCG_C1_CLKS_MASK) == 0x80u)) */
    /* Reserved value */
    return;
  } /* (!((MCG->C1 & MCG_C1_CLKS_MASK) == 0x80u)) */
  SystemCoreClock = (MCGOUTClock / (1u + ((SIM->CLKDIV1 & SIM_CLKDIV1_OUTDIV1_MASK) >> SIM_CLKDIV1_OUTDIV1_SHIFT)));
}
