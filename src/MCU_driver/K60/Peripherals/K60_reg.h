
#ifndef _ADX_K60_REG_H_
#define _ADX_K60_REG_H_

#include "adxM3.h"
#include "../LowlevelSystem/MK60N512MD100.h"


// 关闭在匿名union中定义变量的警告
#pragma diag_suppress 1165

namespace K60
{

	/*
	 * @brief	外设基址到到外设位带基址的换算宏定义
	 * @mparam	BASE 外设基址
	 * @notice	该换算只用于外设基址范围0x40000000 ~ 0x400FFFFF
	 *			共1MB空间
	 */
	#define BITBAND_PERIPH_BASE_TO_BITBAND_BASE(BASE) \
		(0x42000000 + (32 * ((UINT32)(BASE) - (UINT32)0x40000000)))
	


	/*! PORT - Register Bit Band Layout Typedef */
	class PORT_BitBandType
	{
	public:
		
		enum PCR_BITS
		{
			PCR_PS           = 0,						/*!< Pull Select */
			PCR_PE           = 1,						/*!< Pull Enable */
			PCR_SRE          = 2,						/*!< Slew Rate Enable */
			PCR_PFE          = 4,						/*!< Passive Filter Enable */
			PCR_ODE          = 5,						/*!< Open Drain Enable */
			PCR_DSE          = 6,						/*!< Drive Strength Enable */
			PCR_MUX_0        = 8,						/*!< Pin Mux Control */
			PCR_MUX_1        = 9,						/*!< Pin Mux Control */
			PCR_MUX_2        = 10,						/*!< Pin Mux Control */
			PCR_LK		     = 15,						/*!< Lock Register */
			PCR_IRQC_0		 = 16,						/*!< Interrupt Configuration */
			PCR_IRQC_1		 = 17,						/*!< Interrupt Configuration */
			PCR_IRQC_2		 = 18,						/*!< Interrupt Configuration */
			PCR_IRQC_3		 = 19,						/*!< Interrupt Configuration */
			PCR_ISF		     = 24,						/*!< Interrupt Status Flag */
		};

		__IO BIT PCR[32][32];                           /*!< Pin Control Register n, array offset: 0x0, array step: 0x4 */
		__O  BIT GPCLR[32];                             /*!< Global Pin Control Low Register, offset: 0x80 */
		__O  BIT GPCHR[32];                             /*!< Global Pin Control High Register, offset: 0x84 */
		     BIT RESERVED_0[24 * 8];
		__IO BIT ISFR[32];								/*!< Interrupt Status Flag Register, offset: 0xA0 */
		     BIT RESERVED_1[28 * 8];
		__IO BIT DFER[32];                              /*!< Digital Filter Enable Register, offset: 0xC0 */
		__IO BIT DFCR[32];                              /*!< Digital Filter Clock Register, offset: 0xC4 */
		__IO BIT DFWR[32];                              /*!< Digital Filter Width Register, offset: 0xC8 */

	};



	/*! GPIO - Register Bit Band Layout Typedef */
	class GPIO_BitBandType
	{
	public:
		
		__IO BIT PDOR[32];                              /*!< Port Data Output Register, offset: 0x0 */
		__O  BIT PSOR[32];                              /*!< Port Set Output Register, offset: 0x4 */
		__O  BIT PCOR[32];                              /*!< Port Clear Output Register, offset: 0x8 */
		__O  BIT PTOR[32];                              /*!< Port Toggle Output Register, offset: 0xC */
		__I  BIT PDIR[32];                              /*!< Port Data Input Register, offset: 0x10 */
		__IO BIT PDDR[32];                              /*!< Port Data Direction Register, offset: 0x14 */
	
	};



	/*! UART - Register Bit Band Layout Typedef */
	class UART_BitBandType 
	{
	public:

		enum BDH_BITS
		{
			BDH_LBKDIE					= 7,
			BDH_RXEDGIE					= 6,
		};

		enum C1_BITS
		{
			C1_LOOPS					= 7,
			C1_UARTSWAI					= 6,
			C1_RSRC						= 5,
			C1_M						= 4,
			C1_WAKE						= 3,
			C1_ILT						= 2,
			C1_PE						= 1,
			C1_PT						= 0,
		};


		enum C2_BITS
		{
			C2_TIE						= 7,
			C2_TCIE						= 6,
			C2_RIE						= 5,
			C2_ILIE						= 4,
			C2_TE						= 3,
			C2_RE						= 2,
			C2_RWU						= 1,
			C2_SBK						= 0,
		};

		enum S1_BITS
		{
			S1_TDRE						= 7,
			S1_TC						= 6,
			S1_RDRF						= 5,
			S1_IDLE						= 4,
			S1_OR						= 3,
			S1_NF						= 2,
			S1_FE						= 1,
			S1_PF						= 0,
		};

		enum S2_BITS
		{
			S2_LBKDIF					= 7,
			S2_RXEDGIF					= 6,
			S2_MSBF						= 5,
			S2_RXINV					= 4,
			S2_RWUID					= 3,
			S2_BRK13					= 2,
			S2_LBKDE					= 1,
			S2_RAF						= 0,
		};

		enum C3_BITS
		{
			C3_R8						= 7,
			C3_T8 						= 6,
			C3_TXDIR 					= 5,
			C3_TXINV 					= 4,
			C3_ORIE 					= 3,
			C3_NEIE 					= 2,
			C3_FEIE 					= 1,
			C3_PEIE						= 0,
		};

		enum C4_BITS
		{
			C4_MAEN1					= 7,
			C4_MAEN2					= 6,
			C4_M10						= 5,
			C4_BRFA_4					= 4,
			C4_BRFA_3					= 3,
			C4_BRFA_2					= 2,
			C4_BRFA_1					= 1,
			C4_BRFA_0					= 0,
		};

		enum C5_BITS
		{
			C5_TDMAS					= 7,
			C5_RDMAS					= 5,
		};

		enum ED_BITS
		{
			NOISY						= 7,
			PARITYE						= 6,
		};

		enum MODEM_BITS
		{
			MODEM_RXRTSE 				= 3,
			MODEM_TXRTSPOL				= 2,
			MODEM_TXRTSE				= 1,
			MODEM_TXCTSE				= 0,
		};

		enum IR_BITS
		{
			IR_IREN 					= 2,
			IR_TNP_1					= 1,
			IR_TNP_0					= 0,
		};

		enum C6_BITS
		{
			C6_EN709					= 7,
			C6_TX709					= 6,
			C6_CE						= 5,
			C6_CP						= 4,
		};

		enum IE0_BITS
		{
			IE0_RPLOFIE					= 2,
			IE0_CTXDIE					= 1,
			IE0_CPTXIE					= 0,
		};

		enum IE_BITS
		{
			IE_PEIE						= 7,
			IE_WBEIE					= 6,
			IE_ISDIE					= 5,
			IE_PRXIE					= 4,
			IE_PTXIE 					= 3,
			IE_PCTEIE					= 2,
			IE_PSIE						= 1,
			IE_TXDIE					= 0,
		};

		enum S3_BITS
		{
			S3_PEF						= 7,
			S3_WBEF						= 6,
			S3_ISD						= 5,
			S3_PRXF						= 4,
			S3_PTXF						= 3,
			S3_PCTEF					= 2,
			S3_PSF						= 1,
			S3_TXFF						= 0,
		};

		enum S4_BITS
		{
			S4_LNF						= 5,
			S4_RPLOF					= 4,
			S4_CDET_1					= 3,
			S4_CDET_0					= 2,
			S4_TXDF 					= 1,
			S4_FE						= 0,
		};



		__IO BIT BDH[8];                                /*!< UART Baud Rate Registers:High, offset: 0x0 */
		__IO BIT BDL[8];                                /*!< UART Baud Rate Registers: Low, offset: 0x1 */
		__IO BIT C1[8];                                 /*!< UART Control Register 1, offset: 0x2 */
		__IO BIT C2[8];                                 /*!< UART Control Register 2, offset: 0x3 */
		__I  BIT S1[8];                                 /*!< UART Status Register 1, offset: 0x4 */
		__IO BIT S2[8];                                 /*!< UART Status Register 2, offset: 0x5 */
		__IO BIT C3[8];                                 /*!< UART Control Register 3, offset: 0x6 */
		__IO BIT D[8];                                  /*!< UART Data Register, offset: 0x7 */
		__IO BIT MA1[8];                                /*!< UART Match Address Registers 1, offset: 0x8 */
		__IO BIT MA2[8];                                /*!< UART Match Address Registers 2, offset: 0x9 */
		__IO BIT C4[8];                                 /*!< UART Control Register 4, offset: 0xA */
		__IO BIT C5[8];                                 /*!< UART Control Register 5, offset: 0xB */
		__I  BIT ED[8];                                 /*!< UART Extended Data Register, offset: 0xC */
		__IO BIT MODEM[8];                              /*!< UART Modem Register, offset: 0xD */
		__IO BIT IR[8];                                 /*!< UART Infrared Register, offset: 0xE */
		     BIT RESERVED_0[1 * 8];
		__IO BIT PFIFO[8];                              /*!< UART FIFO Parameters, offset: 0x10 */
		__IO BIT CFIFO[8];                              /*!< UART FIFO Control Register, offset: 0x11 */
		__IO BIT SFIFO[8];                              /*!< UART FIFO Status Register, offset: 0x12 */
		__IO BIT TWFIFO[8];                             /*!< UART FIFO Transmit Watermark, offset: 0x13 */
		__I  BIT TCFIFO[8];                             /*!< UART FIFO Transmit Count, offset: 0x14 */
		__IO BIT RWFIFO[8];                             /*!< UART FIFO Receive Watermark, offset: 0x15 */
		__I  BIT RCFIFO[8];                             /*!< UART FIFO Receive Count, offset: 0x16 */
		     BIT RESERVED_1[1][8];
		__IO BIT C7816[8];                              /*!< UART 7816 Control Register, offset: 0x18 */
		__IO BIT IE7816[8];                             /*!< UART 7816 Interrupt Enable Register, offset: 0x19 */
		__IO BIT IS7816[8];                             /*!< UART 7816 Interrupt Status Register, offset: 0x1A */
		union 
		{												/* offset: 0x1B */
			__IO BIT WP7816_T_TYPE0[8];                     /*!< UART 7816 Wait Parameter Register, offset: 0x1B */
			__IO BIT WP7816_T_TYPE1[8];                     /*!< UART 7816 Wait Parameter Register, offset: 0x1B */
		};
		__IO BIT WN7816[8];                             /*!< UART 7816 Wait N Register, offset: 0x1C */
		__IO BIT WF7816[8];                             /*!< UART 7816 Wait FD Register, offset: 0x1D */
		__IO BIT ET7816[8];                             /*!< UART 7816 Error Threshold Register, offset: 0x1E */
		__IO BIT TL7816[8];                             /*!< UART 7816 Transmit Length Register, offset: 0x1F */
	};



	/*! ADC - Register Bit Band Layout Typedef */
	class ADC_BitBandType
	{
	public:


		enum SC1_BITS
		{
			SC1_COCO					= 7,
			SC1_AIEN					= 6,
			SC1_DIFF 					= 5,
			SC1_ADCH_4					= 4,
			SC1_ADCH_3					= 3,
			SC1_ADCH_2					= 2,
			SC1_ADCH_1					= 1,
			SC1_ADCH_0					= 0,
		};


		enum CFG1_BITS
		{
			CFG1_ADLPC					= 7,
			CFG1_ADIV_1					= 6,
			CFG1_ADIV_0					= 5,
			CFG1_ADLSMP					= 4,
			CFG1_MODE_1					= 3,
			CFG1_MODE_0					= 2,
			CFG1_ADICLK_1				= 1,
			CFG1_ADICLK_0				= 0,
		};


		enum CFG2_BITS
		{
			CFG2_MUXSEL					= 4,
			CFG2_ADACKEN				= 3,
			CFG2_ADHSC					= 2,
			CFG2_ADLSTS_1				= 1,
			CFG2_ADLSTS_0				= 0,
		};


		enum SC2_BITS
		{
			SC2_ADACT					= 7,
			SC2_ADTRG					= 6,
			SC2_ACFE					= 5,
			SC2_ACFGT					= 4,
			SC2_ACREN					= 3,
			SC2_DMAEN					= 2,
			SC2_REFSEL_1				= 1,
			SC2_REFSEL_0				= 0,
		};


		enum SC3_BITS
		{
			SC3_CAL						= 7,
			SC3_CALF					= 6,
			SC3_ADCO					= 3,
			SC3_AVGE					= 2,
			SC3_AVGS_1					= 1,
			SC3_AVGS_0					= 0,
		};	



		__IO BIT SC1[2][32];                            /*!< ADC status and control registers 1, array offset: 0x0, array step: 0x4 */
		__IO BIT CFG1[32];                              /*!< ADC configuration register 1, offset: 0x8 */
		__IO BIT CFG2[32];                              /*!< Configuration register 2, offset: 0xC */
		__I  BIT R[2][32];                              /*!< ADC data result register, array offset: 0x10, array step: 0x4 */
		__IO BIT CV1[32];                               /*!< Compare value registers, offset: 0x18 */
		__IO BIT CV2[32];                               /*!< Compare value registers, offset: 0x1C */
		__IO BIT SC2[32];                               /*!< Status and control register 2, offset: 0x20 */
		__IO BIT SC3[32];                               /*!< Status and control register 3, offset: 0x24 */
		__IO BIT OFS[32];                               /*!< ADC offset correction register, offset: 0x28 */
		__IO BIT PG[32];                                /*!< ADC plus-side gain register, offset: 0x2C */
		__IO BIT MG[32];                                /*!< ADC minus-side gain register, offset: 0x30 */
		__IO BIT CLPD[32];                              /*!< ADC plus-side general calibration value register, offset: 0x34 */
		__IO BIT CLPS[32];                              /*!< ADC plus-side general calibration value register, offset: 0x38 */
		__IO BIT CLP4[32];                              /*!< ADC plus-side general calibration value register, offset: 0x3C */
		__IO BIT CLP3[32];                              /*!< ADC plus-side general calibration value register, offset: 0x40 */
		__IO BIT CLP2[32];                              /*!< ADC plus-side general calibration value register, offset: 0x44 */
		__IO BIT CLP1[32];                              /*!< ADC plus-side general calibration value register, offset: 0x48 */
		__IO BIT CLP0[32];                              /*!< ADC plus-side general calibration value register, offset: 0x4C */
		__IO BIT PGA[32];                               /*!< ADC PGA register, offset: 0x50 */
		__IO BIT CLMD[32];                              /*!< ADC minus-side general calibration value register, offset: 0x54 */
		__IO BIT CLMS[32];                              /*!< ADC minus-side general calibration value register, offset: 0x58 */
		__IO BIT CLM4[32];                              /*!< ADC minus-side general calibration value register, offset: 0x5C */
		__IO BIT CLM3[32];                              /*!< ADC minus-side general calibration value register, offset: 0x60 */
		__IO BIT CLM2[32];                              /*!< ADC minus-side general calibration value register, offset: 0x64 */
		__IO BIT CLM1[32];                              /*!< ADC minus-side general calibration value register, offset: 0x68 */
		__IO BIT CLM0[32];                              /*!< ADC minus-side general calibration value register, offset: 0x6C */

	};



	/*! PIT - Register Bit Band Layout Typedef */
	class PIT_BitBandType
	{
	public:


		enum MCR_BITS
		{
			MCR_MDIS					= 1,
			MCR_FRZ						= 0,
		};


		enum TCTRL_BITS
		{
			TCTRL_CHN					= 2,
			TCTRL_TIE					= 1,
			TCTRL_TEN					= 0,
		};


		enum TFLG_BITS
		{	
			TFLG_TIF					= 0,
		};
				

		__IO BIT MCR[32];                               /*!< PIT Module Control Register, offset: 0x0 */
			 BIT RESERVED_0[252 * 8];
		struct 
		{												/* offset: 0x100, array step: 0x10 */
			__IO BIT LDVAL[32];							/*!< Timer Load Value Register, array offset: 0x100, array step: 0x10 */
			__I  BIT CVAL[32];							/*!< Current Timer Value Register, array offset: 0x104, array step: 0x10 */
			__IO BIT TCTRL[32];							/*!< Timer Control Register, array offset: 0x108, array step: 0x10 */
			__IO BIT TFLG[32];							/*!< Timer Flag Register, array offset: 0x10C, array step: 0x10 */
		} CHANNEL[4];
	};



	/*! PIT - Register Bit Band Layout Typedef */
	class FTM_BitBandType
	{
	public:

		enum SC_BITS
		{
			SC_TOF						= 7,
			SC_TOIE						= 6,
			SC_CPWMS					= 5,
			SC_CLKS_1					= 4,
			SC_CLKS_0					= 3,
			SC_PS_2						= 2,
			SC_PS_1						= 1,
			SC_PS_0						= 0,
		};


		enum CnSC_BITS
		{
			CnSC_CHF					= 7,
			CnSC_CHIE 					= 6,
			CnSC_MSB 					= 5,
			CnSC_MSA					= 4,
			CnSC_ELSB					= 3,
			CnSC_ELSA					= 2,
			CnSC_DMA					= 0,
		};


		enum STATUS_BITS
		{
			STATUS_CH7F					= 7,
			STATUS_CH6F					= 6,
			STATUS_CH5F					= 5,
			STATUS_CH4F					= 4,
			STATUS_CH3F					= 3,
			STATUS_CH2F					= 2,
			STATUS_CH1F					= 1,
			STATUS_CH0F					= 0,
		};


		enum MODE_BITS
		{
			MODE_FAULTIE				= 7,
			MODE_FAULTM_1				= 6,
			MODE_FAULTM_0				= 5,
			MODE_CAPTEST				= 4,
			MODE_PWMSYNC				= 3,
			MODE_WPDIS					= 2,
			MODE_INIT					= 1,
			MODE_FTMEN					= 0,
		};


		enum SYNC_BITS
		{
			SYNC_SWSYNC					= 7,
			SYNC_TRIG2					= 6,
			SYNC_TRIG1					= 5,
			SYNC_TRIG0					= 4,
			SYNC_SYNCHOM				= 3,
			SYNC_REINIT					= 2,
			SYNC_CNTMAX					= 1,
			SYNC_CNTMIN					= 0,
		};


		enum OUTINIT_BITS
		{
			OUTINIT_CH7OI				= 7,
			OUTINIT_CH6OI				= 6,
			OUTINIT_CH5OI				= 5,
			OUTINIT_CH4OI				= 4,
			OUTINIT_CH3OI				= 3,
			OUTINIT_CH2OI				= 2,
			OUTINIT_CH1OI				= 1,
			OUTINIT_CH0OI				= 0,
		};


		enum OUTMASK_BITS
		{
			OUTMASK_CH7OM				= 7,
			OUTMASK_CH6OM				= 6,
			OUTMASK_CH5OM				= 5,
			OUTMASK_CH4OM				= 4,
			OUTMASK_CH3OM				= 3,
			OUTMASK_CH2OM				= 2,
			OUTMASK_CH1OM				= 1,
			OUTMASK_CH0OM				= 0,
		};


		enum COMBINE_BITS
		{
			COMBINE_FAULTEN				= 6,
			COMBINE_SYNCEN				= 5,
			COMBINE_DTEN				= 4,
			COMBINE_DECAP				= 3,
			COMBINE_DECAPEN				= 2,
			COMBINE_COMP				= 1,
			COMBINE_COMBINE				= 0,
		};


		enum DEADTIME_BITS
		{
			DEADTIME_DTPS				= 6,
			DEADTIME_DTVAL_5			= 5,
			DEADTIME_DTVAL_4			= 4,
			DEADTIME_DTVAL_3			= 3,
			DEADTIME_DTVAL_2			= 2,
			DEADTIME_DTVAL_1			= 1,
			DEADTIME_DTVAL_0			= 0,
		};


		enum EXTTRIG_BITS
		{
			EXTTRIG_TRIGF				= 7,
			EXTTRIG_INITTRIGEN			= 6,
			EXTTRIG_CH1TRIG				= 5,
			EXTTRIG_CH0TRIG				= 4,
			EXTTRIG_CH5TRIG				= 3,
			EXTTRIG_CH4TRIG				= 2,
			EXTTRIG_CH3TRIG				= 1,
			EXTTRIG_CH2TRIG				= 0,
		};

		enum POL_BITS
		{
			POL_POL7					= 7,
			POL_POL6					= 6,
			POL_POL5					= 5,
			POL_POL4 					= 4,
			POL_POL3 					= 3,
			POL_POL2 					= 2,
			POL_POL1 					= 1,
			POL_POL0					= 0,
		};


		enum FMS_BITS
		{
			FMS_FAULTF					= 7,
			FMS_WPEN					= 6,
			FMS_FAULTIN					= 5,
			FMS_FAULTF3					= 3,
			FMS_FAULTF2					= 2,
			FMS_FAULTF1					= 1,
			FMS_FAULTF0					= 0,
		};


		enum FLTCTRL_BITS
		{
			FLTCTRL_FFVAL_3				= 11,
			FLTCTRL_FFVAL_2				= 10,
			FLTCTRL_FFVAL_1				= 9,
			FLTCTRL_FFVAL_0				= 8,
			FLTCTRL_FFLTR3E0			= 7,
			FLTCTRL_FFLTR2EN			= 6,
			FLTCTRL_FFLTR1EN			= 5,
			FLTCTRL_FFLTR0EN			= 4,
			FLTCTRL_FAULT3EN			= 3,
			FLTCTRL_FAULT2EN			= 2,
			FLTCTRL_FAULT1EN			= 1,
			FLTCTRL_FAULT0EN			= 0,
		};


		enum QDCTRL_BITS
		{
			QDCTRL_PHAFLTREN			= 7,
			QDCTRL_PHBFLTREN			= 6,
			QDCTRL_PHAPOL				= 5,
			QDCTRL_PHBPOL				= 4,
			QDCTRL_QUADMODE				= 3,
			QDCTRL_QUADIR				= 2,
			QDCTRL_TOFDIR				= 1,
			QDCTRL_QUADEN				= 0,
		};


		enum CONF_BITS
		{
			CONF_GTBEOUT				= 10,
			CONF_GTBEEN					= 9,
			CONF_BDMMODE_1				= 7,
			CONF_BDMMODE_0				= 6,
			CONF_NUMTOF_4				= 4,
			CONF_NUMTOF_3				= 3,
			CONF_NUMTOF_2				= 2,
			CONF_NUMTOF_1				= 1,
			CONF_NUMTOF_0				= 0,
		};


		enum FLTPOL_BITS
		{
			FLTPOL_FLT3POL				= 3,
			FLTPOL_FLT2POL				= 2,
			FLTPOL_FLT1POL				= 1,
			FLTPOL_FLT0POL				= 0,
		};


		enum SYNCONF_BITS
		{
			SYNCONF_HWSOC				= 20,
			SYNCONF_HWINVC				= 19,
			SYNCONF_HWOM				= 18,
			SYNCONF_HWWRBUF				= 17,
			SYNCONF_HWRSTCNT			= 16,
			SYNCONF_SWSOC				= 12,
			SYNCONF_SWINVC				= 11,
			SYNCONF_SWOM				= 10,
			SYNCONF_SWWRBUF				= 9,
			SYNCONF_SWRSTCNT			= 8,
			SYNCONF_SYNCMODE			= 7,
			SYNCONF_SWOC				= 5,
			SYNCONF_INVC				= 4,
			SYNCONF_CNTINC				= 2,
			SYNCONF_HWTRIGMODE			= 0,
		};

		
		enum INVCTRL_BITS
		{
			INVCTRL_INV3EN				= 3,
			INVCTRL_INV2EN				= 2,
			INVCTRL_INV1EN				= 1,
			INVCTRL_INV0EN				= 0,
		};


		enum SWOCTRL_BITS
		{
			SWOCTRL_CH7OCV				= 15,
			SWOCTRL_CH6OCV				= 14,
			SWOCTRL_CH5OCV				= 13,
			SWOCTRL_CH4OCV				= 12,
			SWOCTRL_CH3OCV				= 11,
			SWOCTRL_CH2OCV				= 10,
			SWOCTRL_CH1OCV				= 9,
			SWOCTRL_CH0OCV				= 8,
			SWOCTRL_CH7OC				= 7,
			SWOCTRL_CH6OC				= 6,
			SWOCTRL_CH5OC				= 5,
			SWOCTRL_CH4OC				= 4,
			SWOCTRL_CH3OC				= 3,
			SWOCTRL_CH2OC				= 2,
			SWOCTRL_CH1OC				= 1,
			SWOCTRL_CH0OC				= 0,
		};


		enum PWMLOAD_BITS
		{
			PWMLOAD_LDOK				= 9,
			PWMLOAD_CH7SEL				= 7,
			PWMLOAD_CH6SEL				= 6,
			PWMLOAD_CH5SEL				= 5,
			PWMLOAD_CH4SEL				= 4,
			PWMLOAD_CH3SEL				= 3,
			PWMLOAD_CH2SEL				= 2,
			PWMLOAD_CH1SEL				= 1,
			PWMLOAD_CH0SEL				= 0,
		};



		__IO BIT SC[32];                                /*!< Status and Control, offset: 0x0 */
		__IO BIT CNT[32];                               /*!< Counter, offset: 0x4 */
		__IO BIT MOD[32];                               /*!< Modulo, offset: 0x8 */
		struct 
		{												/* offset: 0xC, array step: 0x8 */
			__IO BIT CnSC[32];                              /*!< Channel (n) Status and Control, array offset: 0xC, array step: 0x8 */
			__IO BIT CnV[32];                               /*!< Channel (n) Value, array offset: 0x10, array step: 0x8 */
		} CONTROLS[8];
		__IO BIT CNTIN[32];                             /*!< Counter Initial Value, offset: 0x4C */
		__I  BIT STATUS[32];                            /*!< Capture and Compare Status, offset: 0x50 */
		__IO BIT MODE[32];                              /*!< Features Mode Selection, offset: 0x54 */
		__IO BIT SYNC[32];                              /*!< Synchronization, offset: 0x58 */
		__IO BIT OUTINIT[32];                           /*!< Initial State for Channels Output, offset: 0x5C */
		__IO BIT OUTMASK[32];                           /*!< Output Mask, offset: 0x60 */
		__IO BIT COMBINE[4][8];                         /*!< Function for Linked Channels, offset: 0x64 */
		__IO BIT DEADTIME[32];                          /*!< Deadtime Insertion Control, offset: 0x68 */
		__IO BIT EXTTRIG[32];                           /*!< FTM External Trigger, offset: 0x6C */
		__IO BIT POL[32];                               /*!< Channels Polarity, offset: 0x70 */
		__IO BIT FMS[32];                               /*!< Fault Mode Status, offset: 0x74 */
		__IO BIT FILTER[32];                            /*!< Input Capture Filter Control, offset: 0x78 */
		__IO BIT FLTCTRL[32];                           /*!< Fault Control, offset: 0x7C */
		__IO BIT QDCTRL[32];                            /*!< Quadrature Decoder Control and Status, offset: 0x80 */
		__IO BIT CONF[32];                              /*!< Configuration, offset: 0x84 */
		__IO BIT FLTPOL[32];                            /*!< FTM Fault Input Polarity, offset: 0x88 */
		__IO BIT SYNCONF[32];                           /*!< Synchronization Configuration, offset: 0x8C */
		__IO BIT INVCTRL[32];                           /*!< FTM Inverting Control, offset: 0x90 */
		__IO BIT SWOCTRL[32];                           /*!< FTM Software Output Control, offset: 0x94 */
		__IO BIT PWMLOAD[32];                           /*!< FTM PWM Load, offset: 0x98 */
	};



	/*! SPI - Register Bit Band Layout Typedef */
	class SPI_BitBandType
	{
	public:


		enum MCR_BITS
		{
			MCR_MSTR					= 31,
			MCR_CONT_SCKE				= 30,
			MCR_DCONF_1 				= 29,
			MCR_DCONF_0					= 28,
			MCR_FRZ						= 27,
			MCR_MTFE					= 26,
			MCR_ROOE					= 24,
			MCR_PCSIS_4					= 20,
			MCR_PCSIS_3					= 19,
			MCR_PCSIS_2					= 18,
			MCR_PCSIS_1					= 17,
			MCR_PCSIS_0					= 16,
			MCR_DOZE					= 15,
			MCR_MDIS					= 14,
			MCR_DIS_TXF					= 13,
			MCR_DIS_RXF					= 12,
			MCR_CLR_TXF					= 11,
			MCR_CLR_RXF					= 10,
			MCR_SMPL_PT_1				= 9,
			MCR_SMPL_PT_0				= 8,
			MCR_HALT					= 0,
		};


		enum CTAR_BITS
		{
			CTAR_DBR 					= 31,
			CTAR_FMSZ_3					= 30,
			CTAR_FMSZ_2					= 29,
			CTAR_FMSZ_1					= 28,
			CTAR_FMSZ_0					= 27,
			CTAR_CPOL					= 26,
			CTAR_CPHA					= 25,
			CTAR_LSBFE					= 24,
			CTAR_PCSSCK_1 				= 23,
			CTAR_PCSSCK_0				= 22,
			CTAR_PASC_1 				= 21,
			CTAR_PASC_0					= 20,
			CTAR_PDT_1 					= 19,
			CTAR_PDT_0					= 18,
			CTAR_PBR_1					= 17,
			CTAR_PBR_0					= 16,
			CTAR_CSSCK_3 				= 15,
			CTAR_CSSCK_2 				= 14,
			CTAR_CSSCK_1 				= 13,
			CTAR_CSSCK_0 				= 12,
			CTAR_ASC_3 					= 11,
			CTAR_ASC_2					= 10,
			CTAR_ASC_1					= 9,
			CTAR_ASC_0					= 8,
			CTAR_DT_3 					= 7,
			CTAR_DT_2					= 6,
			CTAR_DT_1					= 5,
			CTAR_DT_0					= 4,
			CTAR_BR_3					= 3,
			CTAR_BR_2					= 2,
			CTAR_BR_1					= 1,
			CTAR_BR_0					= 0,
		};


		enum SR_BITS
		{
			SR_TCF						= 31,
			SR_TXRXS					= 30,
			SR_EOQF						= 28,
			SR_TFUF						= 27,
			SR_TFFF						= 25,
			SR_RFOF						= 19,
			SR_RFDF						= 17,
		};


		enum RSER_BITS
		{
			RSER_TCF_RE					= 31,
			RSER_EOQF_RE				= 28,
			RSER_TFUF_RE				= 27,
			RSER_TFFF_RE				= 25,
			RSER_TFFF_DIRS				= 24,
			RSER_RFOF_RE				= 19,
			RSER_RFDF_RE				= 17,
			RSER_RFDF_DIRS				= 16,
		};


		enum PUSHR_BITS
		{
			PUSHR_CFG_CONT				= 31,
			PUSHR_CFG_CTAS_2 			= 30,
			PUSHR_CFG_CTAS_1			= 29,
			PUSHR_CFG_CTAS_0			= 28,
			PUSHR_CFG_EOQ				= 27,
			PUSHR_CFG_CTCNT				= 26,
			PUSHR_CFG_PCS_5				= 21,
			PUSHR_CFG_PCS_4				= 20,
			PUSHR_CFG_PCS_3				= 19,
			PUSHR_CFG_PCS_2				= 18,
			PUSHR_CFG_PCS_1				= 17,
			PUSHR_CFG_PCS_0				= 16,
		};

		__IO BIT MCR[32];                               /*!< DSPI Module Configuration Register, offset: 0x0 */
			 BIT RESERVED_0[4 * 8];
		__IO BIT TCR[32];                               /*!< DSPI Transfer Count Register, offset: 0x8 */
		union 
		{                                          /* offset: 0xC */
			__IO BIT CTAR[2][32];                           /*!< DSPI Clock and Transfer Attributes Register (In Master Mode), array offset: 0xC, array step: 0x4 */
			__IO BIT CTAR_SLAVE[1][32];                     /*!< DSPI Clock and Transfer Attributes Register (In Slave Mode), array offset: 0xC, array step: 0x4 */
		};
		     BIT RESERVED_1[24 * 8];
		__IO BIT SR[32];                                /*!< DSPI Status Register, offset: 0x2C */
		__IO BIT RSER[32];                              /*!< DSPI DMA/Interrupt Request Select and Enable Register, offset: 0x30 */
		union 
		{                                          /* offset: 0x34 */
			__IO BIT PUSHR[32];                             /*!< DSPI PUSH TX FIFO Register In Master Mode, offset: 0x34 */
			__IO BIT PUSHR_SLAVE[32];                       /*!< DSPI PUSH TX FIFO Register In Slave Mode, offset: 0x34 */
		};
		__I  BIT POPR[32];                              /*!< DSPI POP RX FIFO Register, offset: 0x38 */
		__I  BIT TXFR0[32];                             /*!< DSPI Transmit FIFO Registers, offset: 0x3C */
		__I  BIT TXFR1[32];                             /*!< DSPI Transmit FIFO Registers, offset: 0x40 */
		__I  BIT TXFR2[32];                             /*!< DSPI Transmit FIFO Registers, offset: 0x44 */
		__I  BIT TXFR3[32];                             /*!< DSPI Transmit FIFO Registers, offset: 0x48 */
		     BIT RESERVED_2[48 * 8];
		__I  BIT RXFR0[32];                             /*!< DSPI Receive FIFO Registers, offset: 0x7C */
		__I  BIT RXFR1[32];                             /*!< DSPI Receive FIFO Registers, offset: 0x80 */
		__I  BIT RXFR2[32];                             /*!< DSPI Receive FIFO Registers, offset: 0x84 */
		__I  BIT RXFR3[32];                             /*!< DSPI Receive FIFO Registers, offset: 0x88 */

	};



	/*! DMA - Register Bit Band Layout Typedef */
	class DMA_BitBandType
	{
	public:



		__IO BIT CR[32];                                /*!< Control Register, offset: 0x0 */
		__I  BIT ES[32];                                /*!< Error Status Register, offset: 0x4 */
			 BIT RESERVED_0[4 * 8];
		__IO BIT ERQ[32];                               /*!< Enable Request Register, offset: 0xC */
			 BIT RESERVED_1[4 * 8];
		__IO BIT EEI[32];                               /*!< Enable Error Interrupt Register, offset: 0x14 */
		__O  BIT CEEI[8];                               /*!< Clear Enable Error Interrupt Register, offset: 0x18 */
		__O  BIT SEEI[8];                               /*!< Set Enable Error Interrupt Register, offset: 0x19 */
		__O  BIT CERQ[8];                               /*!< Clear Enable Request Register, offset: 0x1A */
		__O  BIT SERQ[8];                               /*!< Set Enable Request Register, offset: 0x1B */
		__O  BIT CDNE[8];                               /*!< Clear DONE Status Bit Register, offset: 0x1C */
		__O  BIT SSRT[8];                               /*!< Set START Bit Register, offset: 0x1D */
		__O  BIT CERR[8];                               /*!< Clear Error Register, offset: 0x1E */
		__O  BIT CINT[8];                               /*!< Clear Interrupt Request Register, offset: 0x1F */
			 BIT RESERVED_2[4 * 8];
		__IO BIT INT[32];                               /*!< Interrupt Request Register, offset: 0x24 */
			 BIT RESERVED_3[4 * 8];
		__IO BIT ERR[32];                               /*!< Error Register, offset: 0x2C */
			 BIT RESERVED_4[4 * 8];
		__IO BIT HRS[32];                               /*!< Hardware Request Status Register, offset: 0x34 */
			 BIT RESERVED_5[200 * 8];
		__IO BIT DCHPRI3[8];                            /*!< Channel n Priority Register, offset: 0x100 */
		__IO BIT DCHPRI2[8];                            /*!< Channel n Priority Register, offset: 0x101 */
		__IO BIT DCHPRI1[8];                            /*!< Channel n Priority Register, offset: 0x102 */
		__IO BIT DCHPRI0[8];                            /*!< Channel n Priority Register, offset: 0x103 */
		__IO BIT DCHPRI7[8];                            /*!< Channel n Priority Register, offset: 0x104 */
		__IO BIT DCHPRI6[8];                            /*!< Channel n Priority Register, offset: 0x105 */
		__IO BIT DCHPRI5[8];                            /*!< Channel n Priority Register, offset: 0x106 */
		__IO BIT DCHPRI4[8];                            /*!< Channel n Priority Register, offset: 0x107 */
		__IO BIT DCHPRI11[8];                           /*!< Channel n Priority Register, offset: 0x108 */
		__IO BIT DCHPRI10[8];                           /*!< Channel n Priority Register, offset: 0x109 */
		__IO BIT DCHPRI9[8];                            /*!< Channel n Priority Register, offset: 0x10A */
		__IO BIT DCHPRI8[8];                            /*!< Channel n Priority Register, offset: 0x10B */
		__IO BIT DCHPRI15[8];                           /*!< Channel n Priority Register, offset: 0x10C */
		__IO BIT DCHPRI14[8];                           /*!< Channel n Priority Register, offset: 0x10D */
		__IO BIT DCHPRI13[8];                           /*!< Channel n Priority Register, offset: 0x10E */
		__IO BIT DCHPRI12[8];                           /*!< Channel n Priority Register, offset: 0x10F */
			 BIT RESERVED_6[3824 * 8];
		struct 
		{                                         /* offset: 0x1000, array step: 0x20 */
			__IO BIT SADDR[32];                             /*!< TCD Source Address, array offset: 0x1000, array step: 0x20 */
			__IO BIT SOFF[16];                              /*!< TCD Signed Source Address Offset, array offset: 0x1004, array step: 0x20 */
			__IO BIT ATTR[16];                              /*!< TCD Transfer Attributes, array offset: 0x1006, array step: 0x20 */
			union 
			{                                          /* offset: 0x1008, array step: 0x20 */
				__IO BIT NBYTES_MLNO[32];                       /*!< TCD Minor Byte Count (Minor Loop Disabled), array offset: 0x1008, array step: 0x20 */
				__IO BIT NBYTES_MLOFFNO[32];                    /*!< TCD Signed Minor Loop Offset (Minor Loop Enabled and Offset Disabled), array offset: 0x1008, array step: 0x20 */
				__IO BIT NBYTES_MLOFFYES[32];                   /*!< TCD Signed Minor Loop Offset (Minor Loop and Offset Enabled), array offset: 0x1008, array step: 0x20 */
			};
				__IO BIT SLAST[32];                             /*!< TCD Last Source Address Adjustment, array offset: 0x100C, array step: 0x20 */
				__IO BIT DADDR[32];                             /*!< TCD Destination Address, array offset: 0x1010, array step: 0x20 */
				__IO BIT DOFF[16];                              /*!< TCD Signed Destination Address Offset, array offset: 0x1014, array step: 0x20 */
			union 
			{                                          /* offset: 0x1016, array step: 0x20 */
				__IO BIT CITER_ELINKYES[16];                    /*!< TCD Current Minor Loop Link, Major Loop Count (Channel Linking Enabled), array offset: 0x1016, array step: 0x20 */
				__IO BIT CITER_ELINKNO[16];                     /*!< TCD Current Minor Loop Link, Major Loop Count (Channel Linking Disabled), array offset: 0x1016, array step: 0x20 */
			};
				__IO BIT DLAST_SGA[32];                         /*!< TCD Last Destination Address Adjustment/Scatter Gather Address, array offset: 0x1018, array step: 0x20 */
				__IO BIT CSR[16];                               /*!< TCD Control and Status, array offset: 0x101C, array step: 0x20 */
			union 
			{                                          /* offset: 0x101E, array step: 0x20 */
				__IO BIT BITER_ELINKNO[16];                     /*!< TCD Beginning Minor Loop Link, Major Loop Count (Channel Linking Disabled), array offset: 0x101E, array step: 0x20 */
				__IO BIT BITER_ELINKYES[16];                    /*!< TCD Beginning Minor Loop Link, Major Loop Count (Channel Linking Enabled), array offset: 0x101E, array step: 0x20 */
			};
		} TCD[16];
	};



	/*! LPTMR - Register Bit Band Layout Typedef */
	class LPTMR_BitBandType
	{
	public:

		// empty
	};

}

#endif  /*_ADX_K60_REG_H_*/
