
#ifndef _ADX_K60_EXTI_H_
#define _ADX_K60_EXTI_H_

#include "adxM3.h"
#include "GPIO.h"
#include "../../System/System.h"

namespace K60
{


	template< UINT32 BASE >
	class Peripheral< EXTI_INFO<BASE> >
	{
	public:

		typedef typename EXTI_INFO<BASE>::GlobalPortInfo	GlobalPortInfo;
		typedef typename EXTI_INFO<BASE>::GPIOInfo			GPIOInfo;
		typedef EXTI_INFO<BASE>								PeripheralInfo;
		typedef Peripheral<GlobalPortInfo>					GlobalPort;
		typedef Peripheral<GPIOInfo>						GPIO;


		template< PinIndex INDEX >
		class PinSource
			: protected GPIO::template ConstPin<INDEX>
			, public adxM3::ExternalInterrupt
		{
		public:


			typedef typename GPIO::template ConstPin<INDEX> BasePin;

			using BasePin::SetInputFilter;
			using BasePin::InputConfig;
			using BasePin::GetLevel;
			using BasePin::GetPinSource;
			using BasePin::GetPortSource;

			using BasePin::Floating;
			using BasePin::PullUp;
			using BasePin::PullDown;

			PinSource()
			{
				PeripheralInfo::InstancePtr[INDEX] = this;

				// 时钟已在基类中开启
				
				// 设为输入方向
				BasePin::SetDirection(GeneralPurposeInputOutput::IN);

				// 上拉下拉由用户配置


				// 配置NVIC
				CM4::$NVIC::SetPriority(
					PeripheralInfo::PeripheralIRQ, 
					PeripheralInfo::InteruptPriority);
				CM4::$NVIC::EnableIRQ(PeripheralInfo::PeripheralIRQ);
			}



			virtual ~PinSource()
			{
				BasePin::~ConstPin();
			}

				



			/*
			 * @brief	事件通知开关控制
			 * @param	evt 需要开启或关闭通知的事件类型
			 * @retval	成功使能/禁止某个事件通知则返回1，否则返回0
			 */
			virtual int EnableEventNotify(EventType evt)
			{
				switch (evt)
				{
				case LEVEL_RISING :
					if (	GlobalPortInfo::BaseAccess()->PCR[INDEX].IRQC == IRQC_FallingEdge 
						||	GlobalPortInfo::BaseAccess()->PCR[INDEX].IRQC == IRQC_BothEdge)
					{
						GlobalPortInfo::BaseAccess()->PCR[INDEX].IRQC = IRQC_BothEdge;
					}
					else
					{
						GlobalPortInfo::BaseAccess()->PCR[INDEX].IRQC = IRQC_RisingEdge;
					}
					return 1;

				case LEVEL_FALLING :
					if (	GlobalPortInfo::BaseAccess()->PCR[INDEX].IRQC == IRQC_RisingEdge
						||	GlobalPortInfo::BaseAccess()->PCR[INDEX].IRQC == IRQC_BothEdge)
					{
						GlobalPortInfo::BaseAccess()->PCR[INDEX].IRQC = IRQC_BothEdge;
					}
					else
					{
						GlobalPortInfo::BaseAccess()->PCR[INDEX].IRQC = IRQC_FallingEdge;
					}
					return 1;

				default:
					return 0;
				}
			}
			virtual int DisableEventNotify(EventType evt)
			{
				switch (evt)
				{
				case LEVEL_RISING :
					if (GlobalPortInfo::BaseAccess()->PCR[INDEX].IRQC == IRQC_BothEdge)
					{
						GlobalPortInfo::BaseAccess()->PCR[INDEX].IRQC = IRQC_FallingEdge;
					}
					else
					{
						GlobalPortInfo::BaseAccess()->PCR[INDEX].IRQC = IRQC_InterruptDisable;
					}
					return 1;

				case LEVEL_FALLING :
					if (GlobalPortInfo::BaseAccess()->PCR[INDEX].IRQC == IRQC_BothEdge)
					{
						GlobalPortInfo::BaseAccess()->PCR[INDEX].IRQC = IRQC_RisingEdge;
					}
					else
					{
						GlobalPortInfo::BaseAccess()->PCR[INDEX].IRQC = IRQC_InterruptDisable;
					}
					return 1;

				default:
					return 0;
				}
			}



			/*
			 * @brief	添加事件监听器，监听事件对象的某个事件
			 * @param	evt 对象的事件
			 *			FnPtr 触发该事件时的处理函数，由派生类在实现时转换到所需的类型
			 * @retval	成功注册某个事件监听器则返回1，否则返回0
			 */
			virtual int AddEventListener(EventType evt, void* FnPtr)
			{
				switch (evt)
				{
				case LEVEL_RISING :
					PeripheralInfo::ExtPinLevelRisingCBK[INDEX] = FnPtr;
					if (	GlobalPortInfo::BaseAccess()->PCR[INDEX].IRQC == IRQC_FallingEdge 
						||	GlobalPortInfo::BaseAccess()->PCR[INDEX].IRQC == IRQC_BothEdge)
					{
						GlobalPortInfo::BaseAccess()->PCR[INDEX].IRQC = IRQC_BothEdge;
					}
					else
					{
						GlobalPortInfo::BaseAccess()->PCR[INDEX].IRQC = IRQC_RisingEdge;
					}
					return 1;

				case LEVEL_FALLING :
					PeripheralInfo::ExtPinLevelFallingCBK[INDEX] = FnPtr;
					if (	GlobalPortInfo::BaseAccess()->PCR[INDEX].IRQC == IRQC_RisingEdge
						||	GlobalPortInfo::BaseAccess()->PCR[INDEX].IRQC == IRQC_BothEdge)
					{
						GlobalPortInfo::BaseAccess()->PCR[INDEX].IRQC = IRQC_BothEdge;
					}
					else
					{
						GlobalPortInfo::BaseAccess()->PCR[INDEX].IRQC = IRQC_FallingEdge;
					}
					return 1;

				default:
					return 0;
				}
			}


		private:

			enum
			{
				IRQC_InterruptDisable	= 0,
				IRQC_RisingEdge			= 9,
				IRQC_FallingEdge		= 10,
				IRQC_BothEdge			= 11
			};


		};

	};
	
}

extern "C"
{
	void PORTA_IRQHandler(void);
	void PORTB_IRQHandler(void);
	void PORTC_IRQHandler(void);
	void PORTD_IRQHandler(void);
	void PORTE_IRQHandler(void);
};

#endif  /*_ADX_K60_EXTI_H_*/
