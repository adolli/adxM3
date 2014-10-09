
#ifndef _ADX_K60_LPTMR_H_
#define _ADX_K60_LPTMR_H_


#include "adxM3.h"
#include "../../System/System.h"
#include "../PeripheralBase.h"
#include "../PeripheralInfo.h"
#include "../Port/GlobalPort.h"
#include "../Port/SignalPin.h"


namespace K60
{

	/*
	 * @brief	Low Power Timer
	 *			
	 */
	template< UINT32 BASE >
	class Peripheral< LPTMR_INFO<BASE> >
		: public adxM3::Timer
	{

		typedef LPTMR_INFO<BASE> PeripheralInfo;
		typedef ClockGateController<typename PeripheralInfo::ClockSourceGate> ClockSource;

	public:




		Peripheral()
		{
			ClockSource::ClockOn();

			PeripheralInfo::InstancePtr = this;
			

			// ѡ���ⲿ�ο�ʱ�ӣ�������
			OSC->CR |= OSC_CR_ERCLKEN_MASK;
			PeripheralInfo::BaseAccess()->PSR.PCS = 3;

			// ��ʱ����Զ���λ������
			PeripheralInfo::BaseAccess()->CSR.TFC = RESET;

			// ��ʱ��ģʽ
			PeripheralInfo::BaseAccess()->CSR.TMS = RESET;


			// NVIC configuration
			CM4::$NVIC::SetPriority(PeripheralInfo::PeripheralIRQ,
				PeripheralInfo::InteruptPriority);
			CM4::$NVIC::EnableIRQ(PeripheralInfo::PeripheralIRQ);
			
		}



		virtual ~Peripheral()
		{
			ClockSource::ClockOff();
		}






		template< UINT32 InputSourceIndex >
		class PulseCounter
		{

			typedef typename PeripheralInfo::PulseInputs PulseInputs;
			typedef typename TL::TypeAt<PulseInputs, InputSourceIndex>::Result PulseInputSignal;

			SignalPin<PulseInputSignal> PulseInputPin;


		public:



			/*
			 * @type	LPTMR�ļ�����ֻ�ܼӼ���
			 */ 
			typedef UINT16 CounterValueType;



			PulseCounter()
			{
				ClockSource::ClockOn();

				// �Ƚ�ֹ��������
				PeripheralInfo::BaseAccess()->CSR.TEN = RESET;

				// ������ģʽ
				PeripheralInfo::BaseAccess()->CSR.TMS = SET;

				// free running counter
				PeripheralInfo::BaseAccess()->CSR.TFC = SET;

				// Ԥ��Ƶ����·����ʹ�÷�Ƶ
				PeripheralInfo::BaseAccess()->PSR.PBYP = SET;

				// ѡ������
				PeripheralInfo::BaseAccess()->CSR.TPS = InputSourceIndex;

				SetCountEdge(RisingEdge);
					
				// ʹ�ܼ�����
				PeripheralInfo::BaseAccess()->CSR.TEN = SET;
			}


			~PulseCounter()
			{
				ClockSource::ClockOff();
			}




			/*
			 * @brief	��ȡ�������ֵ
			 */
			UINT16 GetCounter()
			{
				return PeripheralInfo::BaseAccess()->CNR;
			}



			/*
			 * @brief	��λ������
			 */
			void ResetCounter()
			{
				PeripheralInfo::BaseAccess()->CSR.TEN = RESET;
				PeripheralInfo::BaseAccess()->CSR.TEN = SET;
			}




			/*
			 * @brief	���ü�����Եѡ��
			 */
			enum CEDGE
			{
				RisingEdge,
				FallingEdge
			};



			/*
			 * @brief	���ü�����Ե
			 */
			void SetCountEdge(const CEDGE edge)
			{
				PeripheralInfo::BaseAccess()->CSR.TPP = edge;
			}



		};






		/*
		 * @brief	������ʱ
		 */
		virtual void Start()
		{
			PeripheralInfo::BaseAccess()->CSR.TEN = SET;
		}




		/*
		 * @brief	ֹͣ��ʱ����ʱ�����㣩
		 */
		virtual void Stop()
		{
			PeripheralInfo::BaseAccess()->CSR.TEN = RESET;
		}






		enum { PulsePerUS = PeripheralInfo::ModuleClock::Frequency / 1000000 };



		/*
		 * @brief	��ȡ��ǰ��ʱ����ʱʱ��ֵ
		 * @retval	��ǰ��ʱֵ����λus
		 */
		virtual UINT32 GetTime() const
		{
			UINT16 cnr = PeripheralInfo::BaseAccess()->CNR;
			UINT16 psc = PeripheralInfo::BaseAccess()->PSR.PRESCALE;
			return cnr * _BV(psc + 1) / PulsePerUS;
		}



		/*
		 * @brief	���ü�ʱ����ʱʱ��
		 * @param	time ʱ������λus
		 */
		virtual void SetTimeOut(UINT32 time)
		{
			UINT16 psc = 1;
			UINT32 cmr = time * PulsePerUS / _BV(psc);
			while (cmr > 0xFFFF && psc <= 0x10)
			{
				++psc;
				cmr = time * PulsePerUS / _BV(psc);
			}
			PeripheralInfo::BaseAccess()->CMR = cmr;
			PeripheralInfo::BaseAccess()->PSR.PRESCALE = psc - 1;
		}



		/*
		 * @brief	���ö�ʱ����ʱƵ�ʣ�
		 * @notice	����ͬSetTimeOut�������õ���TimeOut�ĵ�������Ƶ��
		 */
		virtual void SetFrequency(UINT32 freq)
		{
			UINT16 psc = 1;
			UINT32 cmr = PeripheralInfo::ModuleClock::Frequency / freq / _BV(psc);
			while (cmr > 0xFFFF && psc <= 0x10)
			{
				++psc;
				cmr = PeripheralInfo::ModuleClock::Frequency / freq / _BV(psc);
			}
			PeripheralInfo::BaseAccess()->CMR = cmr;
			PeripheralInfo::BaseAccess()->PSR.PRESCALE = psc - 1;
		}



		/*
		 * @brief	��ȡ��ʱ����ʱʱ��
		 */
		virtual UINT32 GetTimeOut() const
		{
			UINT16 cmr = PeripheralInfo::BaseAccess()->CMR;
			UINT16 psc = PeripheralInfo::BaseAccess()->PSR.PRESCALE;
			return cmr * _BV(psc + 1) / PulsePerUS;
		}



		/*
		 * @brief	��ȡ��ʱ����ʱƵ��
		 */
		virtual UINT32 GetFrequency() const
		{
			UINT16 cmr = PeripheralInfo::BaseAccess()->CMR;
			UINT16 psc = PeripheralInfo::BaseAccess()->PSR.PRESCALE;
			return PulsePerUS / _BV(psc + 1) / cmr;
		}




		/*
		 * @brief	�ж϶�ʱ���Ƿ����ڼ�ʱ
		 * @retval	���ڼ�ʱ����true�����򷵻�false
		 */
		virtual bool TimerIsRunning() const
		{
			return PeripheralInfo::BaseAccess()->CSR.TEN;
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
			case TIME_OUT :
				PeripheralInfo::BaseAccess()->CSR.TIE = SET;
				return 1;

			default:
				return 0;
			}
		}
		virtual int DisableEventNotify(EventType evt)
		{
			switch (evt)
			{
			case TIME_OUT :
				PeripheralInfo::BaseAccess()->CSR.TIE = RESET;
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
			case TIME_OUT :
				timeOutISR = FnPtr;
				PeripheralInfo::BaseAccess()->CSR.TIE = SET;
				return 1;

			default:
				return 0;
			}
		}


		
		// ��ʱ����ص�����
		adxM3::Function< void(void*) > timeOutISR;



	private:


		// ���·������ڴ�������ʱ�޷�ʵ��


		/*
		 * @brief	��ͣ��ʱ����ʱֵ������
		 */
		virtual void Pause()
		{
		}



		/*
		 * @brief	������ʱ����Pause���ʹ��
		 */
		virtual void Continue()
		{
		}


		
		/*
		 * @brief	������ʱ��ʱ�䵽����ֹͣ��ֻ��ʱһ�Σ�
		 */
		virtual void StartOnce()
		{
		}

	};
	
}



extern "C"
{
	void LPTimer_IRQHandler(void);
};


#endif  /*_ADX_K60_LPTMR_H_*/
