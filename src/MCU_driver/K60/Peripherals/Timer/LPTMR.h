
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
			

			// 选择外部参考时钟，即晶振
			OSC->CR |= OSC_CR_ERCLKEN_MASK;
			PeripheralInfo::BaseAccess()->PSR.PCS = 3;

			// 计时溢出自动复位计数器
			PeripheralInfo::BaseAccess()->CSR.TFC = RESET;

			// 定时器模式
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
			 * @type	LPTMR的计数器只能加计数
			 */ 
			typedef UINT16 CounterValueType;



			PulseCounter()
			{
				ClockSource::ClockOn();

				// 先禁止，再配置
				PeripheralInfo::BaseAccess()->CSR.TEN = RESET;

				// 计数器模式
				PeripheralInfo::BaseAccess()->CSR.TMS = SET;

				// free running counter
				PeripheralInfo::BaseAccess()->CSR.TFC = SET;

				// 预分频器旁路，不使用分频
				PeripheralInfo::BaseAccess()->PSR.PBYP = SET;

				// 选择引脚
				PeripheralInfo::BaseAccess()->CSR.TPS = InputSourceIndex;

				SetCountEdge(RisingEdge);
					
				// 使能计数器
				PeripheralInfo::BaseAccess()->CSR.TEN = SET;
			}


			~PulseCounter()
			{
				ClockSource::ClockOff();
			}




			/*
			 * @brief	获取脉冲计数值
			 */
			UINT16 GetCounter()
			{
				return PeripheralInfo::BaseAccess()->CNR;
			}



			/*
			 * @brief	复位计数器
			 */
			void ResetCounter()
			{
				PeripheralInfo::BaseAccess()->CSR.TEN = RESET;
				PeripheralInfo::BaseAccess()->CSR.TEN = SET;
			}




			/*
			 * @brief	设置计数边缘选项
			 */
			enum CEDGE
			{
				RisingEdge,
				FallingEdge
			};



			/*
			 * @brief	设置计数边缘
			 */
			void SetCountEdge(const CEDGE edge)
			{
				PeripheralInfo::BaseAccess()->CSR.TPP = edge;
			}



		};






		/*
		 * @brief	启动计时
		 */
		virtual void Start()
		{
			PeripheralInfo::BaseAccess()->CSR.TEN = SET;
		}




		/*
		 * @brief	停止计时（计时器归零）
		 */
		virtual void Stop()
		{
			PeripheralInfo::BaseAccess()->CSR.TEN = RESET;
		}






		enum { PulsePerUS = PeripheralInfo::ModuleClock::Frequency / 1000000 };



		/*
		 * @brief	获取当前计时器计时时间值
		 * @retval	当前计时值，单位us
		 */
		virtual UINT32 GetTime() const
		{
			UINT16 cnr = PeripheralInfo::BaseAccess()->CNR;
			UINT16 psc = PeripheralInfo::BaseAccess()->PSR.PRESCALE;
			return cnr * _BV(psc + 1) / PulsePerUS;
		}



		/*
		 * @brief	设置计时器定时时长
		 * @param	time 时长，单位us
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
		 * @brief	设置定时器定时频率，
		 * @notice	功能同SetTimeOut，但设置的是TimeOut的倒数，即频率
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
		 * @brief	获取定时器定时时长
		 */
		virtual UINT32 GetTimeOut() const
		{
			UINT16 cmr = PeripheralInfo::BaseAccess()->CMR;
			UINT16 psc = PeripheralInfo::BaseAccess()->PSR.PRESCALE;
			return cmr * _BV(psc + 1) / PulsePerUS;
		}



		/*
		 * @brief	获取定时器定时频率
		 */
		virtual UINT32 GetFrequency() const
		{
			UINT16 cmr = PeripheralInfo::BaseAccess()->CMR;
			UINT16 psc = PeripheralInfo::BaseAccess()->PSR.PRESCALE;
			return PulsePerUS / _BV(psc + 1) / cmr;
		}




		/*
		 * @brief	判断定时器是否正在计时
		 * @retval	正在计时返回true，否则返回false
		 */
		virtual bool TimerIsRunning() const
		{
			return PeripheralInfo::BaseAccess()->CSR.TEN;
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
		 * @brief	添加事件监听器，监听事件对象的某个事件
		 * @param	evt 对象的事件
		 *			FnPtr 触发该事件时的处理函数，由派生类在实现时转换到所需的类型
		 * @retval	成功注册某个事件监听器则返回1，否则返回0
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


		
		// 定时溢出回调函数
		adxM3::Function< void(void*) > timeOutISR;



	private:


		// 以下方法对于此外设暂时无法实现


		/*
		 * @brief	暂停计时（计时值保留）
		 */
		virtual void Pause()
		{
		}



		/*
		 * @brief	继续计时，与Pause配对使用
		 */
		virtual void Continue()
		{
		}


		
		/*
		 * @brief	启动计时，时间到后则停止（只计时一次）
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
