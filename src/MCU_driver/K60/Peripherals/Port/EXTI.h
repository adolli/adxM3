
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

				// ʱ�����ڻ����п���
				
				// ��Ϊ���뷽��
				BasePin::SetDirection(GeneralPurposeInputOutput::IN);

				// �����������û�����


				// ����NVIC
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
			 * @brief	�¼�֪ͨ���ؿ���
			 * @param	evt ��Ҫ������ر�֪ͨ���¼�����
			 * @retval	�ɹ�ʹ��/��ֹĳ���¼�֪ͨ�򷵻�1�����򷵻�0
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
			 * @brief	����¼��������������¼������ĳ���¼�
			 * @param	evt ������¼�
			 *			FnPtr �������¼�ʱ�Ĵ�����������������ʵ��ʱת�������������
			 * @retval	�ɹ�ע��ĳ���¼��������򷵻�1�����򷵻�0
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
