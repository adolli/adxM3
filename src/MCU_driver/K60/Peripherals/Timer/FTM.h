
#ifndef _ADX_K60_FTM_H_
#define _ADX_K60_FTM_H_

#include "adxM3.h"
#include "../../System/System.h"
#include "../PeripheralBase.h"
#include "../PeripheralInfo.h"
#include "../Port/GlobalPort.h"
#include "../Port/SignalPin.h"

namespace K60
{

	/*
	 * @brief	FlexTimer
	 *			FTM核心是个时基单元，继承并实现adxM3定时器
	 *			
	 */
	template< UINT32 BASE >
	class Peripheral< FTM_INFO<BASE> >
		: public adxM3::Timer
	{

		typedef FTM_INFO<BASE> PeripheralInfo;
		typedef ClockGateController<typename PeripheralInfo::ClockSourceGate> ClockSource;

	public:



		/*
		 * @brief	定时器时钟源选项
		 */
		enum CLKSRC
		{
			NoClockSource			= 0,
			SystemClock				= 1,
			FixedFrequencyClock		= 2,
			ExternalClock			= 3
		};




		/*
		 * @brief	标准构造函数
		 *			初始化时默认时钟源为系统时钟
		 */
		Peripheral(UINT32 time_us = 0)
			: previousClockSource(SystemClock)
		{
			ClockSource::ClockOn();
			

			// 注册全局实例指针
			PeripheralInfo::InstancePtr = this;

			if (time_us > 0)
			{
				SetTimeOut(time_us);
			}

			// 配置NVIC
			CM4::$NVIC::SetPriority(PeripheralInfo::PeripheralIRQ,
				PeripheralInfo::InteruptPriority);
			CM4::$NVIC::EnableIRQ(PeripheralInfo::PeripheralIRQ);

			
		}


		virtual ~Peripheral()
		{
			ClockSource::ClockOff();
		}




		/*
		 * @brief	启动计时
		 */
		virtual void Start()
		{
			SetClockSource(previousClockSource);
		}





		/*
		 * @brief	停止计时
		 *			并将计数器回归到初始值
		 */
		virtual void Stop()
		{
			// 当已经停止时不能再停止，以免丢失先前的时钟源信息
			CLKSRC clksrc = GetClockSource();
			if (clksrc != NoClockSource)
			{
				previousClockSource = clksrc;
				SetClockSource(NoClockSource);

				// 写入任意值到CNT寄存器以初始化其值为CNTIN的值
				PeripheralInfo::BaseAccess()->CNT = 0;
			}
		}



		/*
		 * @brief	暂停计时（计时值保留）
		 *			请务必与Continue方法配对使用
		 */
		virtual void Pause()
		{
			// 将时钟源切除
			CLKSRC clksrc = GetClockSource();
			if (clksrc != NoClockSource)
			{
				previousClockSource = clksrc;
				SetClockSource(NoClockSource);
			}
		}



		/*
		 * @brief	继续计时，与Pause配对使用
		 */
		virtual void Continue()
		{
			//	恢复时钟源
			SetClockSource(previousClockSource);
		}




		enum
		{
			PulsePerUS = PeripheralInfo::ModuleClock::Frequency / 1000000,
			ModuleClockFrequencyInMHz = PulsePerUS,
			ModuleClockFrequencyInHz = PeripheralInfo::ModuleClock::Frequency
		};



		/*
		 * @brief	FTM设置/获取与分频器值
		 * @param	psc_value 预分频器值
		 *				@arg 1, 2, 4, 8, 16, 32, 64, 128
		 * @notice	请不要设置上述列出的值以外的值
		 */
		void SetPrescaler(UINT8 psc_value)
		{
			StaticSetPrescalerImpl(psc_value);
		}
		UINT8 GetPrescaler() const
		{
			return StaticGetPrescalerImpl();
		}



		/*
		 * @brief	获取当前计时器计时时间值
		 */
		virtual UINT32 GetTime() const
		{
			UINT8 psc = GetPrescaler();
			
			// 计算出已经计数的tick数，用现在计数值减去初始计数值
			// （使用有符号的计算，最后转换成无符号）
			UINT32 countedVal = (UINT32)((int)PeripheralInfo::BaseAccess()->CNT - (int)PeripheralInfo::BaseAccess()->CNTIN);
			return countedVal * psc / PulsePerUS;
		}



		/*
		 * @brief	设置计时器定时时长
		 *			单位 us
		 * @notice	定时长度不要超过 93,200 us
		 */
		virtual void SetTimeOut(UINT32 time_us)
		{
			StaticSetTimeOutImpl(time_us);
		}



		/*
		 * @brief	设置定时器定时频率
		 * @notice	功能同SetTimeOut，但设置的是TimeOut的倒数，即频率
		 *			频率最小值为11Hz，小于此值时自动返回
		 */
		virtual void SetFrequency(UINT32 freq)
		{
			StaticSetFrequencyImpl(freq);
		}



		/*
		 * @brief	获取定时器定时时长
		 * @notice	当定时时长较长时（超过10ms），获取值时可能会产生一定误差
		 */
		virtual UINT32 GetTimeOut() const
		{
			return StaticGetTimeOutImpl();
		}



		/*
		 * @brief	获取定时器定时频率
		 */
		virtual UINT32 GetFrequency() const
		{
			return StaticGetFrequencyImpl();
		}






		/*
		 * @brief	判断定时器是否正在计时
		 * @retval	正在计时返回true，否则返回false
		 */
		virtual bool TimerIsRunning() const
		{
			return (PeripheralInfo::BaseAccess()->SC.CLKS != 0);
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
				PeripheralInfo::BaseAccess()->SC.TOIE = SET;
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
				PeripheralInfo::BaseAccess()->SC.TOIE = RESET;
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
				PeripheralInfo::BaseAccess()->SC.TOIE = SET;
				return 1;

			default:
				return 0;
			}
		}





		/*
		 * @brief	FTM选择时钟源
		 * @param	clksrc 时钟源，可以是CLKSRC枚举中的任何一个
		 */
		void SetClockSource(const CLKSRC clksrc)
		{
			if (PeripheralInfo::BaseAccess()->MODE.WPDIS == 0)
			{
				PeripheralInfo::BaseAccess()->MODE.WPDIS = SET;
			}	
			PeripheralInfo::BaseAccess()->SC.CLKS = clksrc;
		}



		/*
		 * @brief	FTM获取当前时钟源
		 * @retval	时钟源，可以是CLKSRC枚举中的任何一个
		 */
		CLKSRC GetClockSource() const
		{
			return CLKSRC(PeripheralInfo::BaseAccess()->SC.CLKS);
		}




		/*
		 * @brief	计数器计数方向选项
		 */
		enum CNTDIR
		{
			CountUp,
			CenterAligned
		};



		/*
		 * @brief	FTM设置/获取计数器计数方向
		 * @param	dir 计数方向，可以是CNTDIR枚举中的任何一个
		 * @retval	计数方向
		 * @notice	该方法仅在非写保护状态(MODE[WPDIS] == 1)下生效
		 */
		void SetCounterDirection(const CNTDIR dir)
		{
			STATIC_ASSERT(CountUp == 0 && CenterAligned == 1,
				enumerate_value_is_not_campacted_with_this_configuration);
			PeripheralInfo::BaseAccess()->SC.CPWMS = dir;
		}
		CNTDIR GetCounterDirection() const
		{
			return CNTDIR(PeripheralInfo::BaseAccess()->SC.CPWMS);
		}



		// 定时器溢出事件响应函数
		adxM3::Function< void(void*) > timeOutISR;


	private:

		CLKSRC previousClockSource;


	public:


		/*
		 * @brief	FTM输入捕获或者输出比较通道
		 */
		template< UINT32 CHANNEL_INDEX >
		class CH
		{
		public:

			CH()
			{
				STATIC_ASSERT(CHANNEL_INDEX < PeripheralInfo::Channel_MAX,
					FTM_does_not_have_so_much_capture_or_compare_channels);

				ClockSource::ClockOn();

				// 配置NVIC
				CM4::$NVIC::SetPriority(PeripheralInfo::PeripheralIRQ,
					PeripheralInfo::InteruptPriority);
				CM4::$NVIC::EnableIRQ(PeripheralInfo::PeripheralIRQ);
			}

			~CH()
			{
				ClockSource::ClockOff();
			}



			/*
			 * @brief	捕获/比较模式选项
			 */
			enum CCMODE
			{
				CaptureOnRisingEdge		= 0x1,
				CaptureOnFallingEdge	= 0x2,
				CaptureOnBothEdge		= 0x3,
				ToggleOutputOnMatch		= 0x5,
				ClearOutputOnMatch		= 0x6,
				SetOutputOnMatch		= 0x7,
			};



			/*
			 * @brief	捕获/比较模式配置
			 * @param	mode 捕获模式或者比较模式，可以是CCMODE枚举中的任何一个
			 *			comparaVal 输出比较值，仅在输出比较模式下该参数有意义
			 */
			void CaptureCompareConfig(const CCMODE mode, const UINT16 comparaVal = 0)
			{
				STATIC_ASSERT(
					CaptureOnRisingEdge		== 0x1 &&
					CaptureOnFallingEdge	== 0x2 &&
					CaptureOnBothEdge		== 0x3 &&
					ToggleOutputOnMatch		== 0x5 &&
					ClearOutputOnMatch		== 0x6 &&
					SetOutputOnMatch		== 0x7,
					enumerate_value_is_not_campacted_with_this_configuration);


				PeripheralInfo::BaseAccess()
					->CONTROLS[CHANNEL_INDEX].CnSC.MS = (mode & 0xC) >> 2;
				PeripheralInfo::BaseAccess()
					->CONTROLS[CHANNEL_INDEX].CnSC.ELS = mode & 0x3;

				// 配置比较值（只在输出比较模式下有效）
				PeripheralInfo::BaseAccess()->CONTROLS[CHANNEL_INDEX].CnV = comparaVal;
			}






		protected:

			typedef typename PeripheralInfo::Channels Channels;
			typedef typename TL::TypeAt<Channels, CHANNEL_INDEX>::Result ChannelSignal;
			
			SignalPin<ChannelSignal> ChannelPin;

		};



		/*
		 * @brief	FTM PWM输出通道
		 */
		template< UINT32 CHANNEL_INDEX >
		class PWM
			: protected CH<CHANNEL_INDEX>
			, public adxM3::PulseWidthModulator
			, public adxM3::TimerIOEnabler
		{
		public:


			typedef Peripheral<PeripheralInfo> PeripheralFTM;



			/*
			 * @brief	PWM通道标准构造函数
			 *			构造时默认配置为HighTruePulsePWM
			 *			构造时不改变定时器周期，请注意先配置好定时器再构造PWM通道
			 * @param	duty 占空比，范围[0,1]
			 *			period PWM波周期，周期为0时表示不改变原先脉冲周期
			 *			align PWM对齐模式
			 *				@arg EdgeAligned 边沿对齐
			 *				@arg CenterAligned 中央对齐
			 */
			PWM(float duty = 0, const UINT32 period = 0, const PWMALIGN align = EdgeAligned)
			{
				STATIC_ASSERT(CHANNEL_INDEX < PeripheralInfo::Channel_MAX,
					FTM_does_not_have_so_much_PWM_channels);

				// 配置为HighTruePulse模式
				PeripheralInfo::BaseAccess()->CONTROLS[CHANNEL_INDEX].CnSC.MS = 3;
				PeripheralInfo::BaseAccess()->CONTROLS[CHANNEL_INDEX].CnSC.ELS = 2;

				// 配置必须按照以下的顺序

				// 设置对齐模式
				SetAlignment(align);

				// 设置PWM波周期
				if (period != 0)
				{
					SetPulsePeriod(period);
				}

				// 设置PWM占空比
				SetDutyCycle(duty);

				// 切入时钟源
				PeripheralInfo::BaseAccess()->SC.CLKS = 1;

				// 比较值快速装载
				PeripheralInfo::BaseAccess()->PWMLOAD.LDOK = SET;
				PeripheralInfo::BitBandAccess()->PWMLOAD[CHANNEL_INDEX] = SET;
			}

			virtual ~PWM()
			{
			}




			/*
			 * @brief	脉宽调制器输出使能/禁止
			 *			并配置好输入输出相关模式设置
			 * @retval	成功开启/关闭输出返回1，否则返回0
			 */
			virtual int EnableOutput()
			{
				PeripheralInfo::BitBandAccess()->OUTMASK[CHANNEL_INDEX] = RESET;
				return 1;
			}
			virtual int DisableOutput()
			{
				PeripheralInfo::BitBandAccess()->OUTMASK[CHANNEL_INDEX] = SET;
				return 1;
			}




			/*
			 * @brief	脉宽调制器设置占空比（百分比，精确到1%）
			 * @param	duty 占空比，取值范围[0,1]，此方法只取用小数部分，（请用户不要超过取值范围）
			 * @param	duty_percent 占空比，取值范围[0,100]，
			 *				重载一个非浮点的版本，实现时请不要涉及浮点运算
			 */
			virtual void SetDutyCycle(float duty)
			{
				INT16 cntin = PeripheralInfo::BaseAccess()->CNTIN;
				INT16 mod = PeripheralInfo::BaseAccess()->MOD;

				if (GetAlignment() == EdgeAligned)
				{
					PeripheralInfo::BaseAccess()->CONTROLS[CHANNEL_INDEX].CnV 
						= duty * (mod - cntin + 1) + cntin;
				}
				else // Center Aligned
				{
					PeripheralInfo::BaseAccess()->CONTROLS[CHANNEL_INDEX].CnV 
						= duty * (mod - cntin) + cntin;
				}
			}
			virtual void SetDutyCyclePercent(const UINT8 duty_percent)
			{
				INT16 cntin = PeripheralInfo::BaseAccess()->CNTIN;
				INT16 mod = PeripheralInfo::BaseAccess()->MOD;
				
				if (GetAlignment() == EdgeAligned)
				{
					PeripheralInfo::BaseAccess()->CONTROLS[CHANNEL_INDEX].CnV 
						= duty_percent * (mod - cntin + 1) / 100 + cntin;
				}
				else // Center Aligned
				{
					PeripheralInfo::BaseAccess()->CONTROLS[CHANNEL_INDEX].CnV 
						= duty_percent * (mod - cntin) / 100 + cntin;
				}
			}



			/*
			 * @brief	脉宽调制器获取占空比（百分比，精确到1%）
			 * @retval	占空比，取值范围[0,1]
			 */
			virtual float GetDutyCycle() const
			{
				INT16 cntin = PeripheralInfo::BaseAccess()->CNTIN;
				INT16 mod = PeripheralInfo::BaseAccess()->MOD;
				INT16 cnv = PeripheralInfo::BaseAccess()->CONTROLS[CHANNEL_INDEX].CnV;

				if (GetAlignment() == EdgeAligned)
				{
					return (float)(cnv - cntin) / (mod - cntin + 1);
				}
				else
				{
					return (float)(cnv - cntin) / (mod - cntin);
				}
			}
			virtual UINT8 GetDutyCyclePercent() const
			{
				INT16 cntin = PeripheralInfo::BaseAccess()->CNTIN;
				INT16 mod = PeripheralInfo::BaseAccess()->MOD;
				INT16 cnv = PeripheralInfo::BaseAccess()->CONTROLS[CHANNEL_INDEX].CnV;

				if (GetAlignment() == EdgeAligned)
				{
					return (cnv - cntin) * 100 / (mod - cntin + 1);
				}
				else
				{
					return (cnv - cntin) * 100 / (mod - cntin);
				}
			}



			/*
			 * @brief	脉宽调制器设置脉冲周期
			 *			调节脉冲周期不会改变占空比
			 * @param	period_us 脉冲周期，单位us
			 */
			virtual void SetPulsePeriod(const UINT32 period_us)
			{
				StaticSetTimeOutImpl(period_us);
			}





			/*
			 * @brief	脉宽调制器设置脉冲周期
			 * @retval	脉冲周期，单位us
			 */
			virtual UINT32 GetPulsePeriod() const
			{
				return PeripheralFTM::StaticGetTimeOutImpl();
			}





			/*
			 * @brief	脉宽调制器设置方波频率
			 * @param	freq 脉冲（方波）周期，单位Hz，至少大于22Hz
			 * @retval	脉冲（方波）周期，单位Hz
			 */
			virtual void SetFrequency(const UINT32 freq)
			{
				StaticSetFrequencyImpl(freq);
			}
			virtual UINT32 GetFrequency() const
			{
				return StaticGetFrequencyImpl();
			}




			/*
			 * @brief	脉宽调制器设置高电平脉宽
			 * @param	width 脉冲宽度，单位us，该值请不要超过脉冲周期
			 * @retval	脉冲宽度，单位us
			 */
			virtual void SetPulseWidth(const UINT32 width)
			{
				INT16 cntin = PeripheralInfo::BaseAccess()->CNTIN;
				UINT8 psc = StaticGetPrescalerImpl();

				PeripheralInfo::BaseAccess()->CONTROLS[CHANNEL_INDEX].CnV 
					= width * PulsePerUS / psc + cntin;
			}
			virtual UINT32 GetPulseWidth() const
			{
				INT16 cntin = PeripheralInfo::BaseAccess()->CNTIN;
				INT16 cnv = PeripheralInfo::BaseAccess()->CONTROLS[CHANNEL_INDEX].CnV;
				UINT8 psc = StaticGetPrescalerImpl();
				return UINT32(cnv - cntin) * psc / PulsePerUS;
			}




			/*
			 * @brief	PWM极性配置
			 * @param	pol 极性，可以是PWMPOL枚举中任何一个
			 *				@arg ActiveLow 成功比较时低电平(LowTruePulse)
			 *				@arg ActiveHigh 成功比较时高电平(HighTruePulse)
			 */
			virtual void SetPolarity(const PWMPOL pol)
			{
				STATIC_ASSERT(ActiveLow == 0 && ActiveHigh == 1,
					enumerate_value_is_not_campacted_with_this_configuration);
				PeripheralInfo::BitBandAccess()->POL[CHANNEL_INDEX] = !pol;
			}




			/*
			 * @brief	PWM对齐设置/获取
			 * @param	align PWM对齐模式，可以是PWMALIGN枚举中的任何一个
			 */
			void SetAlignment(const PWMALIGN align)
			{
				STATIC_ASSERT(EdgeAligned == 0 && CenterAligned == 1,
					enumerate_value_is_not_campacted_with_this_configuration);
				
				PeripheralInfo::BaseAccess()->SC.CPWMS = align;
			}
			PWMALIGN GetAlignment() const 
			{
				STATIC_ASSERT(EdgeAligned == 0 && CenterAligned == 1,
					enumerate_value_is_not_campacted_with_this_configuration);

				return PWMALIGN(PeripheralInfo::BaseAccess()->SC.CPWMS);
			}





		protected:

			typedef typename PeripheralInfo::Channels Channels;
			typedef typename TL::TypeAt<Channels, CHANNEL_INDEX>::Result ChannelSignal;
			
			// PWM通道引脚
			SignalPin<ChannelSignal> ChannelPin;

		};



		/*
		 * @brief	FTM 互补PWM输出通道
		 *			只能为EdgeAligned对齐方式
		 */
		template< UINT32 PAIR_CHANNEL_INDEX >
		class ComplementaryPWM
			: public PWM<PAIR_CHANNEL_INDEX * 2>
			, public PWM<PAIR_CHANNEL_INDEX * 2 + 1>
		{

			/*
			 * @brief	双路PWM通道号
			 */
			enum 
			{
				PAIR_CH	= PAIR_CHANNEL_INDEX,
				CHA		= PAIR_CHANNEL_INDEX * 2,
				CHB		= PAIR_CHANNEL_INDEX * 2 + 1
			};


		public:


			using PWM<CHA>::GetAlignment;


			ComplementaryPWM(float duty = 0, const UINT32 period = 0)
				: PWM<CHA>(0, period), PWM<CHB>(duty)
			{
				PeripheralInfo::BaseAccess()->MODE.WPDIS = SET;
				PeripheralInfo::BaseAccess()->MODE.FTMEN = SET;


				// 组合模式配置
				PeripheralInfo::BitBandAccess()
				   ->COMBINE[PAIR_CH][FTM_BitBandType::COMBINE_COMBINE] = SET;
				PeripheralInfo::BitBandAccess()
				   ->COMBINE[PAIR_CH][FTM_BitBandType::COMBINE_DECAP] = RESET;
				PeripheralInfo::BitBandAccess()
				   ->COMBINE[PAIR_CH][FTM_BitBandType::COMBINE_SYNCEN] = SET;		// 同步总使能

				
				// 互补输出
				PeripheralInfo::BitBandAccess()
				   ->COMBINE[PAIR_CH][FTM_BitBandType::COMBINE_COMP] = SET;			// 互补模式

				PeripheralInfo::BaseAccess()->QDCTRL.QUADEN = RESET;
				PeripheralInfo::BaseAccess()->SC.CPWMS = RESET;

				// PWM装载使能
				PeripheralInfo::BitBandAccess()->PWMLOAD[CHA] = SET;
				PeripheralInfo::BitBandAccess()->PWMLOAD[CHB] = SET;
				PeripheralInfo::BaseAccess()->PWMLOAD.LDOK = SET;

				// 同步配置
				PeripheralInfo::BaseAccess()->SYNC.SYNCHOM = SET;					// OutMask 同步总开关
				PeripheralInfo::BaseAccess()->SYNCONF.SYNCMODE = SET;				// Enhanced PWM synchronization
				PeripheralInfo::BaseAccess()->SYNCONF.SWWRBUF = SET;				// MOD, CNTIN, CV 同步
				PeripheralInfo::BaseAccess()->SYNCONF.SWRSTCNT = SET;				// 计数功能寄存器同步使能
				PeripheralInfo::BaseAccess()->SYNCONF.SWOM = SET;					// OutMask 同步
				PeripheralInfo::BaseAccess()->SYNCONF.SWINVC = SET;					// Inverting 同步
				PeripheralInfo::BaseAccess()->SYNCONF.INVC = SET;					// Inverting 伴随PWM同步
				PeripheralInfo::BaseAccess()->SYNCONF.CNTINC = SET;					// CNTIN 伴随PWM同步

				// 同步装载点配置
				PeripheralInfo::BaseAccess()->SYNC.CNTMAX = SET;
				PeripheralInfo::BaseAccess()->SYNC.CNTMIN = SET;

				// 初始时不使能
				DisableOutput();
			}





			/*
			 * @brief	使能死区控制
			 */
			void EnableDeadTimeInsertion()
			{
				PeripheralInfo::BitBandAccess()->COMBINE[PAIR_CH][FTM_BitBandType::COMBINE_DTEN] = SET;
				PeripheralInfo::BaseAccess()->DEADTIME.DTPS = 2;
				PeripheralInfo::BaseAccess()->DEADTIME.DTVAL = 48;
			}
			void DisableDeadTimeInsertion()
			{
				PeripheralInfo::BitBandAccess()->COMBINE[PAIR_CH][FTM_BitBandType::COMBINE_DTEN] = RESET;
			}




			/*
			 * @brief	互补PWM反相使能与禁止
			 */
			void EnableInverting()
			{
				PeripheralInfo::BitBandAccess()->INVCTRL[PAIR_CH] = SET;
				PeripheralInfo::BaseAccess()->SYNC.SWSYNC = SET;
			}
			void DisableInverting()
			{
				PeripheralInfo::BitBandAccess()->INVCTRL[PAIR_CH] = RESET;
				PeripheralInfo::BaseAccess()->SYNC.SWSYNC = SET;
			}




			/*
			 * @brief	脉宽调制器输出使能/禁止
			 *			并配置好输入输出相关模式设置
			 * @retval	成功开启/关闭输出返回1，否则返回0
			 */
			virtual int EnableOutput()
			{
				PeripheralInfo::BaseAccess()->OUTMASK &= ~(UINT8)(_BV(CHA) | _BV(CHB));
				PeripheralInfo::BaseAccess()->SYNC.SWSYNC = SET;
				return 1;
			}
			virtual int DisableOutput()
			{
				PeripheralInfo::BaseAccess()->OUTMASK |= (UINT8)(_BV(CHA) | _BV(CHB));
				PeripheralInfo::BaseAccess()->SYNC.SWSYNC = SET;
				return 1;
			}





			/*
			 * @brief	脉宽调制器设置占空比（百分比，精确到1%）
			 * @param	duty 占空比，取值范围[0,1]，此方法只取用小数部分，（请用户不要超过取值范围）
			 * @param	duty_percent 占空比，取值范围[0,100]，
			 *				重载一个非浮点的版本，实现时请不要涉及浮点运算
			 */
			virtual void SetDutyCycle(float duty)
			{
				INT16 cntin = PeripheralInfo::BaseAccess()->CNTIN;
				INT16 mod = PeripheralInfo::BaseAccess()->MOD;

				UINT16 regval = duty * (mod - cntin + 1) + cntin;
				PeripheralInfo::BaseAccess()->CONTROLS[CHA].CnV = 0;
				PeripheralInfo::BaseAccess()->CONTROLS[CHB].CnV = regval;
				PeripheralInfo::BaseAccess()->SYNC.SWSYNC = SET;
			}
			virtual void SetDutyCyclePercent(const UINT8 duty_percent)
			{
				INT16 cntin = PeripheralInfo::BaseAccess()->CNTIN;
				INT16 mod = PeripheralInfo::BaseAccess()->MOD;
				
				UINT16 regval = duty_percent * (mod - cntin + 1) / 100 + cntin;
				PeripheralInfo::BaseAccess()->CONTROLS[CHA].CnV = 0;
				PeripheralInfo::BaseAccess()->CONTROLS[CHB].CnV = regval;
				PeripheralInfo::BaseAccess()->SYNC.SWSYNC = SET;
			}




			/*
			 * @brief	脉宽调制器获取占空比（百分比，精确到1%）
			 * @retval	占空比，取值范围[0,1]
			 */
			virtual float GetDutyCycle() const
			{
				INT16 cntin = PeripheralInfo::BaseAccess()->CNTIN;
				INT16 mod = PeripheralInfo::BaseAccess()->MOD;
				INT16 cav = PeripheralInfo::BaseAccess()->CONTROLS[CHA].CnV;
				INT16 cbv = PeripheralInfo::BaseAccess()->CONTROLS[CHB].CnV;

				return (float)abs(cav - cbv) / (mod - cntin + 1);
			}
			virtual UINT8 GetDutyCyclePercent() const
			{
				INT16 cntin = PeripheralInfo::BaseAccess()->CNTIN;
				INT16 mod = PeripheralInfo::BaseAccess()->MOD;
				INT16 cav = PeripheralInfo::BaseAccess()->CONTROLS[CHA].CnV;
				INT16 cbv = PeripheralInfo::BaseAccess()->CONTROLS[CHB].CnV;

				return abs(cav - cbv) * 100 / (mod - cntin + 1);
			}




			/*
			 * @brief	PWM极性配置
			 * @param	pol 极性，可以是PWMPOL枚举中任何一个
			 *				@arg ActiveLow 成功比较时低电平(LowTruePulse)
			 *				@arg ActiveHigh 成功比较时高电平(HighTruePulse)
			 */
			virtual void SetPolarity(const PulseWidthModulator::PWMPOL pol)
			{
				STATIC_ASSERT(	PulseWidthModulator::ActiveLow == 0 
							&&  PulseWidthModulator::ActiveHigh == 1,
					enumerate_value_is_not_campacted_with_this_configuration);
				PeripheralInfo::BitBandAccess()->POL[CHA] = !pol;
				PeripheralInfo::BitBandAccess()->POL[CHB] = !pol;
				PeripheralInfo::BaseAccess()->SYNC.SWSYNC = SET;
			}



		private:


			using PWM<CHA>::SetAlignment;


		};




		/*
		 * @brief	FTM 正交解码器输入通道
		 */
		class QuadratureDecoder
		{
		public:



			// 区分正反计数
			typedef INT16 CounterValueType;



			/*
			 * @brief	 编码器编码方式选项
			 */
			enum
			{
				PHA_PHB_Encoding,
				CNT_DIR_Encoding
			};




			QuadratureDecoder()
			{
				STATIC_ASSERT(
						PeripheralInfo::BaseAddress == FTM1_BASE
					||	PeripheralInfo::BaseAddress == FTM2_BASE,
					this_FTM_peripheral_does_not_support_quadrature_decode_functions);

				ClockSource::ClockOn();


				// QUAD 模式要求使能FTM
				WriteProtectDisable();
				PeripheralInfo::BaseAccess()->MODE.FTMEN = SET;

				// QUAD 两相解码模式
				PeripheralInfo::BaseAccess()->QDCTRL.QUADMODE = PHA_PHB_Encoding;


				PeripheralInfo::BaseAccess()->CNTIN = 0;
				PeripheralInfo::BaseAccess()->MOD = 0xFFFF;
				PeripheralInfo::BaseAccess()->CNT = 0;


				// QUAD 模式使能
				PeripheralInfo::BaseAccess()->QDCTRL.QUADEN = SET;
			}




			~QuadratureDecoder()
			{
				ClockSource::ClockOff();
			}




			/*
			 * @brief	获取当前编码计数值
			 */
			INT16 GetCounter() const
			{
				return (INT16)PeripheralInfo::BaseAccess()->CNT;
			}



			/*
			 * @brief	复位编码计数器计数值
			 */
			void ResetCounter()
			{
				PeripheralInfo::BaseAccess()->CNT = 0;
			}




			/*
			 * @brief	解码器计数方向选项
			 *			根据右手定则区分正反方向，
			 *			以编码器伸出的轴指向为正向，轴向右手四指旋转方向为正转（forward）
			 */
			enum QCNTDIR
			{
				ForwardUpCounting,			// 正转时向上计数
				ForwardDownCounting			// 正转是向下计数
			};


			/*
			 * @brief	
			 */
			void SetCountingDirection(QCNTDIR dir)
			{
				STATIC_ASSERT(ForwardUpCounting == 0 && ForwardDownCounting == 1,
					enumerate_value_is_not_campacted_with_this_configuration);
				PeripheralInfo::BaseAccess()->QDCTRL.PHAPOL = dir;
				PeripheralInfo::BaseAccess()->QDCTRL.PHBPOL = dir;
			}




			
			enum FLWT
			{
				UnderFlow,
				OverFlow,
			};


			FLWT GetFlowDirection() const
			{
				return FLWT(PeripheralInfo::BaseAccess()->QDCTRL.TOFDIR);
			}




			enum DIR
			{
				CountDown,
				CountUp,
			};

			DIR GetCounterDirection() const
			{
				return DIR(PeripheralInfo::BaseAccess()->QDCTRL.QUADIR);
			}



		private:

			typedef typename PeripheralInfo::QuadratureInputs::PHA QPHA;
			typedef typename PeripheralInfo::QuadratureInputs::PHB QPHB;

			// 正交编码器信号引脚
			SignalPin<QPHA> QuadratureInputPhaseA;
			SignalPin<QPHB> QuadratureInputPhaseB;

		};




	protected:

		/*
		 * @brief	设置/获取定时器定时周期，静态实现
		 */
		static inline void StaticSetTimeOutImpl(const UINT32 time_us)
		{
			// 为了保证FTM在使用时，不发生配置错误，
			// （如PWM模式下MOD寄存器限制值范围[0,7FFF]），
			// FTM定时一律按上述MOD范围配置，代价是定时长度减半，和计数器精度减半

			// 超过定时长度则直接返回
			if (time_us > 46604) return;

			// 默认情况下使初始值为0，修改模值（终止值）来确定定时时长
			PeripheralInfo::BaseAccess()->CNTIN = 0;

			// FTM用的时钟源是SystemClock，频率较高，定时时长较短
			// 对PSC分段处理，从小开始分，直到找到一个PSC值使得计数器满溢出的时长大于定时时长
			UINT8 psc = 1;
			//					以下为0x7FFF，限制MOD寄存器值[0,7FFF]
			while (time_us > psc * 0x7FFF / ModuleClockFrequencyInMHz && psc < 128)
			{
				psc <<= 1;
			}
			StaticSetPrescalerImpl(psc);
			PeripheralInfo::BaseAccess()->MOD = time_us * PulsePerUS / psc - 1;
		}
		static inline UINT32 StaticGetTimeOutImpl()
		{
			UINT8 psc = StaticGetPrescalerImpl();
			
			// 计算出可计数的最大tick数，用模值减去初始值
			// （使用有符号的计算，最后转换成无符号）
			UINT32 maxCount = (UINT32)((int)PeripheralInfo::BaseAccess()->MOD - (int)PeripheralInfo::BaseAccess()->CNTIN);
			return (maxCount + 1) * psc / PulsePerUS;
		}



		/*
		 * @brief	设置/获取定时器溢出频率，静态实现
		 */
		static inline void StaticSetFrequencyImpl(const UINT32 freq)
		{
			if (freq < 22) return;

			// 默认情况下使初始值为0，修改模值（终止值）来确定定时时长（频率）
			PeripheralInfo::BaseAccess()->CNTIN = 0;

			// FTM用的时钟源是SystemClock，频率较高，定时时长较短
			// 对PSC分段处理，从大开始分，直到找到一个PSC值使得计数器满溢出频率大于定时频率
			UINT8 psc = 128;
			//									以下为0x7FFF，限制MOD寄存器值[0,7FFF]
			while (freq > ModuleClockFrequencyInHz / 0x7FFF / psc && psc > 0)
			{
				psc >>= 1;
			}

			if (psc == 0)
			{
				psc = 1;
			}
			else
			{
				psc <<= 1;
			}
			StaticSetPrescalerImpl(psc);
			PeripheralInfo::BaseAccess()->MOD = ModuleClockFrequencyInHz / freq / psc - 1;
		}
		static inline UINT32 StaticGetFrequencyImpl()
		{
			UINT8 psc = StaticGetPrescalerImpl();

			// 计算出可计数的最大tick数，用模值减去初始值
			// （使用有符号的计算，最后转换成无符号）
			UINT32 maxCount = (UINT32)((int)PeripheralInfo::BaseAccess()->MOD - (int)PeripheralInfo::BaseAccess()->CNTIN);
			return ModuleClockFrequencyInHz / (maxCount + 1) / psc;
		}



		/*
		 * @brief	设置/获取定时器预分频器，静态实现
		 */
		static inline void StaticSetPrescalerImpl(const UINT8 psc_value)
		{
			// regval 为写入寄存器SC_PS域的值，他和预分频系数有以下关系
			// psc_value = _BV(regval);
			UINT8 regval = 0;
			while (_BV(regval) != psc_value && regval < 7)
			{
				++regval;
			}

			PeripheralInfo::BaseAccess()->SC.PS = regval;
		}
		static inline UINT8 StaticGetPrescalerImpl()
		{
			return _BV(PeripheralInfo::BaseAccess()->SC.PS);
		}



		/*
		 * @brief	禁止写保护
		 */
		static inline void WriteProtectDisable()
		{
			// 以下软件序列为RM手册上所说，务必检查WPDIS是否RESET后再SET
			if (PeripheralInfo::BaseAccess()->MODE.WPDIS == RESET)
			{
				PeripheralInfo::BaseAccess()->MODE.WPDIS = SET;
			}
		}



	private:



		/*
		 * @brief	启动计时，时间到后则停止（只计时一次）
		 * @notice	FTM无法实现只定时一次，请手动在溢出事件中停止该定时器
		 */
		virtual void StartOnce()
		{
			// 暂时无法实现定时一次
		}

	};
	
}



extern "C"
{
	void FTM0_IRQHandler(void);
	void FTM1_IRQHandler(void);
	void FTM2_IRQHandler(void);
};


#endif  /*_ADX_K60_FTM_H_*/
