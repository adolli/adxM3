
#ifndef _ADX_K60_PIT_H_
#define _ADX_K60_PIT_H_

#include "adxM3.h"
#include "../../System/System.h"
#include "../PeripheralBase.h"
#include "../PeripheralInfo.h"
#include "../Port/GlobalPort.h"


namespace K60
{


	/*
	 * @brief	周期中断定时器
	 *			只能向下计数，达到0后自动重装在
	 */
	template< UINT32 BASE >
	class Peripheral< PIT_INFO<BASE> >
	{
	public:

		typedef PIT_INFO<BASE> PeripheralInfo;
		typedef ClockGateController<typename PeripheralInfo::ClockSourceGate> ClockSource;



		Peripheral()
		{
			ClockSource::ClockOn();
			EnablePeripheral();
		}

		~Peripheral()
		{
			ClockSource::ClockOff();
		}


		

		/*
		 * @brief	PIT外设总时钟使能/禁止
		 */
		void EnablePeripheral()
		{
			PeripheralInfo::BitBandAccess()->MCR[PIT_BitBandType::MCR_MDIS] = RESET;
		}
		void DisablePeripheral()
		{
			PeripheralInfo::BitBandAccess()->MCR[PIT_BitBandType::MCR_MDIS] = SET;
		}



		/*
		 * @brief	PIT定时通道类模版
		 * @tparam	CHANNEL_INDEX 定时通道号，范围[0,3]
		 */
		template< UINT32 CHANNEL_INDEX >
		class CH
			: public adxM3::Timer
		{
		public:


			CH(const UINT32 time_us = 0)
				: autoReloadValue(0), currentValue(0), paused(false)
			{
				STATIC_ASSERT(CHANNEL_INDEX < PeripheralInfo::TimerChannel_MAX,
					PIT_does_not_have_so_much_timer_channels);

				ClockSource::ClockOn();

				// 注册全局实例指针
				PeripheralInfo::InstancePtr[CHANNEL_INDEX] = this;

				// 设置定时时长
				SetTimeOut(time_us);

				// 配置NVIC
				CM4::$NVIC::SetPriority(PeripheralInfo::ChannelIQR[CHANNEL_INDEX],
					PeripheralInfo::InteruptPriority[CHANNEL_INDEX]);
				CM4::$NVIC::EnableIRQ(PeripheralInfo::ChannelIQR[CHANNEL_INDEX]);

				// 开启总时钟
				PeripheralInfo::BitBandAccess()->MCR[PIT_BitBandType::MCR_MDIS] = RESET;
			}

			virtual ~CH()
			{
				ClockSource::ClockOff();
			}



			/*
			 * @brief	启动计时
			 */
			virtual void Start()
			{
				PeripheralInfo::BitBandAccess()
					->CHANNEL[CHANNEL_INDEX].TCTRL[PIT_BitBandType::TCTRL_TEN] = SET;
			}



			/*
			 * @brief	启动计时，时间到后则停止（只计时一次）
			 */
			virtual void StartOnce()
			{
				// 启动定时器
				PeripheralInfo::BitBandAccess()
					->CHANNEL[CHANNEL_INDEX].TCTRL[PIT_BitBandType::TCTRL_TEN] = SET;
				
				// 启动后清除自动重装载值可实现一次定时
				PeripheralInfo::BaseAccess()->CHANNEL[CHANNEL_INDEX].LDVAL = 0;
			}



			/*
			 * @brief	停止计时（计时器归零）
			 */
			virtual void Stop()
			{
				PeripheralInfo::BitBandAccess()
					->CHANNEL[CHANNEL_INDEX].TCTRL[PIT_BitBandType::TCTRL_TEN] = RESET;
			}



			/*
			 * @brief	暂停计时（计时值保留）
			 *			请务必与Continue方法配对使用
			 */
			virtual void Pause()
			{
				paused = true;

				// 先保存当前定时器计数值
				currentValue = PeripheralInfo::BaseAccess()->CHANNEL[CHANNEL_INDEX].CVAL;

				// 停止定时器
				PeripheralInfo::BitBandAccess()
					->CHANNEL[CHANNEL_INDEX].TCTRL[PIT_BitBandType::TCTRL_TEN] = RESET;
			}



			/*
			 * @brief	继续计时，与Pause配对使用
			 */
			virtual void Continue()
			{
				// 当且仅当paused == true时该方法有效
				// 这样是为了不要在停止的时候产生了一个不正确周期的中断
				if (!paused) return;

				// 由于定时器每次启动都自动重新从autoReloadValue开始倒数
				// 为了继续先前的计时，先设定计时值为先前的计数值
				PeripheralInfo::BaseAccess()->CHANNEL[CHANNEL_INDEX].LDVAL = currentValue;

				// 启动定时器
				PeripheralInfo::BitBandAccess()
					->CHANNEL[CHANNEL_INDEX].TCTRL[PIT_BitBandType::TCTRL_TEN] = SET;

				// 再恢复正常的autoReloadValue，下一周期得以正常计时
				PeripheralInfo::BaseAccess()->CHANNEL[CHANNEL_INDEX].LDVAL = autoReloadValue;

				paused = false;
			}




			enum
			{
				PulsePerUS = PeripheralInfo::ModuleClock::Frequency / 1000000
			};



			/*
			 * @brief	获取当前计时器计时时间值
			 */
			virtual UINT32 GetTime() const
			{
				return PeripheralInfo::BaseAccess()
					->CHANNEL[CHANNEL_INDEX].CVAL / PulsePerUS;
			}



			/*
			 * @brief	设置计时器定时时长
			 *			单位 us
			 * @notice	定时长度不要超过 29,000,000 us
			 */
			virtual void SetTimeOut(UINT32 time_us)
			{
				autoReloadValue = time_us * PulsePerUS;
				PeripheralInfo::BaseAccess()->CHANNEL[CHANNEL_INDEX].LDVAL = autoReloadValue;
			}



			/*
			 * @brief	设置定时器定时频率，
			 * @notice	功能同SetTimeOut，但设置的是TimeOut的倒数，即频率
			 */
			virtual void SetFrequency(UINT32 freq)
			{
				autoReloadValue = PulsePerUS / freq;
				PeripheralInfo::BaseAccess()->CHANNEL[CHANNEL_INDEX].LDVAL = autoReloadValue;
			}



			/*
			 * @brief	获取定时器定时时长
			 */
			virtual UINT32 GetTimeOut() const
			{
				return autoReloadValue / PulsePerUS;
			}



			/*
			 * @brief	获取定时器定时频率
			 */
			virtual UINT32 GetFrequency() const
			{
				return PulsePerUS / autoReloadValue;
			}




			/*
			 * @brief	判断定时器是否正在计时
			 * @retval	正在计时返回true，否则返回false
			 */
			virtual bool TimerIsRunning() const
			{
				return PeripheralInfo::BitBandAccess()->CHANNEL[CHANNEL_INDEX].TCTRL[PIT_BitBandType::TCTRL_TEN];
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
					PeripheralInfo::BitBandAccess()
						->CHANNEL[CHANNEL_INDEX].TCTRL[PIT_BitBandType::TCTRL_TIE] = SET;
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
					PeripheralInfo::BitBandAccess()
						->CHANNEL[CHANNEL_INDEX].TCTRL[PIT_BitBandType::TCTRL_TIE] = RESET;
					return 1;

				default:
					return 0;
				}
			}



			/*
			 * @brief	添加事件监听器，监听事件对象的某个事件
			 * @param	l 监听器对象，需要实现该监听器对应接口
			 */
			void SetEventListener(const TimerUpdateListener<Timer&>* l)
			{
				PeripheralInfo::BitBandAccess()
					->CHANNEL[CHANNEL_INDEX].TCTRL[PIT_BitBandType::TCTRL_TIE] = SET;
				updateListenerWrapper.SetEventListener(l);
			}
			



			/*
			 * @brief	更新自动重装计数器中的值
			 *			用在单次计时时，更新事件（中断）处恢复倒计时值
			 *			以便下次再调用Start方法时不用重新调用TimeOut方法;
			 */
			void UpdateAutoReloadValue()
			{
				PeripheralInfo::BaseAccess()->CHANNEL[CHANNEL_INDEX].LDVAL = autoReloadValue;
			}


		private:

			UINT32 autoReloadValue;
			UINT32 currentValue;
			bool paused;

		};


	};
	
	
}


extern "C"
{
	void PIT0_IRQHandler(void);
	void PIT1_IRQHandler(void);
	void PIT2_IRQHandler(void);
	void PIT3_IRQHandler(void);
};

#endif  /*_ADX_K60_PIT_H_*/
