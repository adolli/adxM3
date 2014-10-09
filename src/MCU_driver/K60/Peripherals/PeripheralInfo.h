
#ifndef _ADX_K60_PERIPHERALINFO_H_
#define _ADX_K60_PERIPHERALINFO_H_

#include "adxM3.h"
#include "../LowLevelSystem/MK60N512MD100.h"
#include "../irqchx.h"
#include "K60_reg.h"
#include "Atomic.h"
#include "Clock/ClockDistributor.h"
#include "Port/SignalMultiplexing.h"


#include "Typelist.h"
#include "EmptyType.h"
using namespace Loki;


namespace K60
{

	

	class MemoryMapping
	{
	public:

		enum
		{
			RAM_BASE					= 0x1FFF0000,
			RAM_SIZE					= 0x00020000,
			ROM_BASE					= 0x00000000,
			ROM_SIZE					= 0x00080000,
			PERIPHERAL_BASE				= 0x40000000,
			PERIPHERAL_SIZE				= 0x000FFFFF,
			PERIPHERAL_BITBAND_BASE		= 0x42000000,
			PERIPHERAL_BITBAND_SIZE		= 0x01FFFFFF,
			CM4_CORE_PERIPHERAL_BASE	= 0xE0000000,
			CM4_CORE_PERIPHERAL_SIZE	= 0x000FFFFF,
		};
	};







	//////////////////////////////////////////////////////////////////////////
	//		Base and basic utilities
	//////////////////////////////////////////////////////////////////////////


	template< typename RegType, typename BitBandType, AddressType BASE >
	class PeripheralBaseAndBitBandAccess
	{
	public:
		enum 
		{ 
			BaseAddress = BASE,
			BitBandBaseAddress = BITBAND_PERIPH_BASE_TO_BITBAND_BASE(BASE) 
		};

		static RegType BaseAccess()
		{
			return (RegType)BASE;
		}
		static BitBandType BitBandAccess()
		{
			return (BitBandType)BitBandBaseAddress;
		}
	};





	/*
	 * @brief	外设信息名字空间0
	 */
	namespace PI0
	{


		//////////////////////////////////////////////////////////////////////////
		//		Chip clock source definition
		//////////////////////////////////////////////////////////////////////////

		class MCGOutputClock
		{
		public:
			enum
			{
				Frequency = 100000000
				//Frequency = 180000000
			};
		};

		class CoreClock
		{
		public:
			enum
			{
				Frequency = 100000000
				//Frequency = 180000000
			};
		};

		class SystemClock
		{
		public:
			enum
			{
				Frequency = 100000000
				//Frequency = 180000000
			};
		};

		class BusClock
		{
		public:
			enum
			{
				Frequency = 50000000
				//Frequency = 90000000
			};
		};

		class FlexBusClock
		{
		public:
			enum
			{
				Frequency = 50000000
				//Frequency = 90000000
			};
		};

		class FlashClock
		{
		public:
			enum
			{
				Frequency = 25000000
				//Frequency = 30000000
			};
		};



		class InternalClockReference
		{
		public:
			enum
			{
				Frequency = 4000000
			};
		};


		class ExternalClockReference
		{
		public:
			enum
			{
				Frequency = 50000000
			};
		};







		//////////////////////////////////////////////////////////////////////////
		//    Direct Memory Access Controller (eDMA)
		//////////////////////////////////////////////////////////////////////////



		/*
		 * @brief	DMA触发源列表
		 */
		enum DMARequestSource
		{
			Disabled				,
			ReservedNotUsed			,
			UART0_Receive			,
			UART0_Transmit			,
			UART1_Receive			,
			UART1_Transmit			,
			UART2_Receive			,
			UART2_Transmit			,
			UART3_Receive			,
			UART3_Transmit			,
			UART4_Receive			,
			UART4_Transmit			,
			UART5_Receive			,
			UART5_Transmit			,
			I2S0_Receive			,
			I2S0_Transmit			,
			SPI0_Receive			,
			SPI0_Transmit			,
			SPI1_Receive			,
			SPI1_Transmit			,
			SPI2_Receive			,
			SPI2_Transmit			,
			I2C0_Transfer			,
			I2C1_Transfer			,
			FTM0_Channel0			,
			FTM0_Channel1			,
			FTM0_Channel2			,
			FTM0_Channel3			,
			FTM0_Channel4			,
			FTM0_Channel5			,
			FTM0_Channel6			,
			FTM0_Channel7			,
			FTM1_Channel0			,
			FTM1_Channel1			,
			FTM2_Channel0			,
			FTM2_Channel1			,
			IEEE1588Timers_Timer0	,
			IEEE1588Timers_Timer1	,
			IEEE1588Timers_Timer2	,
			IEEE1588Timers_Timer3	,
			ADC0_Trigger			,
			ADC1_Trigger			,
			CMP0_Trigger			,
			CMP1_Trigger			,
			CMP2_Trigger			,
			DAC0_Trigger			,
			DAC1_Trigger			,
			CMT_Trigger				,
			PDB_Trigger				,
			PortControlModule_PortA	,
			PortControlModule_PortB	,
			PortControlModule_PortC	,
			PortControlModule_PortD	,
			PortControlModule_PortE	,
			DMAMUX_Always0			,
			DMAMUX_Always1			,
			DMAMUX_Always2			,
			DMAMUX_Always3			,
			DMAMUX_Always4			,
			DMAMUX_Always5			,
			DMAMUX_Always6			,
			DMAMUX_Always7			,
			DMAMUX_Always8			,
			DMAMUX_Always9			
		};





		template< UINT32 peripheral_base > class DMA_INFO;

		template<> class DMA_INFO<DMA_BASE>
			: public PeripheralBaseAndBitBandAccess<DMA_Type*, DMA_BitBandType*, DMA_BASE>
			{
			public:
				typedef GateType<(UINT32)&SIM->SCGC7, 1>	ClockSourceGate;
				typedef GateType<(UINT32)&SIM->SCGC6, 1>	MultiplexerClockSourceGate;
				typedef ClockDistributor<SystemClock>		ModuleClock;

				enum
				{
					CHANNEL_MAX = 16
				};
				static const IRQn_Type TransferCompleteIRQ[CHANNEL_MAX];
				static const IRQn_Type ErrorIRQ = DMA_Error_IRQn;
				enum
				{
					DefaultIRQPriority = 4
				};

				static DMAMUX_Type* MultiplexerAccess()
				{
					return (DMAMUX_Type*)DMAMUX_BASE;
				}



				/*
				 * @brief	通道空闲标记，每一位表示一个通道
				 *			在申请一个DMA通道后，对应位被标记0
				 *			如CH0可以使用，则(ChannelIdleFlagStatus & _BV(0)) == true
				 */
				static volatile UINT16 ChannelIdleFlagStatus;



				/*
				 * @brief	传输完成中断回调
				 *			对应通道传输完成后的回调函数，由用户指定处理内容
				 */
				static adxM3::Function< void() > transferCompleteCBK[CHANNEL_MAX];



				/*
				 * @brief	通道传输完成事件通知屏蔽标志，每一位表示一个通道
				 *			如果对应位为1，则在传输完成时不响应用户事件监听器
				 */
				static volatile UINT16 ChannelTCEventMask;



				/*
				 * @brief	通道传输缓冲区，用于外设与内存之间的传输，把要传输的数据复制到此缓冲区
				 *			因为用户提交完DMA传输申请后，源缓冲区可能被析构（由于函数的返回）
				 *			内存到内存的DMA传输不会用到此缓冲区（没必要多此一举）
				 *			缓冲区在提交传输申请的时候被分配
				 *			每次传输完成后，中断服务中自动会释放所申请的缓冲区
				 */
				static void* TransferSourceBuffer[CHANNEL_MAX];


			};

	


		#undef DMA
		typedef DMA_INFO<DMA_BASE> DMA;





		//////////////////////////////////////////////////////////////////////////
		//    Port Control and Interrupts (PORT)
		//////////////////////////////////////////////////////////////////////////

		template< UINT32 peripheral_base > class PORT_INFO;

		template<> class PORT_INFO<PORTA_BASE>
			: public PeripheralBaseAndBitBandAccess<PORT_Type*, PORT_BitBandType*, PORTA_BASE>
			{
			public:
				typedef GateType<(UINT32)&SIM->SCGC5, 9>	ClockSourceGate;
				typedef ClockDistributor<SystemClock>		ModuleClock;
				enum
				{
					PIN_MAX = 30
				};
			};
	
		template<> class PORT_INFO<PORTB_BASE> 
			: public PeripheralBaseAndBitBandAccess<PORT_Type*, PORT_BitBandType*, PORTB_BASE>
			{
			public:
				typedef GateType<(UINT32)&SIM->SCGC5, 10>	ClockSourceGate;
				typedef ClockDistributor<SystemClock>		ModuleClock;
				enum
				{
					PIN_MAX = 24
				};
			};

		template<> class PORT_INFO<PORTC_BASE>
			: public PeripheralBaseAndBitBandAccess<PORT_Type*, PORT_BitBandType*, PORTC_BASE>
			{
			public:
				typedef GateType<(UINT32)&SIM->SCGC5, 11>	ClockSourceGate;
				typedef ClockDistributor<SystemClock>		ModuleClock;
				enum
				{
					PIN_MAX = 20
				};
			};

		template<> class PORT_INFO<PORTD_BASE>
			: public PeripheralBaseAndBitBandAccess<PORT_Type*, PORT_BitBandType*, PORTD_BASE>
			{
			public:
				typedef GateType<(UINT32)&SIM->SCGC5, 12>	ClockSourceGate;
				typedef ClockDistributor<SystemClock>		ModuleClock;
				enum
				{
					PIN_MAX = 16
				};
			};

		template<> class PORT_INFO<PORTE_BASE>
			: public PeripheralBaseAndBitBandAccess<PORT_Type*, PORT_BitBandType*, PORTE_BASE>
			{
			public:
				typedef GateType<(UINT32)&SIM->SCGC5, 13>	ClockSourceGate;
				typedef ClockDistributor<SystemClock>		ModuleClock;
				enum
				{
					PIN_MAX = 29
				};
			};
	
	

		typedef PORT_INFO<PORTA_BASE> GPTA;
		typedef PORT_INFO<PORTB_BASE> GPTB;
		typedef PORT_INFO<PORTC_BASE> GPTC;
		typedef PORT_INFO<PORTD_BASE> GPTD;
		typedef PORT_INFO<PORTE_BASE> GPTE;
	



		//////////////////////////////////////////////////////////////////////////
		//    Special Analog Port
		//////////////////////////////////////////////////////////////////////////

		enum 
		{
			// be careful!
			// this value should never be collision with others
			ANALOG_PORT = 0x10200030u
		};

		template<> class PORT_INFO<ANALOG_PORT>
			{
			public:
				typedef EmptyGateType<> ClockSourceGate;
			};


		typedef PORT_INFO<ANALOG_PORT> GlobalAnalogPort;




		/*
		 * @brief	模拟信号的引脚
		 *			其值为芯片引脚号（LQFP引脚号）
		 */
		enum AnalogPinName
		{
			ADC1_DP0			= 29,
			ADC1_DM0			= 30,
			ADC1_DP1			= 25,
			ADC1_DM1			= 26,
			ADC1_DP3			= 27,
			ADC1_DM3			= 28,

			PGA0_DP				= 27,
			PGA0_DM				= 28,
			PGA1_DP				= 29,
			PGA1_DM				= 30,

			ADC0_DP0			= 27,
			ADC0_DM0			= 28,
			ADC0_DP1			= 23,
			ADC0_DM1			= 24,
			ADC0_DP3			= 29,
			ADC0_DM3			= 30,

			ADC1_SE16			= 35,
			CMP2_IN2			= 35,
			ADC0_SE22			= 35,

			ADC0_SE16			= 36,
			CMP1_IN2			= 36,
			ADC0_SE21			= 36,

			VREF_OUT			= 37,
			CMP1_IN5			= 37,
			CMP0_IN5			= 37,
			ADC1_SE18			= 37,

			DAC0_OUT			= 38,
			CMP1_IN3			= 38,
			ADC0_SE23			= 38,

			DAC1_OUT			= 39,
			CMP2_IN3			= 39,
			ADC1_SE23			= 39,

		};




		//////////////////////////////////////////////////////////////////////////
		//    General Purpose Input/Output (GPIO)
		//////////////////////////////////////////////////////////////////////////

		template< UINT32 peripheral_base > class GPIO_INFO;

		template<> class GPIO_INFO<PTA_BASE>
			: public PeripheralBaseAndBitBandAccess<GPIO_Type*, GPIO_BitBandType*, PTA_BASE>
			{
			public:
				typedef PORT_INFO<PORTA_BASE>			GlobalPortInfo;
				typedef ClockDistributor<SystemClock>	ModuleClock;
			};

		template<> class GPIO_INFO<PTB_BASE>
			: public PeripheralBaseAndBitBandAccess<GPIO_Type*, GPIO_BitBandType*, PTB_BASE>
			{
			public:
				typedef PORT_INFO<PORTB_BASE>			GlobalPortInfo;
				typedef ClockDistributor<SystemClock>	ModuleClock;
			};

		template<> class GPIO_INFO<PTC_BASE>
			: public PeripheralBaseAndBitBandAccess<GPIO_Type*, GPIO_BitBandType*, PTC_BASE>
			{
			public:
				typedef PORT_INFO<PORTC_BASE>			GlobalPortInfo;
				typedef ClockDistributor<SystemClock>	ModuleClock;
			};

		template<> class GPIO_INFO<PTD_BASE>
			: public PeripheralBaseAndBitBandAccess<GPIO_Type*, GPIO_BitBandType*, PTD_BASE>
			{
			public:
				typedef PORT_INFO<PORTD_BASE>			GlobalPortInfo;
				typedef ClockDistributor<SystemClock>	ModuleClock;
			};

		template<> class GPIO_INFO<PTE_BASE>
			: public PeripheralBaseAndBitBandAccess<GPIO_Type*, GPIO_BitBandType*, PTE_BASE>
			{
			public:
				typedef PORT_INFO<PORTE_BASE>			GlobalPortInfo;
				typedef ClockDistributor<SystemClock>	ModuleClock;
			};



		typedef GPIO_INFO<PTA_BASE> GPIOA;
		typedef GPIO_INFO<PTB_BASE> GPIOB;
		typedef GPIO_INFO<PTC_BASE> GPIOC;
		typedef GPIO_INFO<PTD_BASE> GPIOD;
		typedef GPIO_INFO<PTE_BASE> GPIOE;





		//////////////////////////////////////////////////////////////////////////
		//    General Purpose Input/Output Pin (GPIOP)
		//////////////////////////////////////////////////////////////////////////


		template< UINT32 PORT_BASE, PinIndex PIN_INDEX >
											class GPIO_Pin_INFO
	{
	public:
		typedef GPIO_INFO<PORT_BASE> PortInfo;
		enum 
		{ 
			Pin = PIN_INDEX,
			PIN_MAX = PortInfo::PIN_MAX
		};
	};






		//////////////////////////////////////////////////////////////////////////
		//		External Interrupt
		//////////////////////////////////////////////////////////////////////////

		template< UINT32 peripheral_base > class EXTI_INFO;

		template<> class EXTI_INFO<PTA_BASE>
			: public GPIO_INFO<PTA_BASE>
			{
			public:
				typedef GPIO_INFO<PTA_BASE>	GPIOInfo;

				static const IRQn_Type PeripheralIRQ = PORTA_IRQn;
				enum
				{
					InteruptPriority = 4
				};
				static void* InstancePtr[GlobalPortInfo::PIN_MAX];
				static adxM3::Function< void(void*) > ExtPinLevelRisingCBK[GlobalPortInfo::PIN_MAX];
				static adxM3::Function< void(void*) > ExtPinLevelFallingCBK[GlobalPortInfo::PIN_MAX];
			};

		template<> class EXTI_INFO<PTB_BASE>
			: public GPIO_INFO<PTB_BASE>
			{
			public:
				typedef GPIO_INFO<PTB_BASE>	GPIOInfo;

				static const IRQn_Type PeripheralIRQ = PORTB_IRQn;
				enum
				{
					InteruptPriority = 4
				};
				static void* InstancePtr[GlobalPortInfo::PIN_MAX];
				static adxM3::Function< void(void*) > ExtPinLevelRisingCBK[GlobalPortInfo::PIN_MAX];
				static adxM3::Function< void(void*) > ExtPinLevelFallingCBK[GlobalPortInfo::PIN_MAX];
			};

		template<> class EXTI_INFO<PTC_BASE>
			: public GPIO_INFO<PTC_BASE>
			{
			public:
				typedef GPIO_INFO<PTC_BASE>	GPIOInfo;

				static const IRQn_Type PeripheralIRQ = PORTC_IRQn;
				enum
				{
					InteruptPriority = 4
				};
				static void* InstancePtr[GlobalPortInfo::PIN_MAX];
				static adxM3::Function< void(void*) > ExtPinLevelRisingCBK[GlobalPortInfo::PIN_MAX];
				static adxM3::Function< void(void*) > ExtPinLevelFallingCBK[GlobalPortInfo::PIN_MAX];
			};

		template<> class EXTI_INFO<PTD_BASE>
			: public GPIO_INFO<PTD_BASE>
			{
			public:
				typedef GPIO_INFO<PTD_BASE>	GPIOInfo;

				static const IRQn_Type PeripheralIRQ = PORTD_IRQn;
				enum
				{
					InteruptPriority = 6
				};
				static void* InstancePtr[GlobalPortInfo::PIN_MAX];
				static adxM3::Function< void(void*) > ExtPinLevelRisingCBK[GlobalPortInfo::PIN_MAX];
				static adxM3::Function< void(void*) > ExtPinLevelFallingCBK[GlobalPortInfo::PIN_MAX];
			};

		template<> class EXTI_INFO<PTE_BASE>
			: public GPIO_INFO<PTE_BASE>
			{
			public:
				typedef GPIO_INFO<PTE_BASE>	GPIOInfo;

				static const IRQn_Type PeripheralIRQ = PORTE_IRQn;
				enum
				{
					InteruptPriority = 6
				};
				static void* InstancePtr[GlobalPortInfo::PIN_MAX];
				static adxM3::Function< void(void*) > ExtPinLevelRisingCBK[GlobalPortInfo::PIN_MAX];
				static adxM3::Function< void(void*) > ExtPinLevelFallingCBK[GlobalPortInfo::PIN_MAX];
			};



		typedef EXTI_INFO<PTA_BASE> EXTIA;
		typedef EXTI_INFO<PTB_BASE> EXTIB;
		typedef EXTI_INFO<PTC_BASE> EXTIC;
		typedef EXTI_INFO<PTD_BASE> EXTID;
		typedef EXTI_INFO<PTE_BASE> EXTIE;




		//////////////////////////////////////////////////////////////////////////
		//    Universal Asynchronous Receiver/Transmitter (UART)
		//////////////////////////////////////////////////////////////////////////

		template< UINT32 peripheral_base > class UART_INFO;

		template<> class UART_INFO<UART0_BASE>
			: public PeripheralBaseAndBitBandAccess<UART_Type*, UART_BitBandType*, UART0_BASE>
			{
			public:
				typedef GateType<(UINT32)&SIM->SCGC4, 10>	ClockSourceGate;
				typedef ClockDistributor<SystemClock>		ModuleClock;

				typedef Signal<GPTD, 7, PinMultiplexing::ALT3> TxSignal;
				typedef Signal<GPTD, 6, PinMultiplexing::ALT3> RxSignal;
				typedef Signal<GPTD, 5, PinMultiplexing::ALT3> CTSSignal;
				typedef Signal<GPTD, 4, PinMultiplexing::ALT3> RTSSignal;

				static void* InstancePtr;
				static const IRQn_Type TranceiveIQR = UART0_RX_TX_IRQn;
				static const IRQn_Type ErrorIQR = UART0_ERR_IRQn;
				enum
				{
					TranceiveInteruptPriority = 10
				};

				typedef DMA DMAController;
				static const DMARequestSource DMATransmitRequest = UART0_Transmit;
				static const DMARequestSource DMAReceiveRequest = UART0_Receive;
			};
	 
		template<> class UART_INFO<UART1_BASE>
			: public PeripheralBaseAndBitBandAccess<UART_Type*, UART_BitBandType*, UART1_BASE>
			{
			public:
				typedef GateType<(UINT32)&SIM->SCGC4, 11>	ClockSourceGate;
				typedef ClockDistributor<SystemClock>		ModuleClock;

				typedef Signal<GPTC, 4, PinMultiplexing::ALT3> TxSignal;
				typedef Signal<GPTC, 3, PinMultiplexing::ALT3> RxSignal;
				typedef Signal<GPTC, 2, PinMultiplexing::ALT3> CTSSignal;
				typedef Signal<GPTC, 1, PinMultiplexing::ALT3> RTSSignal;

				static void* InstancePtr;
				static const IRQn_Type TranceiveIQR = UART1_RX_TX_IRQn;
				static const IRQn_Type ErrorIQR = UART1_ERR_IRQn;
				enum
				{
					TranceiveInteruptPriority = 10
				};

				typedef DMA DMAController;
				static const DMARequestSource DMATransmitRequest = UART1_Transmit;
				static const DMARequestSource DMAReceiveRequest = UART1_Receive;
			};

		template<> class UART_INFO<UART2_BASE>
			: public PeripheralBaseAndBitBandAccess<UART_Type*, UART_BitBandType*, UART2_BASE>
			{
			public:
				typedef GateType<(UINT32)&SIM->SCGC4, 12>	ClockSourceGate;
				typedef ClockDistributor<BusClock>			ModuleClock;

				typedef Signal<GPTD, 3, PinMultiplexing::ALT3> TxSignal;
				typedef Signal<GPTD, 2, PinMultiplexing::ALT3> RxSignal;
				typedef Signal<GPTD, 1, PinMultiplexing::ALT3> CTSSignal;
				typedef Signal<GPTD, 0, PinMultiplexing::ALT3> RTSSignal;
			
				static void* InstancePtr;
				static const IRQn_Type TranceiveIQR = UART2_RX_TX_IRQn;
				static const IRQn_Type ErrorIQR = UART2_ERR_IRQn;
				enum
				{
					TranceiveInteruptPriority = 10
				};

				typedef DMA DMAController;
				static const DMARequestSource DMATransmitRequest = UART2_Transmit;
				static const DMARequestSource DMAReceiveRequest = UART2_Receive;
			};

		template<> class UART_INFO<UART3_BASE>
			: public PeripheralBaseAndBitBandAccess<UART_Type*, UART_BitBandType*, UART3_BASE>
			{
			public:
				typedef GateType<(UINT32)&SIM->SCGC4, 13>	ClockSourceGate;
				typedef ClockDistributor<BusClock>			ModuleClock;

				typedef Signal<GPTC, 17, PinMultiplexing::ALT3> TxSignal;
				typedef Signal<GPTC, 16, PinMultiplexing::ALT3> RxSignal;
				typedef Signal<GPTC, 19, PinMultiplexing::ALT3> CTSSignal;
				typedef Signal<GPTC, 18, PinMultiplexing::ALT3> RTSSignal;
			
				static void* InstancePtr;
				static const IRQn_Type TranceiveIQR = UART3_RX_TX_IRQn;
				static const IRQn_Type ErrorIQR = UART3_ERR_IRQn;
				enum
				{
					TranceiveInteruptPriority = 10
				};

				typedef DMA DMAController;
				static const DMARequestSource DMATransmitRequest = UART3_Transmit;
				static const DMARequestSource DMAReceiveRequest = UART3_Receive;
			};

		template<> class UART_INFO<UART4_BASE>
			: public PeripheralBaseAndBitBandAccess<UART_Type*, UART_BitBandType*, UART4_BASE>
			{
			public:
				typedef GateType<(UINT32)&SIM->SCGC1, 10>	ClockSourceGate;
				typedef ClockDistributor<BusClock>			ModuleClock;

				typedef Signal<GPTE, 24, PinMultiplexing::ALT3> TxSignal;
				typedef Signal<GPTE, 25, PinMultiplexing::ALT3> RxSignal;
				typedef Signal<GPTE, 26, PinMultiplexing::ALT3> CTSSignal;
				typedef Signal<GPTE, 27, PinMultiplexing::ALT3> RTSSignal;
			
				static void* InstancePtr;
				static const IRQn_Type TranceiveIQR = UART4_RX_TX_IRQn;
				static const IRQn_Type ErrorIQR = UART4_ERR_IRQn;
				enum
				{
					TranceiveInteruptPriority = 10
				};

				typedef DMA DMAController;
				static const DMARequestSource DMATransmitRequest = UART4_Transmit;
				static const DMARequestSource DMAReceiveRequest = UART4_Receive;
			};

		template<> class UART_INFO<UART5_BASE>
			: public PeripheralBaseAndBitBandAccess<UART_Type*, UART_BitBandType*, UART5_BASE>
			{
			public:
				typedef GateType<(UINT32)&SIM->SCGC1, 11>	ClockSourceGate;
				typedef ClockDistributor<BusClock>			ModuleClock;

				typedef Signal<GPTE, 8, PinMultiplexing::ALT3> TxSignal;
				typedef Signal<GPTE, 9, PinMultiplexing::ALT3> RxSignal;
				typedef Signal<GPTE, 10, PinMultiplexing::ALT3> CTSSignal;
				typedef Signal<GPTE, 11, PinMultiplexing::ALT3> RTSSignal;
			
				static void* InstancePtr;
				static const IRQn_Type TranceiveIQR = UART5_RX_TX_IRQn;
				static const IRQn_Type ErrorIQR = UART5_ERR_IRQn;
				enum
				{
					TranceiveInteruptPriority = 10
				};

				typedef DMA DMAController;
				static const DMARequestSource DMATransmitRequest = UART5_Transmit;
				static const DMARequestSource DMAReceiveRequest = UART5_Receive;
			};




		#undef UART0
		#undef UART1
		#undef UART2
		#undef UART3
		#undef UART4
		#undef UART5

		typedef UART_INFO<UART0_BASE> UART0;
		typedef UART_INFO<UART1_BASE> UART1;
		typedef UART_INFO<UART2_BASE> UART2;
		typedef UART_INFO<UART3_BASE> UART3;
		typedef UART_INFO<UART4_BASE> UART4;
		typedef UART_INFO<UART5_BASE> UART5;




		//////////////////////////////////////////////////////////////////////////
		//    Analog-to-Digital Converter (ADC)
		//////////////////////////////////////////////////////////////////////////


		template< UINT32 peripheral_base > class ADC_INFO;

		template<> class ADC_INFO<ADC0_BASE>
			: public PeripheralBaseAndBitBandAccess<ADC_Type*, ADC_BitBandType*, ADC0_BASE>
			{
			public:
				typedef GateType<(UINT32)&SIM->SCGC6, 27>	ClockSourceGate;
				typedef ClockDistributor<BusClock>			ModuleClock;							  
			
				typedef Signal<GlobalAnalogPort, ADC0_DP0, PinMultiplexing::ALT0> DP0;
				typedef Signal<GlobalAnalogPort, ADC0_DM0, PinMultiplexing::ALT0> DM0;
				typedef Signal<GlobalAnalogPort, ADC0_DP1, PinMultiplexing::ALT0> DP1;
				typedef Signal<GlobalAnalogPort, ADC0_DM1, PinMultiplexing::ALT0> DM1;
				typedef Signal<GlobalAnalogPort, PGA0_DP, PinMultiplexing::ALT0> DP2;
				typedef Signal<GlobalAnalogPort, PGA0_DM, PinMultiplexing::ALT0> DM2;
				typedef Signal<GlobalAnalogPort, ADC0_DP3, PinMultiplexing::ALT0> DP3;
				typedef Signal<GlobalAnalogPort, ADC0_DM3, PinMultiplexing::ALT0> DM3;

				typedef Signal<GPTC, 2, PinMultiplexing::ALT0> SE4b;
				typedef Signal<GPTD, 1, PinMultiplexing::ALT0> SE5b;
				typedef Signal<GPTD, 5, PinMultiplexing::ALT0> SE6b;
				typedef Signal<GPTD, 6, PinMultiplexing::ALT0> SE7b;

				typedef Signal<GPTB, 0, PinMultiplexing::ALT0> SE8;
				typedef Signal<GPTB, 1, PinMultiplexing::ALT0> SE9;
				typedef Signal<GPTA, 7, PinMultiplexing::ALT0> SE10;
				typedef Signal<GPTA, 8, PinMultiplexing::ALT0> SE11;
				typedef Signal<GPTB, 2, PinMultiplexing::ALT0> SE12;
				typedef Signal<GPTB, 3, PinMultiplexing::ALT0> SE13;
				typedef Signal<GPTC, 0, PinMultiplexing::ALT0> SE14;
				typedef Signal<GPTC, 1, PinMultiplexing::ALT0> SE15;
				typedef Signal<GlobalAnalogPort, ADC0_SE16, PinMultiplexing::ALT0> SE16;
				typedef Signal<GPTE, 24, PinMultiplexing::ALT0> SE17;
				typedef Signal<GPTE, 18, PinMultiplexing::ALT0> SE18;

				typedef Signal<GlobalAnalogPort, ADC0_SE21, PinMultiplexing::ALT0> SE21;
				typedef Signal<GlobalAnalogPort, ADC0_SE22, PinMultiplexing::ALT0> SE22;
				typedef Signal<GlobalAnalogPort, ADC0_SE23, PinMultiplexing::ALT0> SE23;

				typedef LOKI_TYPELIST_24														   
				(																				   
					DP0, DP1, DP2, DP3, 
					EmptyType, EmptyType, EmptyType, EmptyType, SE8, SE9, SE10, 
					SE11, SE12, SE13, SE14, SE15, SE16, SE17, 
					SE18, EmptyType, EmptyType, SE21, SE22, SE23		   
				) InputChannels;


				struct DifferentialChannelPair0
				{
					typedef DP0 DP;
					typedef DM0 DM;
				};

				struct DifferentialChannelPair1
				{
					typedef DP1 DP;
					typedef DM1 DM;
				};

				struct DifferentialChannelPair2
				{
					typedef DP2 DP;
					typedef DM2 DM;
				};

				struct DifferentialChannelPair3
				{
					typedef DP3 DP;
					typedef DM3 DM;
				};


				typedef LOKI_TYPELIST_4(
					DifferentialChannelPair0,
					DifferentialChannelPair1,
					DifferentialChannelPair2,
					DifferentialChannelPair3
				) DifferentialChannels;


				enum
				{
					InputChannel_MAX = 24,
					DifferentialChannel_MAX = 4,
				};
			};

		template<> class ADC_INFO<ADC1_BASE>
			: public PeripheralBaseAndBitBandAccess<ADC_Type*, ADC_BitBandType*, ADC1_BASE>
			{
			public:
				typedef GateType<(UINT32)&SIM->SCGC3, 27>	ClockSourceGate;
				typedef ClockDistributor<BusClock>			ModuleClock;

				typedef Signal<GlobalAnalogPort, ADC1_DP0, PinMultiplexing::ALT0> DP0;
				typedef Signal<GlobalAnalogPort, ADC1_DM0, PinMultiplexing::ALT0> DM0;
				typedef Signal<GlobalAnalogPort, ADC1_DP1, PinMultiplexing::ALT0> DP1;
				typedef Signal<GlobalAnalogPort, ADC1_DM1, PinMultiplexing::ALT0> DM1;
				typedef Signal<GlobalAnalogPort, PGA1_DP, PinMultiplexing::ALT0> DP2;
				typedef Signal<GlobalAnalogPort, PGA1_DM, PinMultiplexing::ALT0> DM2;
				typedef Signal<GlobalAnalogPort, ADC1_DP3, PinMultiplexing::ALT0> DP3;
				typedef Signal<GlobalAnalogPort, ADC1_DM3, PinMultiplexing::ALT0> DM3;

				typedef Signal<GPTE, 0, PinMultiplexing::ALT0> SE4a;
				typedef Signal<GPTC, 8, PinMultiplexing::ALT0> SE4b;
				typedef Signal<GPTE, 1, PinMultiplexing::ALT0> SE5a;
				typedef Signal<GPTC, 9, PinMultiplexing::ALT0> SE5b;
				typedef Signal<GPTE, 2, PinMultiplexing::ALT0> SE6a;
				typedef Signal<GPTC, 10, PinMultiplexing::ALT0> SE6b;
				typedef Signal<GPTE, 3, PinMultiplexing::ALT0> SE7a;
				typedef Signal<GPTC, 11, PinMultiplexing::ALT0> SE7b;
				typedef Signal<GPTB, 0, PinMultiplexing::ALT0> SE8;
				typedef Signal<GPTB, 1, PinMultiplexing::ALT0> SE9;
				typedef Signal<GPTB, 4, PinMultiplexing::ALT0> SE10;
				typedef Signal<GPTB, 5, PinMultiplexing::ALT0> SE11;
				typedef Signal<GPTB, 6, PinMultiplexing::ALT0> SE12;
				typedef Signal<GPTB, 7, PinMultiplexing::ALT0> SE13;
				typedef Signal<GPTB, 10, PinMultiplexing::ALT0> SE14;
				typedef Signal<GPTB, 11, PinMultiplexing::ALT0> SE15;
				typedef Signal<GlobalAnalogPort, ADC1_SE16, PinMultiplexing::ALT0> SE16;
				typedef Signal<GPTA, 17, PinMultiplexing::ALT0> SE17;
				typedef Signal<GlobalAnalogPort, ADC1_SE18, PinMultiplexing::ALT0> SE18;
				typedef Signal<GlobalAnalogPort, ADC1_SE23, PinMultiplexing::ALT0> SE23;

				typedef LOKI_TYPELIST_24														   
				(																				   
					DP0, DP1, DP2, DP3, 
					SE4a, SE5a, SE6a, SE7a, SE8, SE9, SE10, 
					SE11, SE12, SE13, SE14, SE15, SE16, SE17, 
					SE18, EmptyType, EmptyType, EmptyType, EmptyType, SE23
				) InputChannels;


				struct DifferentialChannelPair0
				{
					typedef DP0 DP;
					typedef DM0 DM;
				};

				struct DifferentialChannelPair1
				{
					typedef DP1 DP;
					typedef DM1 DM;
				};

				struct DifferentialChannelPair2
				{
					typedef DP2 DP;
					typedef DM2 DM;
				};

				struct DifferentialChannelPair3
				{
					typedef DP3 DP;
					typedef DM3 DM;
				};


				typedef LOKI_TYPELIST_4(
					DifferentialChannelPair0,
					DifferentialChannelPair1,
					DifferentialChannelPair2,
					DifferentialChannelPair3
				) DifferentialChannels;


				enum
				{
					InputChannel_MAX = 24,
					DifferentialChannel_MAX = 4,
				};
			};



		#undef ADC0
		#undef ADC1

		typedef ADC_INFO<ADC0_BASE> ADC0;
		typedef ADC_INFO<ADC1_BASE> ADC1;




		//////////////////////////////////////////////////////////////////////////
		//    Periodic Interrupt Timer (PIT)
		//////////////////////////////////////////////////////////////////////////

		template< UINT32 peripheral_base > class PIT_INFO;

		template<> class PIT_INFO<PIT_BASE>
			: public PeripheralBaseAndBitBandAccess<PIT_Type*, PIT_BitBandType*, PIT_BASE>
			{
			public:
				typedef GateType<(UINT32)&SIM->SCGC6, 23>	ClockSourceGate;
				typedef ClockDistributor<BusClock>			ModuleClock;

				enum
				{
					TimerChannel_MAX = 4
				};
				static void* InstancePtr[TimerChannel_MAX];
				static const UINT32 InteruptPriority[TimerChannel_MAX];

				// 每个定时通道一个IRQ号
				static const IRQn_Type ChannelIQR[TimerChannel_MAX];
			
			};


		#undef PIT
		typedef PIT_INFO<PIT_BASE> PIT;






		//////////////////////////////////////////////////////////////////////////
		//    FlexTimer Module (FTM)
		//////////////////////////////////////////////////////////////////////////

		template< UINT32 peripheral_base > class FTM_INFO;

		template<> class FTM_INFO<FTM0_BASE>
			: public PeripheralBaseAndBitBandAccess<FTM_Type*, FTM_BitBandType*, FTM0_BASE>
			{
			public:
				typedef GateType<(UINT32)&SIM->SCGC6, 24>	ClockSourceGate;
				typedef ClockDistributor<BusClock>			ModuleClock;
			
				typedef Signal<GPTC, 1, PinMultiplexing::ALT4> CH0;
				typedef Signal<GPTC, 2, PinMultiplexing::ALT4> CH1;
				typedef Signal<GPTC, 3, PinMultiplexing::ALT4> CH2;
				typedef Signal<GPTC, 4, PinMultiplexing::ALT4> CH3;
				typedef Signal<GPTD, 4, PinMultiplexing::ALT4> CH4;
				typedef Signal<GPTD, 5, PinMultiplexing::ALT4> CH5;
				typedef Signal<GPTD, 6, PinMultiplexing::ALT4> CH6;
				typedef Signal<GPTD, 7, PinMultiplexing::ALT4> CH7;

				typedef Signal<GPTB, 3, PinMultiplexing::ALT6> FLT0;
				typedef Signal<GPTB, 10, PinMultiplexing::ALT6> FLT1;
				typedef Signal<GPTB, 11, PinMultiplexing::ALT6> FLT2;
				typedef Signal<GPTB, 2, PinMultiplexing::ALT6> FLT3;

				typedef LOKI_TYPELIST_8
				(
					CH0, CH1, CH2, CH3, CH4, CH5, CH6, CH7
				) Channels;
			
				typedef LOKI_TYPELIST_4
				(
					FLT0, FLT1, FLT2, FLT3
				) FaultInputs;

				enum
				{
					Channel_MAX = 8,
					FaultInput_MAX = 4
				};
			
				static void* InstancePtr;
				static const IRQn_Type PeripheralIRQ = FTM0_IRQn;
				enum
				{
					InteruptPriority = 9
				};

			};

		template<> class FTM_INFO<FTM1_BASE>
			: public PeripheralBaseAndBitBandAccess<FTM_Type*, FTM_BitBandType*, FTM1_BASE>
			{
			public:
				typedef GateType<(UINT32)&SIM->SCGC6, 25>	ClockSourceGate;
				typedef ClockDistributor<BusClock>			ModuleClock;
			
				typedef Signal<GPTA, 12, PinMultiplexing::ALT3> CH0;
				typedef Signal<GPTA, 13, PinMultiplexing::ALT3> CH1;

				typedef Signal<GPTB, 4, PinMultiplexing::ALT6> FLT0;

				typedef LOKI_TYPELIST_2(CH0, CH1) Channels;
				typedef LOKI_TYPELIST_1(FLT0) FaultInputs;

				struct QuadratureInputs
				{
					typedef Signal<GPTA, 12, PinMultiplexing::ALT7> PHA;
					typedef Signal<GPTA, 13, PinMultiplexing::ALT7> PHB;
				};


				enum
				{
					Channel_MAX = 2,
					FaultInput_MAX = 1,
				};

			
				static void* InstancePtr;
				static const IRQn_Type PeripheralIRQ = FTM1_IRQn;
				enum
				{
					InteruptPriority = 8
				};

			};

		template<> class FTM_INFO<FTM2_BASE>
			: public PeripheralBaseAndBitBandAccess<FTM_Type*, FTM_BitBandType*, FTM2_BASE>
			{
			public:
				typedef GateType<(UINT32)&SIM->SCGC3, 24>	ClockSourceGate;
				typedef ClockDistributor<BusClock>			ModuleClock;
			
				typedef Signal<GPTA, 10, PinMultiplexing::ALT3> CH0;
				typedef Signal<GPTA, 11, PinMultiplexing::ALT3> CH1;

				typedef Signal<GPTB, 5, PinMultiplexing::ALT3> FLT0;

				typedef LOKI_TYPELIST_2(CH0, CH1) Channels;
				typedef LOKI_TYPELIST_1(FLT0) FaultInputs;

				struct QuadratureInputs
				{
					typedef Signal<GPTA, 10, PinMultiplexing::ALT6> PHA;
					typedef Signal<GPTA, 11, PinMultiplexing::ALT6> PHB;
				};

				enum
				{
					Channel_MAX = 2,
					FaultInput_MAX = 1,
				};

			
				static void* InstancePtr;
				static const IRQn_Type PeripheralIRQ = FTM2_IRQn;
				enum
				{
					InteruptPriority = 8
				};

			};


		#undef FTM0
		#undef FTM1
		#undef FTM2

		typedef FTM_INFO<FTM0_BASE> FTM0;
		typedef FTM_INFO<FTM1_BASE> FTM1;
		typedef FTM_INFO<FTM2_BASE> FTM2;








		//////////////////////////////////////////////////////////////////////////
		//    Low-Power Timer (LPTMR)
		//////////////////////////////////////////////////////////////////////////

		template< UINT32 peripheral_base > class LPTMR_INFO;

		template<> class LPTMR_INFO<LPTMR0_BASE>
			: public PeripheralBaseAndBitBandAccess<LPTMR_Type*, LPTMR_BitBandType*, LPTMR0_BASE>
			{
			public:
				typedef GateType<(UINT32)&SIM->SCGC5, 0>			ClockSourceGate;
				typedef ClockDistributor<ExternalClockReference>	ModuleClock;


				typedef Signal<GPTA, 19, PinMultiplexing::ALT6> PulseInput1;
				typedef Signal<GPTC, 5, PinMultiplexing::ALT4> PulseInput2;
				typedef LOKI_TYPELIST_3(EmptyType, PulseInput1, PulseInput2) PulseInputs;


				static void* InstancePtr;
				static const IRQn_Type PeripheralIRQ = LPTimer_IRQn;
				enum
				{
					InteruptPriority = 11
				};
			};

			#undef LPTMR0
			typedef LPTMR_INFO<LPTMR0_BASE> LPTMR0;







		//////////////////////////////////////////////////////////////////////////
		//    Serial Peripheral Interface (SPI)
		//////////////////////////////////////////////////////////////////////////

		template< UINT32 peripheral_base > class SPI_INFO;

		template<> class SPI_INFO<SPI0_BASE>
			: public PeripheralBaseAndBitBandAccess<SPI_Type*, SPI_BitBandType*, SPI0_BASE>
			{
			public:
				typedef GateType<(UINT32)&SIM->SCGC6, 12>	ClockSourceGate;
				typedef ClockDistributor<BusClock>			ModuleClock;
			
				typedef Signal<GPTA, 14, PinMultiplexing::ALT2> PCS0_SS;
				typedef Signal<GPTA, 15, PinMultiplexing::ALT2> SCK;
				typedef Signal<GPTA, 16, PinMultiplexing::ALT2> SOUT;
				typedef Signal<GPTA, 17, PinMultiplexing::ALT2> SIN;
				typedef Signal<GPTC, 3, PinMultiplexing::ALT2> PCS1;
				typedef Signal<GPTC, 2, PinMultiplexing::ALT2> PCS2;
				typedef Signal<GPTC, 1, PinMultiplexing::ALT2> PCS3;
				typedef Signal<GPTC, 0, PinMultiplexing::ALT2> PCS4;
				typedef Signal<GPTB, 23, PinMultiplexing::ALT3> PCS5;
			

				static void* InstancePtr;
				static const IRQn_Type PeripheralIRQ = SPI0_IRQn;
				enum
				{
					InteruptPriority = 12
				};
			};

		template<> class SPI_INFO<SPI1_BASE>
			: public PeripheralBaseAndBitBandAccess<SPI_Type*, SPI_BitBandType*, SPI1_BASE>
			{
			public:
				typedef GateType<(UINT32)&SIM->SCGC6, 13>	ClockSourceGate;
				typedef ClockDistributor<BusClock>			ModuleClock;
				
				typedef Signal<GPTE, 4, PinMultiplexing::ALT2> PCS0_SS;
				typedef Signal<GPTE, 2, PinMultiplexing::ALT2> SCK;
				typedef Signal<GPTE, 1, PinMultiplexing::ALT2> SOUT;
				typedef Signal<GPTE, 3, PinMultiplexing::ALT2> SIN;
				typedef Signal<GPTE, 0, PinMultiplexing::ALT2> PCS1;
				typedef Signal<GPTE, 5, PinMultiplexing::ALT2> PCS2;
				typedef Signal<GPTE, 6, PinMultiplexing::ALT2> PCS3;
			

				static void* InstancePtr;
				static const IRQn_Type PeripheralIRQ = SPI1_IRQn;
				enum
				{
					InteruptPriority = 11
				};
			};

		template<> class SPI_INFO<SPI2_BASE>
			: public PeripheralBaseAndBitBandAccess<SPI_Type*, SPI_BitBandType*, SPI2_BASE>
			{
			public:
				typedef GateType<(UINT32)&SIM->SCGC3, 12>	ClockSourceGate;
				typedef ClockDistributor<BusClock>			ModuleClock;
			
				typedef Signal<GPTD, 11, PinMultiplexing::ALT2> PCS0_SS;
				typedef Signal<GPTD, 12, PinMultiplexing::ALT2> SCK;
				typedef Signal<GPTD, 13, PinMultiplexing::ALT2> SOUT;
				typedef Signal<GPTD, 14, PinMultiplexing::ALT2> SIN;
				typedef Signal<GPTD, 15, PinMultiplexing::ALT2> PCS1;
			

				static void* InstancePtr;
				static const IRQn_Type PeripheralIRQ = SPI2_IRQn;
				enum
				{
					InteruptPriority = 11
				};
			};


			#undef SPI0
			#undef SPI1
			#undef SPI2

			typedef SPI_INFO<SPI0_BASE> SPI0;
			typedef SPI_INFO<SPI1_BASE> SPI1;
			typedef SPI_INFO<SPI2_BASE> SPI2;


	} /* namespace PI0 */






	/*
	 * @brief	外设信息名字空间1
	 *			for mk60dn512zvll10
	 */
	namespace PI1
	{


		//////////////////////////////////////////////////////////////////////////
		//		Chip clock source definition
		//////////////////////////////////////////////////////////////////////////

		class MCGOutputClock
		{
		public:
			enum
			{
				Frequency = 100000000
				//Frequency = 180000000
			};
		};

		class CoreClock
		{
		public:
			enum
			{
				Frequency = 100000000
				//Frequency = 180000000
			};
		};

		class SystemClock
		{
		public:
			enum
			{
				Frequency = 100000000
				//Frequency = 180000000
			};
		};

		class BusClock
		{
		public:
			enum
			{
				Frequency = 50000000
				//Frequency = 90000000
			};
		};

		class FlexBusClock
		{
		public:
			enum
			{
				Frequency = 50000000
				//Frequency = 90000000
			};
		};

		class FlashClock
		{
		public:
			enum
			{
				Frequency = 25000000
				//Frequency = 30000000
			};
		};



		class InternalClockReference
		{
		public:
			enum
			{
				Frequency = 4000000
			};
		};


		class ExternalClockReference
		{
		public:
			enum
			{
				Frequency = 50000000
			};
		};







		//////////////////////////////////////////////////////////////////////////
		//    Direct Memory Access Controller (eDMA)
		//////////////////////////////////////////////////////////////////////////



		/*
		 * @brief	DMA触发源列表
		 */
		enum DMARequestSource
		{
			Disabled				,
			ReservedNotUsed			,
			UART0_Receive			,
			UART0_Transmit			,
			UART1_Receive			,
			UART1_Transmit			,
			UART2_Receive			,
			UART2_Transmit			,
			UART3_Receive			,
			UART3_Transmit			,
			UART4_Receive			,
			UART4_Transmit			,
			UART5_Receive			,
			UART5_Transmit			,
			I2S0_Receive			,
			I2S0_Transmit			,
			SPI0_Receive			,
			SPI0_Transmit			,
			SPI1_Receive			,
			SPI1_Transmit			,
			SPI2_Receive			,
			SPI2_Transmit			,
			I2C0_Transfer			,
			I2C1_Transfer			,
			FTM0_Channel0			,
			FTM0_Channel1			,
			FTM0_Channel2			,
			FTM0_Channel3			,
			FTM0_Channel4			,
			FTM0_Channel5			,
			FTM0_Channel6			,
			FTM0_Channel7			,
			FTM1_Channel0			,
			FTM1_Channel1			,
			FTM2_Channel0			,
			FTM2_Channel1			,
			IEEE1588Timers_Timer0	,
			IEEE1588Timers_Timer1	,
			IEEE1588Timers_Timer2	,
			IEEE1588Timers_Timer3	,
			ADC0_Trigger			,
			ADC1_Trigger			,
			CMP0_Trigger			,
			CMP1_Trigger			,
			CMP2_Trigger			,
			DAC0_Trigger			,
			DAC1_Trigger			,
			CMT_Trigger				,
			PDB_Trigger				,
			PortControlModule_PortA	,
			PortControlModule_PortB	,
			PortControlModule_PortC	,
			PortControlModule_PortD	,
			PortControlModule_PortE	,
			DMAMUX_Always0			,
			DMAMUX_Always1			,
			DMAMUX_Always2			,
			DMAMUX_Always3			,
			DMAMUX_Always4			,
			DMAMUX_Always5			,
			DMAMUX_Always6			,
			DMAMUX_Always7			,
			DMAMUX_Always8			,
			DMAMUX_Always9			
		};





		template< UINT32 peripheral_base > class DMA_INFO;

		template<> class DMA_INFO<DMA_BASE>
			: public PeripheralBaseAndBitBandAccess<DMA_Type*, DMA_BitBandType*, DMA_BASE>
			{
			public:
				typedef GateType<(UINT32)&SIM->SCGC7, 1>	ClockSourceGate;
				typedef GateType<(UINT32)&SIM->SCGC6, 1>	MultiplexerClockSourceGate;
				typedef ClockDistributor<SystemClock>		ModuleClock;

				enum
				{
					CHANNEL_MAX = 16
				};
				static const IRQn_Type TransferCompleteIRQ[CHANNEL_MAX];
				static const IRQn_Type ErrorIRQ = DMA_Error_IRQn;
				enum
				{
					DefaultIRQPriority = 4
				};

				static DMAMUX_Type* MultiplexerAccess()
				{
					return (DMAMUX_Type*)DMAMUX_BASE;
				}



				/*
				 * @brief	通道空闲标记，每一位表示一个通道
				 *			在申请一个DMA通道后，对应位被标记0
				 *			如CH0可以使用，则(ChannelIdleFlagStatus & _BV(0)) == true
				 */
				static volatile UINT16 ChannelIdleFlagStatus;



				/*
				 * @brief	传输完成中断回调
				 *			对应通道传输完成后的回调函数，由用户指定处理内容
				 */
				static adxM3::Function< void() > transferCompleteCBK[CHANNEL_MAX];



				/*
				 * @brief	通道传输完成事件通知屏蔽标志，每一位表示一个通道
				 *			如果对应位为1，则在传输完成时不响应用户事件监听器
				 */
				static volatile UINT16 ChannelTCEventMask;



				/*
				 * @brief	通道传输缓冲区，用于外设与内存之间的传输，把要传输的数据复制到此缓冲区
				 *			因为用户提交完DMA传输申请后，源缓冲区可能被析构（由于函数的返回）
				 *			内存到内存的DMA传输不会用到此缓冲区（没必要多此一举）
				 *			缓冲区在提交传输申请的时候被分配
				 *			每次传输完成后，中断服务中自动会释放所申请的缓冲区
				 */
				static void* TransferSourceBuffer[CHANNEL_MAX];


			};

	


		#undef DMA
		typedef DMA_INFO<DMA_BASE> DMA;





		//////////////////////////////////////////////////////////////////////////
		//    Port Control and Interrupts (PORT)
		//////////////////////////////////////////////////////////////////////////

		template< UINT32 peripheral_base > class PORT_INFO;

		template<> class PORT_INFO<PORTA_BASE>
			: public PeripheralBaseAndBitBandAccess<PORT_Type*, PORT_BitBandType*, PORTA_BASE>
			{
			public:
				typedef GateType<(UINT32)&SIM->SCGC5, 9>	ClockSourceGate;
				typedef ClockDistributor<SystemClock>		ModuleClock;
				enum
				{
					PIN_MAX = 20
				};
			};
	
		template<> class PORT_INFO<PORTB_BASE> 
			: public PeripheralBaseAndBitBandAccess<PORT_Type*, PORT_BitBandType*, PORTB_BASE>
			{
			public:
				typedef GateType<(UINT32)&SIM->SCGC5, 10>	ClockSourceGate;
				typedef ClockDistributor<SystemClock>		ModuleClock;
				enum
				{
					PIN_MAX = 24
				};
			};

		template<> class PORT_INFO<PORTC_BASE>
			: public PeripheralBaseAndBitBandAccess<PORT_Type*, PORT_BitBandType*, PORTC_BASE>
			{
			public:
				typedef GateType<(UINT32)&SIM->SCGC5, 11>	ClockSourceGate;
				typedef ClockDistributor<SystemClock>		ModuleClock;
				enum
				{
					PIN_MAX = 19
				};
			};

		template<> class PORT_INFO<PORTD_BASE>
			: public PeripheralBaseAndBitBandAccess<PORT_Type*, PORT_BitBandType*, PORTD_BASE>
			{
			public:
				typedef GateType<(UINT32)&SIM->SCGC5, 12>	ClockSourceGate;
				typedef ClockDistributor<SystemClock>		ModuleClock;
				enum
				{
					PIN_MAX = 8
				};
			};

		template<> class PORT_INFO<PORTE_BASE>
			: public PeripheralBaseAndBitBandAccess<PORT_Type*, PORT_BitBandType*, PORTE_BASE>
			{
			public:
				typedef GateType<(UINT32)&SIM->SCGC5, 13>	ClockSourceGate;
				typedef ClockDistributor<SystemClock>		ModuleClock;
				enum
				{
					PIN_MAX = 27
				};
			};
	
	

		typedef PORT_INFO<PORTA_BASE> GPTA;
		typedef PORT_INFO<PORTB_BASE> GPTB;
		typedef PORT_INFO<PORTC_BASE> GPTC;
		typedef PORT_INFO<PORTD_BASE> GPTD;
		typedef PORT_INFO<PORTE_BASE> GPTE;
	



		//////////////////////////////////////////////////////////////////////////
		//    Special Analog Port
		//////////////////////////////////////////////////////////////////////////

		enum 
		{
			// be careful!
			// this value should never be collision with others
			ANALOG_PORT = 0x10200030u
		};

		template<> class PORT_INFO<ANALOG_PORT>
			{
			public:
				typedef EmptyGateType<> ClockSourceGate;
			};


		typedef PORT_INFO<ANALOG_PORT> GlobalAnalogPort;




		/*
		 * @brief	模拟信号的引脚
		 *			其值为芯片引脚号（LQFP引脚号）
		 */
		enum AnalogPinName
		{
			ADC1_DP0			= 20,
			ADC1_DM0			= 21,
			ADC1_DP1			= 16,
			ADC1_DM1			= 17,
			ADC1_DP3			= 18,
			ADC1_DM3			= 19,

			PGA0_DP				= 18,
			PGA0_DM				= 19,
			PGA1_DP				= 20,
			PGA1_DM				= 21,

			ADC0_DP0			= 18,
			ADC0_DM0			= 19,
			ADC0_DP1			= 14,
			ADC0_DM1			= 15,
			ADC0_DP3			= 20,
			ADC0_DM3			= 21,



			VREF_OUT			= 26,
			CMP1_IN5			= 26,
			CMP0_IN5			= 26,
			ADC1_SE18			= 26,

			DAC0_OUT			= 27,
			CMP1_IN3			= 27,
			ADC0_SE23			= 27,

		};




		//////////////////////////////////////////////////////////////////////////
		//    General Purpose Input/Output (GPIO)
		//////////////////////////////////////////////////////////////////////////

		template< UINT32 peripheral_base > class GPIO_INFO;

		template<> class GPIO_INFO<PTA_BASE>
			: public PeripheralBaseAndBitBandAccess<GPIO_Type*, GPIO_BitBandType*, PTA_BASE>
			{
			public:
				typedef PORT_INFO<PORTA_BASE>			GlobalPortInfo;
				typedef ClockDistributor<SystemClock>	ModuleClock;
			};

		template<> class GPIO_INFO<PTB_BASE>
			: public PeripheralBaseAndBitBandAccess<GPIO_Type*, GPIO_BitBandType*, PTB_BASE>
			{
			public:
				typedef PORT_INFO<PORTB_BASE>			GlobalPortInfo;
				typedef ClockDistributor<SystemClock>	ModuleClock;
			};

		template<> class GPIO_INFO<PTC_BASE>
			: public PeripheralBaseAndBitBandAccess<GPIO_Type*, GPIO_BitBandType*, PTC_BASE>
			{
			public:
				typedef PORT_INFO<PORTC_BASE>			GlobalPortInfo;
				typedef ClockDistributor<SystemClock>	ModuleClock;
			};

		template<> class GPIO_INFO<PTD_BASE>
			: public PeripheralBaseAndBitBandAccess<GPIO_Type*, GPIO_BitBandType*, PTD_BASE>
			{
			public:
				typedef PORT_INFO<PORTD_BASE>			GlobalPortInfo;
				typedef ClockDistributor<SystemClock>	ModuleClock;
			};

		template<> class GPIO_INFO<PTE_BASE>
			: public PeripheralBaseAndBitBandAccess<GPIO_Type*, GPIO_BitBandType*, PTE_BASE>
			{
			public:
				typedef PORT_INFO<PORTE_BASE>			GlobalPortInfo;
				typedef ClockDistributor<SystemClock>	ModuleClock;
			};



		typedef GPIO_INFO<PTA_BASE> GPIOA;
		typedef GPIO_INFO<PTB_BASE> GPIOB;
		typedef GPIO_INFO<PTC_BASE> GPIOC;
		typedef GPIO_INFO<PTD_BASE> GPIOD;
		typedef GPIO_INFO<PTE_BASE> GPIOE;





		//////////////////////////////////////////////////////////////////////////
		//    General Purpose Input/Output Pin (GPIOP)
		//////////////////////////////////////////////////////////////////////////


		template< UINT32 PORT_BASE, PinIndex PIN_INDEX >
		class GPIO_Pin_INFO
		{
		public:
			typedef GPIO_INFO<PORT_BASE> PortInfo;
			enum 
			{ 
				Pin = PIN_INDEX,
				PIN_MAX = PortInfo::PIN_MAX
			};
		};






		//////////////////////////////////////////////////////////////////////////
		//		External Interrupt
		//////////////////////////////////////////////////////////////////////////

		template< UINT32 peripheral_base > class EXTI_INFO;

		template<> class EXTI_INFO<PTA_BASE>
			: public GPIO_INFO<PTA_BASE>
			{
			public:
				typedef GPIO_INFO<PTA_BASE>	GPIOInfo;

				static const IRQn_Type PeripheralIRQ = PORTA_IRQn;
				enum
				{
					InteruptPriority = 4
				};
				static void* InstancePtr[GlobalPortInfo::PIN_MAX];
				static adxM3::Function< void(void*) > ExtPinLevelRisingCBK[GlobalPortInfo::PIN_MAX];
				static adxM3::Function< void(void*) > ExtPinLevelFallingCBK[GlobalPortInfo::PIN_MAX];
			};

		template<> class EXTI_INFO<PTB_BASE>
			: public GPIO_INFO<PTB_BASE>
			{
			public:
				typedef GPIO_INFO<PTB_BASE>	GPIOInfo;

				static const IRQn_Type PeripheralIRQ = PORTB_IRQn;
				enum
				{
					InteruptPriority = 4
				};
				static void* InstancePtr[GlobalPortInfo::PIN_MAX];
				static adxM3::Function< void(void*) > ExtPinLevelRisingCBK[GlobalPortInfo::PIN_MAX];
				static adxM3::Function< void(void*) > ExtPinLevelFallingCBK[GlobalPortInfo::PIN_MAX];
			};

		template<> class EXTI_INFO<PTC_BASE>
			: public GPIO_INFO<PTC_BASE>
			{
			public:
				typedef GPIO_INFO<PTC_BASE>	GPIOInfo;

				static const IRQn_Type PeripheralIRQ = PORTC_IRQn;
				enum
				{
					InteruptPriority = 4
				};
				static void* InstancePtr[GlobalPortInfo::PIN_MAX];
				static adxM3::Function< void(void*) > ExtPinLevelRisingCBK[GlobalPortInfo::PIN_MAX];
				static adxM3::Function< void(void*) > ExtPinLevelFallingCBK[GlobalPortInfo::PIN_MAX];
			};

		template<> class EXTI_INFO<PTD_BASE>
			: public GPIO_INFO<PTD_BASE>
			{
			public:
				typedef GPIO_INFO<PTD_BASE>	GPIOInfo;

				static const IRQn_Type PeripheralIRQ = PORTD_IRQn;
				enum
				{
					InteruptPriority = 4
				};
				static void* InstancePtr[GlobalPortInfo::PIN_MAX];
				static adxM3::Function< void(void*) > ExtPinLevelRisingCBK[GlobalPortInfo::PIN_MAX];
				static adxM3::Function< void(void*) > ExtPinLevelFallingCBK[GlobalPortInfo::PIN_MAX];
			};

		template<> class EXTI_INFO<PTE_BASE>
			: public GPIO_INFO<PTE_BASE>
			{
			public:
				typedef GPIO_INFO<PTE_BASE>	GPIOInfo;

				static const IRQn_Type PeripheralIRQ = PORTE_IRQn;
				enum
				{
					InteruptPriority = 4
				};
				static void* InstancePtr[GlobalPortInfo::PIN_MAX];
				static adxM3::Function< void(void*) > ExtPinLevelRisingCBK[GlobalPortInfo::PIN_MAX];
				static adxM3::Function< void(void*) > ExtPinLevelFallingCBK[GlobalPortInfo::PIN_MAX];
			};



		typedef EXTI_INFO<PTA_BASE> EXTIA;
		typedef EXTI_INFO<PTB_BASE> EXTIB;
		typedef EXTI_INFO<PTC_BASE> EXTIC;
		typedef EXTI_INFO<PTD_BASE> EXTID;
		typedef EXTI_INFO<PTE_BASE> EXTIE;




		//////////////////////////////////////////////////////////////////////////
		//    Universal Asynchronous Receiver/Transmitter (UART)
		//////////////////////////////////////////////////////////////////////////

		template< UINT32 peripheral_base > class UART_INFO;

		template<> class UART_INFO<UART0_BASE>
			: public PeripheralBaseAndBitBandAccess<UART_Type*, UART_BitBandType*, UART0_BASE>
			{
			public:
				typedef GateType<(UINT32)&SIM->SCGC4, 10>	ClockSourceGate;
				typedef ClockDistributor<SystemClock>		ModuleClock;

				typedef Signal<GPTA, 2, PinMultiplexing::ALT2> TxSignal;
				typedef Signal<GPTA, 1, PinMultiplexing::ALT2> RxSignal;
				typedef Signal<GPTA, 0, PinMultiplexing::ALT2> CTSSignal;
				typedef Signal<GPTA, 3, PinMultiplexing::ALT2> RTSSignal;

				static void* InstancePtr;
				static const IRQn_Type TranceiveIQR = UART0_RX_TX_IRQn;
				static const IRQn_Type ErrorIQR = UART0_ERR_IRQn;
				enum
				{
					TranceiveInteruptPriority = 10
				};

				typedef DMA DMAController;
				static const DMARequestSource DMATransmitRequest = UART0_Transmit;
				static const DMARequestSource DMAReceiveRequest = UART0_Receive;
			};
	 
		template<> class UART_INFO<UART1_BASE>
			: public PeripheralBaseAndBitBandAccess<UART_Type*, UART_BitBandType*, UART1_BASE>
			{
			public:
				typedef GateType<(UINT32)&SIM->SCGC4, 11>	ClockSourceGate;
				typedef ClockDistributor<SystemClock>		ModuleClock;

				typedef Signal<GPTE, 0, PinMultiplexing::ALT3> TxSignal;
				typedef Signal<GPTE, 1, PinMultiplexing::ALT3> RxSignal;
				typedef Signal<GPTE, 2, PinMultiplexing::ALT3> CTSSignal;
				typedef Signal<GPTE, 3, PinMultiplexing::ALT3> RTSSignal;

				static void* InstancePtr;
				static const IRQn_Type TranceiveIQR = UART1_RX_TX_IRQn;
				static const IRQn_Type ErrorIQR = UART1_ERR_IRQn;
				enum
				{
					TranceiveInteruptPriority = 10
				};

				typedef DMA DMAController;
				static const DMARequestSource DMATransmitRequest = UART1_Transmit;
				static const DMARequestSource DMAReceiveRequest = UART1_Receive;
			};

		template<> class UART_INFO<UART2_BASE>
			: public PeripheralBaseAndBitBandAccess<UART_Type*, UART_BitBandType*, UART2_BASE>
			{
			public:
				typedef GateType<(UINT32)&SIM->SCGC4, 12>	ClockSourceGate;
				typedef ClockDistributor<BusClock>			ModuleClock;

				typedef Signal<GPTD, 3, PinMultiplexing::ALT3> TxSignal;
				typedef Signal<GPTD, 2, PinMultiplexing::ALT3> RxSignal;
				typedef Signal<GPTD, 1, PinMultiplexing::ALT3> CTSSignal;
				typedef Signal<GPTD, 0, PinMultiplexing::ALT3> RTSSignal;
			
				static void* InstancePtr;
				static const IRQn_Type TranceiveIQR = UART2_RX_TX_IRQn;
				static const IRQn_Type ErrorIQR = UART2_ERR_IRQn;
				enum
				{
					TranceiveInteruptPriority = 10
				};

				typedef DMA DMAController;
				static const DMARequestSource DMATransmitRequest = UART2_Transmit;
				static const DMARequestSource DMAReceiveRequest = UART2_Receive;
			};

		template<> class UART_INFO<UART3_BASE>
			: public PeripheralBaseAndBitBandAccess<UART_Type*, UART_BitBandType*, UART3_BASE>
			{
			public:
				typedef GateType<(UINT32)&SIM->SCGC4, 13>	ClockSourceGate;
				typedef ClockDistributor<BusClock>			ModuleClock;

				typedef Signal<GPTC, 17, PinMultiplexing::ALT3> TxSignal;
				typedef Signal<GPTC, 16, PinMultiplexing::ALT3> RxSignal;
				typedef Signal<GPTE, 6, PinMultiplexing::ALT3> CTSSignal;
				typedef Signal<GPTC, 18, PinMultiplexing::ALT3> RTSSignal;
			
				static void* InstancePtr;
				static const IRQn_Type TranceiveIQR = UART3_RX_TX_IRQn;
				static const IRQn_Type ErrorIQR = UART3_ERR_IRQn;
				enum
				{
					TranceiveInteruptPriority = 10
				};

				typedef DMA DMAController;
				static const DMARequestSource DMATransmitRequest = UART3_Transmit;
				static const DMARequestSource DMAReceiveRequest = UART3_Receive;
			};

		template<> class UART_INFO<UART4_BASE>
			: public PeripheralBaseAndBitBandAccess<UART_Type*, UART_BitBandType*, UART4_BASE>
			{
			public:
				typedef GateType<(UINT32)&SIM->SCGC1, 10>	ClockSourceGate;
				typedef ClockDistributor<BusClock>			ModuleClock;

				typedef Signal<GPTE, 24, PinMultiplexing::ALT3> TxSignal;
				typedef Signal<GPTE, 25, PinMultiplexing::ALT3> RxSignal;
				typedef Signal<GPTE, 26, PinMultiplexing::ALT3> CTSSignal;
				typedef Signal<GPTC, 12, PinMultiplexing::ALT3> RTSSignal;
			
				static void* InstancePtr;
				static const IRQn_Type TranceiveIQR = UART4_RX_TX_IRQn;
				static const IRQn_Type ErrorIQR = UART4_ERR_IRQn;
				enum
				{
					TranceiveInteruptPriority = 10
				};

				typedef DMA DMAController;
				static const DMARequestSource DMATransmitRequest = UART4_Transmit;
				static const DMARequestSource DMAReceiveRequest = UART4_Receive;
			};
		
		/* this peripheral UART5 is not available in mk60dn512zvll10 */
		template<> class UART_INFO<UART5_BASE>
			: public PeripheralBaseAndBitBandAccess<UART_Type*, UART_BitBandType*, UART5_BASE>
			{
			public:
				typedef GateType<(UINT32)&SIM->SCGC1, 11>	ClockSourceGate;
				typedef ClockDistributor<BusClock>			ModuleClock;

				typedef Signal<GPTE, 8, PinMultiplexing::ALT3> TxSignal;
				typedef Signal<GPTE, 9, PinMultiplexing::ALT3> RxSignal;
				typedef Signal<GPTE, 10, PinMultiplexing::ALT3> CTSSignal;
				typedef Signal<GPTE, 11, PinMultiplexing::ALT3> RTSSignal;

				static void* InstancePtr;
				static const IRQn_Type TranceiveIQR = UART5_RX_TX_IRQn;
				static const IRQn_Type ErrorIQR = UART5_ERR_IRQn;
				enum
				{
					TranceiveInteruptPriority = 10
				};

				typedef DMA DMAController;
				static const DMARequestSource DMATransmitRequest = UART5_Transmit;
				static const DMARequestSource DMAReceiveRequest = UART5_Receive;
			};




		#undef UART0
		#undef UART1
		#undef UART2
		#undef UART3
		#undef UART4
		#undef UART5

		typedef UART_INFO<UART0_BASE> UART0;
		typedef UART_INFO<UART1_BASE> UART1;
		typedef UART_INFO<UART2_BASE> UART2;
		typedef UART_INFO<UART3_BASE> UART3;
		typedef UART_INFO<UART4_BASE> UART4;

		// 为了使K60驱动部分完整，加入以下哑的UART5
		// 使用时请不要使用MCU中不存在的外设，否则造成难以预料的后果
		typedef UART_INFO<UART5_BASE> UART5;



		//////////////////////////////////////////////////////////////////////////
		//    Analog-to-Digital Converter (ADC)
		//////////////////////////////////////////////////////////////////////////


		template< UINT32 peripheral_base > class ADC_INFO;

		template<> class ADC_INFO<ADC0_BASE>
			: public PeripheralBaseAndBitBandAccess<ADC_Type*, ADC_BitBandType*, ADC0_BASE>
			{
			public:
				typedef GateType<(UINT32)&SIM->SCGC6, 27>	ClockSourceGate;
				typedef ClockDistributor<BusClock>			ModuleClock;							  
			
				typedef Signal<GlobalAnalogPort, ADC0_DP0, PinMultiplexing::ALT0> DP0;
				typedef Signal<GlobalAnalogPort, ADC0_DM0, PinMultiplexing::ALT0> DM0;
				typedef Signal<GlobalAnalogPort, ADC0_DP1, PinMultiplexing::ALT0> DP1;
				typedef Signal<GlobalAnalogPort, ADC0_DM1, PinMultiplexing::ALT0> DM1;
				typedef Signal<GlobalAnalogPort, PGA0_DP, PinMultiplexing::ALT0> DP2;
				typedef Signal<GlobalAnalogPort, PGA0_DM, PinMultiplexing::ALT0> DM2;
				typedef Signal<GlobalAnalogPort, ADC0_DP3, PinMultiplexing::ALT0> DP3;
				typedef Signal<GlobalAnalogPort, ADC0_DM3, PinMultiplexing::ALT0> DM3;

				typedef Signal<GPTC, 2, PinMultiplexing::ALT0> SE4b;
				typedef Signal<GPTD, 1, PinMultiplexing::ALT0> SE5b;
				typedef Signal<GPTD, 5, PinMultiplexing::ALT0> SE6b;
				typedef Signal<GPTD, 6, PinMultiplexing::ALT0> SE7b;

				typedef Signal<GPTB, 0, PinMultiplexing::ALT0> SE8;
				typedef Signal<GPTB, 1, PinMultiplexing::ALT0> SE9;
				typedef Signal<GPTB, 2, PinMultiplexing::ALT0> SE12;
				typedef Signal<GPTB, 3, PinMultiplexing::ALT0> SE13;
				typedef Signal<GPTC, 0, PinMultiplexing::ALT0> SE14;
				typedef Signal<GPTC, 1, PinMultiplexing::ALT0> SE15;
				typedef Signal<GPTE, 24, PinMultiplexing::ALT0> SE17;
				typedef Signal<GPTE, 25, PinMultiplexing::ALT0> SE18;

				typedef Signal<GlobalAnalogPort, ADC0_SE23, PinMultiplexing::ALT0> SE23;

				typedef LOKI_TYPELIST_24														   
				(																				   
					DP0, DP1, DP2, DP3, 
					EmptyType, EmptyType, EmptyType, EmptyType, SE8, SE9, EmptyType, 
					EmptyType, SE12, SE13, SE14, SE15, EmptyType, SE17, 
					SE18, EmptyType, EmptyType, EmptyType, EmptyType, SE23		   
				) InputChannels;


				struct DifferentialChannelPair0
				{
					typedef DP0 DP;
					typedef DM0 DM;
				};

				struct DifferentialChannelPair1
				{
					typedef DP1 DP;
					typedef DM1 DM;
				};

				struct DifferentialChannelPair2
				{
					typedef DP2 DP;
					typedef DM2 DM;
				};

				struct DifferentialChannelPair3
				{
					typedef DP3 DP;
					typedef DM3 DM;
				};


				typedef LOKI_TYPELIST_4(
					DifferentialChannelPair0,
					DifferentialChannelPair1,
					DifferentialChannelPair2,
					DifferentialChannelPair3
				) DifferentialChannels;


				enum
				{
					InputChannel_MAX = 24,
					DifferentialChannel_MAX = 4,
				};
			};

		template<> class ADC_INFO<ADC1_BASE>
			: public PeripheralBaseAndBitBandAccess<ADC_Type*, ADC_BitBandType*, ADC1_BASE>
			{
			public:
				typedef GateType<(UINT32)&SIM->SCGC3, 27>	ClockSourceGate;
				typedef ClockDistributor<BusClock>			ModuleClock;

				typedef Signal<GlobalAnalogPort, ADC1_DP0, PinMultiplexing::ALT0> DP0;
				typedef Signal<GlobalAnalogPort, ADC1_DM0, PinMultiplexing::ALT0> DM0;
				typedef Signal<GlobalAnalogPort, ADC1_DP1, PinMultiplexing::ALT0> DP1;
				typedef Signal<GlobalAnalogPort, ADC1_DM1, PinMultiplexing::ALT0> DM1;
				typedef Signal<GlobalAnalogPort, PGA1_DP, PinMultiplexing::ALT0> DP2;
				typedef Signal<GlobalAnalogPort, PGA1_DM, PinMultiplexing::ALT0> DM2;
				typedef Signal<GlobalAnalogPort, ADC1_DP3, PinMultiplexing::ALT0> DP3;
				typedef Signal<GlobalAnalogPort, ADC1_DM3, PinMultiplexing::ALT0> DM3;

				typedef Signal<GPTE, 0, PinMultiplexing::ALT0> SE4a;
				typedef Signal<GPTE, 1, PinMultiplexing::ALT0> SE5a;
				typedef Signal<GPTE, 2, PinMultiplexing::ALT0> SE6a;
				typedef Signal<GPTE, 3, PinMultiplexing::ALT0> SE7a;
				
				typedef Signal<GPTC, 8, PinMultiplexing::ALT0> SE4b;
				typedef Signal<GPTC, 9, PinMultiplexing::ALT0> SE5b;
				typedef Signal<GPTC, 10, PinMultiplexing::ALT0> SE6b;
				typedef Signal<GPTC, 11, PinMultiplexing::ALT0> SE7b;
				
				typedef Signal<GPTB, 0, PinMultiplexing::ALT0> SE8;
				typedef Signal<GPTB, 1, PinMultiplexing::ALT0> SE9;
				typedef Signal<GPTB, 10, PinMultiplexing::ALT0> SE14;
				typedef Signal<GPTB, 11, PinMultiplexing::ALT0> SE15;
				typedef Signal<GPTA, 17, PinMultiplexing::ALT0> SE17;
				typedef Signal<GlobalAnalogPort, ADC1_SE18, PinMultiplexing::ALT0> SE18;

				typedef LOKI_TYPELIST_24														   
				(																				   
					DP0, DP1, DP2, DP3, 
					SE4a, SE5a, SE6a, SE7a, SE8, SE9, EmptyType, 
					EmptyType, EmptyType, EmptyType, SE14, SE15, EmptyType, SE17, 
					SE18, EmptyType, EmptyType, EmptyType, EmptyType, EmptyType
				) InputChannels;


				struct DifferentialChannelPair0
				{
					typedef DP0 DP;
					typedef DM0 DM;
				};

				struct DifferentialChannelPair1
				{
					typedef DP1 DP;
					typedef DM1 DM;
				};

				struct DifferentialChannelPair2
				{
					typedef DP2 DP;
					typedef DM2 DM;
				};

				struct DifferentialChannelPair3
				{
					typedef DP3 DP;
					typedef DM3 DM;
				};


				typedef LOKI_TYPELIST_4(
					DifferentialChannelPair0,
					DifferentialChannelPair1,
					DifferentialChannelPair2,
					DifferentialChannelPair3
				) DifferentialChannels;


				enum
				{
					InputChannel_MAX = 24,
					DifferentialChannel_MAX = 4,
				};
			};



		#undef ADC0
		#undef ADC1

		typedef ADC_INFO<ADC0_BASE> _ADC0_;
		typedef ADC_INFO<ADC1_BASE> _ADC1_;




		//////////////////////////////////////////////////////////////////////////
		//    Periodic Interrupt Timer (PIT)
		//////////////////////////////////////////////////////////////////////////

		template< UINT32 peripheral_base > class PIT_INFO;

		template<> class PIT_INFO<PIT_BASE>
			: public PeripheralBaseAndBitBandAccess<PIT_Type*, PIT_BitBandType*, PIT_BASE>
			{
			public:
				typedef GateType<(UINT32)&SIM->SCGC6, 23>	ClockSourceGate;
				typedef ClockDistributor<BusClock>			ModuleClock;

				enum
				{
					TimerChannel_MAX = 4
				};
				static void* InstancePtr[TimerChannel_MAX];
				static const UINT32 InteruptPriority[TimerChannel_MAX];

				// 每个定时通道一个IRQ号
				static const IRQn_Type ChannelIQR[TimerChannel_MAX];
			
			};


		#undef PIT
		typedef PIT_INFO<PIT_BASE> PIT;






		//////////////////////////////////////////////////////////////////////////
		//    FlexTimer Module (FTM)
		//////////////////////////////////////////////////////////////////////////

		template< UINT32 peripheral_base > class FTM_INFO;

		template<> class FTM_INFO<FTM0_BASE>
			: public PeripheralBaseAndBitBandAccess<FTM_Type*, FTM_BitBandType*, FTM0_BASE>
			{
			public:
				typedef GateType<(UINT32)&SIM->SCGC6, 24>	ClockSourceGate;
				typedef ClockDistributor<BusClock>			ModuleClock;
			
				typedef Signal<GPTC, 1, PinMultiplexing::ALT4> CH0;
				typedef Signal<GPTC, 2, PinMultiplexing::ALT4> CH1;
				typedef Signal<GPTC, 3, PinMultiplexing::ALT4> CH2;
				typedef Signal<GPTC, 4, PinMultiplexing::ALT4> CH3;
				typedef Signal<GPTD, 4, PinMultiplexing::ALT4> CH4;
				typedef Signal<GPTD, 5, PinMultiplexing::ALT4> CH5;
				typedef Signal<GPTD, 6, PinMultiplexing::ALT4> CH6;
				typedef Signal<GPTD, 7, PinMultiplexing::ALT4> CH7;

				typedef Signal<GPTB, 3, PinMultiplexing::ALT6> FLT0;
				typedef Signal<GPTB, 10, PinMultiplexing::ALT6> FLT1;
				typedef Signal<GPTB, 11, PinMultiplexing::ALT6> FLT2;
				typedef Signal<GPTB, 2, PinMultiplexing::ALT6> FLT3;

				typedef LOKI_TYPELIST_8
				(
					CH0, CH1, CH2, CH3, CH4, CH5, CH6, CH7
				) Channels;
			
				typedef LOKI_TYPELIST_4
				(
					FLT0, FLT1, FLT2, FLT3
				) FaultInputs;

				enum
				{
					Channel_MAX = 8,
					FaultInput_MAX = 4
				};
			
				static void* InstancePtr;
				static const IRQn_Type PeripheralIRQ = FTM0_IRQn;
				enum
				{
					InteruptPriority = 9
				};

			};

		template<> class FTM_INFO<FTM1_BASE>
			: public PeripheralBaseAndBitBandAccess<FTM_Type*, FTM_BitBandType*, FTM1_BASE>
			{
			public:
				typedef GateType<(UINT32)&SIM->SCGC6, 25>	ClockSourceGate;
				typedef ClockDistributor<BusClock>			ModuleClock;
			
				typedef Signal<GPTA, 12, PinMultiplexing::ALT3> CH0;
				typedef Signal<GPTA, 13, PinMultiplexing::ALT3> CH1;

				typedef Signal<GPTA, 19, PinMultiplexing::ALT6> FLT0;

				typedef LOKI_TYPELIST_2(CH0, CH1) Channels;
				typedef LOKI_TYPELIST_1(FLT0) FaultInputs;

				struct QuadratureInputs
				{
					typedef Signal<GPTA, 12, PinMultiplexing::ALT7> PHA;
					typedef Signal<GPTA, 13, PinMultiplexing::ALT7> PHB;
				};


				enum
				{
					Channel_MAX = 2,
					FaultInput_MAX = 1,
				};

			
				static void* InstancePtr;
				static const IRQn_Type PeripheralIRQ = FTM1_IRQn;
				enum
				{
					InteruptPriority = 8
				};

			};

		template<> class FTM_INFO<FTM2_BASE>
			: public PeripheralBaseAndBitBandAccess<FTM_Type*, FTM_BitBandType*, FTM2_BASE>
			{
			public:
				typedef GateType<(UINT32)&SIM->SCGC3, 24>	ClockSourceGate;
				typedef ClockDistributor<BusClock>			ModuleClock;
			
				typedef Signal<GPTB, 18, PinMultiplexing::ALT3> CH0;
				typedef Signal<GPTB, 19, PinMultiplexing::ALT3> CH1;

				typedef Signal<GPTC, 9, PinMultiplexing::ALT6> FLT0;

				typedef LOKI_TYPELIST_2(CH0, CH1) Channels;
				typedef LOKI_TYPELIST_1(FLT0) FaultInputs;

				struct QuadratureInputs
				{
					typedef Signal<GPTB, 18, PinMultiplexing::ALT6> PHA;
					typedef Signal<GPTB, 19, PinMultiplexing::ALT6> PHB;
				};

				enum
				{
					Channel_MAX = 2,
					FaultInput_MAX = 1,
				};

			
				static void* InstancePtr;
				static const IRQn_Type PeripheralIRQ = FTM2_IRQn;
				enum
				{
					InteruptPriority = 8
				};

			};


		#undef FTM0
		#undef FTM1
		#undef FTM2

		typedef FTM_INFO<FTM0_BASE> FTM0;
		typedef FTM_INFO<FTM1_BASE> FTM1;
		typedef FTM_INFO<FTM2_BASE> FTM2;








		//////////////////////////////////////////////////////////////////////////
		//    Low-Power Timer (LPTMR)
		//////////////////////////////////////////////////////////////////////////

		template< UINT32 peripheral_base > class LPTMR_INFO;

		template<> class LPTMR_INFO<LPTMR0_BASE>
			: public PeripheralBaseAndBitBandAccess<LPTMR_Type*, LPTMR_BitBandType*, LPTMR0_BASE>
			{
			public:
				typedef GateType<(UINT32)&SIM->SCGC5, 0>			ClockSourceGate;
				typedef ClockDistributor<ExternalClockReference>	ModuleClock;


				typedef Signal<GPTA, 19, PinMultiplexing::ALT6> PulseInput1;
				typedef Signal<GPTC, 5, PinMultiplexing::ALT3> PulseInput2;
				typedef LOKI_TYPELIST_3(EmptyType, PulseInput1, PulseInput2) PulseInputs;


				static void* InstancePtr;
				static const IRQn_Type PeripheralIRQ = LPTimer_IRQn;
				enum
				{
					InteruptPriority = 11
				};
			};

			#undef LPTMR0
			typedef LPTMR_INFO<LPTMR0_BASE> LPTMR0;







		//////////////////////////////////////////////////////////////////////////
		//    Serial Peripheral Interface (SPI)
		//////////////////////////////////////////////////////////////////////////

		template< UINT32 peripheral_base > class SPI_INFO;

		template<> class SPI_INFO<SPI0_BASE>
			: public PeripheralBaseAndBitBandAccess<SPI_Type*, SPI_BitBandType*, SPI0_BASE>
			{
			public:
				typedef GateType<(UINT32)&SIM->SCGC6, 12>	ClockSourceGate;
				typedef ClockDistributor<BusClock>			ModuleClock;
			
				typedef Signal<GPTD, 0, PinMultiplexing::ALT2> PCS0_SS;
				typedef Signal<GPTD, 1, PinMultiplexing::ALT2> SCK;
				typedef Signal<GPTD, 2, PinMultiplexing::ALT2> SOUT;
				typedef Signal<GPTD, 3, PinMultiplexing::ALT2> SIN;
				typedef Signal<GPTD, 4, PinMultiplexing::ALT2> PCS1;
				typedef Signal<GPTD, 5, PinMultiplexing::ALT2> PCS2;
				typedef Signal<GPTD, 6, PinMultiplexing::ALT2> PCS3;
				typedef Signal<GPTC, 0, PinMultiplexing::ALT2> PCS4;
				typedef Signal<GPTB, 23, PinMultiplexing::ALT3> PCS5;
			

				static void* InstancePtr;
				static const IRQn_Type PeripheralIRQ = SPI0_IRQn;
				enum
				{
					InteruptPriority = 12
				};
			};

		template<> class SPI_INFO<SPI1_BASE>
			: public PeripheralBaseAndBitBandAccess<SPI_Type*, SPI_BitBandType*, SPI1_BASE>
			{
			public:
				typedef GateType<(UINT32)&SIM->SCGC6, 13>	ClockSourceGate;
				typedef ClockDistributor<BusClock>			ModuleClock;
				
				typedef Signal<GPTE, 4, PinMultiplexing::ALT2> PCS0_SS;
				typedef Signal<GPTE, 2, PinMultiplexing::ALT2> SCK;
				typedef Signal<GPTE, 1, PinMultiplexing::ALT2> SOUT;
				typedef Signal<GPTE, 3, PinMultiplexing::ALT2> SIN;
				typedef Signal<GPTE, 0, PinMultiplexing::ALT2> PCS1;
				typedef Signal<GPTE, 5, PinMultiplexing::ALT2> PCS2;
				typedef Signal<GPTE, 6, PinMultiplexing::ALT2> PCS3;
			

				static void* InstancePtr;
				static const IRQn_Type PeripheralIRQ = SPI1_IRQn;
				enum
				{
					InteruptPriority = 11
				};
			};

		template<> class SPI_INFO<SPI2_BASE>
			: public PeripheralBaseAndBitBandAccess<SPI_Type*, SPI_BitBandType*, SPI2_BASE>
			{
			public:
				typedef GateType<(UINT32)&SIM->SCGC3, 12>	ClockSourceGate;
				typedef ClockDistributor<BusClock>			ModuleClock;
			
				typedef Signal<GPTB, 20, PinMultiplexing::ALT2> PCS0_SS;
				typedef Signal<GPTB, 21, PinMultiplexing::ALT2> SCK;
				typedef Signal<GPTB, 22, PinMultiplexing::ALT2> SOUT;
				typedef Signal<GPTB, 23, PinMultiplexing::ALT2> SIN;
			

				static void* InstancePtr;
				static const IRQn_Type PeripheralIRQ = SPI2_IRQn;
				enum
				{
					InteruptPriority = 11
				};
			};


			#undef SPI0
			#undef SPI1
			#undef SPI2

			typedef SPI_INFO<SPI0_BASE> SPI0;
			typedef SPI_INFO<SPI1_BASE> SPI1;
			typedef SPI_INFO<SPI2_BASE> SPI2;


	} /* namespace PI1 */




	// 外设信息描述表选择
	using namespace PI1;



} /* namespace K60 */

#endif  /*_ADX_K60_PERIPHERALINFO_H_*/
