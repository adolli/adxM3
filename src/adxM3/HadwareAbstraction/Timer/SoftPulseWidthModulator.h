
#ifndef _ADX_HA_SOFTPULSEWIDTHMODULATOR_H_
#define _ADX_HA_SOFTPULSEWIDTHMODULATOR_H_


#include "../../common/defs.h"
#include "../GPIO/GeneralPurposeInputOutputPin.h"


namespace adxM3
{


	/*
	 * @brief	软件脉宽调制器
	 *			占空比总是在该脉冲一个周期后更新
	 * @tparam	TimerType 定时器类型，要求实现以下方法（见using TimerType::xxx）
	 *			PinType 引脚类型，要求引脚实现以下方法
	 *				@required Setlevel 设置电平
	 *				@required Toggle 翻转电平
	 */
	template
	< 
		class TimerType,
		class PinType
	>
	class SoftPulseWidthModulator
		: public TimerType
		, public PulseWidthModulator
		, public TimerIOEnabler
	{
	protected:

		using TimerType::AddEventListener;
		using TimerType::RemoveEventListener;
		using TimerType::StartOnce;
		using TimerType::Start;
		using TimerType::Stop;
		using TimerType::SetTimeOut;


	public:



		SoftPulseWidthModulator(float duty = 0, UINT32 period = 0)
			: PulsePeriod(period), PulseWidth(0), ActivePolarity(ActiveHigh)
		{
			OutputChannel.SetDirection(GeneralPurposeInputOutput::OUT);
			OutputChannel.OutputConfig(GeneralPurposeInputOutput::PushPull, GeneralPurposeInputOutput::HighSpeed);
			OutputChannel.SetLevel(!ActivePolarity);

			if (period != 0)
			{
				SetPulsePeriod(period);
			}
			SetDutyCycle(duty);

			struct TimerBaseEventListener
			{
				static void OnUpdate(Timer* _This)
				{
					SoftPulseWidthModulator* This = dynamic_cast<SoftPulseWidthModulator*>(_This);
					if (!This)
					{
						return;
					}
					
					// 初始极性
					static bool currentOutputLevel = This->ActivePolarity;
					if (This->PulsePeriod != 0)
					{
						if (currentOutputLevel == This->ActivePolarity)
						{
							// 如果当前PWM输出电平处于活动状态，则下一次定时非活动状态脉宽
							This->SetTimeOut(This->PulsePeriod - This->PulseWidth);
						}
						else
						{
							This->SetTimeOut(This->PulseWidth);
						}
						This->OutputChannel.SetLevel(currentOutputLevel);
						currentOutputLevel = !currentOutputLevel;
					}
				}
			};
			AddEventListener(Timer::TIME_OUT, TimerBaseEventListener::OnUpdate);
		}





		/*
		 * @brief	脉宽调制器输出使能/禁止
		 *			并配置好输入输出相关模式设置
		 * @retval	成功开启/关闭输出返回1，否则返回0
		 */
		virtual int EnableOutput()
		{
			Start();
			return 1;
		}
		virtual int DisableOutput()
		{
			Stop();
			OutputChannel = !ActivePolarity;
			return 1;
		}






		/*
		 * @brief	脉宽调制器设置占空比（百分比，精确到1%）
		 * @param	duty 占空比，取值范围[0,1]，此方法只取用小数部分
		 * @param	duty_percent 占空比，取值范围[0,100]，
		 *				重载一个非浮点的版本，实现时请不要涉及浮点运算
		 */
		virtual void SetDutyCycle(float duty)
		{
			if (PulsePeriod == 0) return;

			RangeLimit(0.001, <=, duty, <=, 0.999);
			PulseWidth = PulsePeriod * duty;
		}
		virtual void SetDutyCyclePercent(const UINT8 duty_percent)
		{
			if (PulsePeriod == 0) return;

			UINT8 dty = duty_percent;
			RangeLimit(1, <=, dty, <=, 99);
			PulseWidth = PulsePeriod * dty / 100;
		}



		/*
		 * @brief	脉宽调制器获取占空比（百分比，精确到1%）
		 * @retval	占空比，取值范围[0,1]
		 */
		virtual float GetDutyCycle() const
		{
			if (PulsePeriod == 0) return 0;

			return (float)PulseWidth / PulsePeriod;
		}
		virtual UINT8 GetDutyCyclePercent() const
		{
			if (PulsePeriod == 0) return 0;

			return PulseWidth * 100 / PulsePeriod;
		}



		/*
		 * @brief	脉宽调制器设置脉冲周期
		 *			调节脉冲周期不会改变占空比
		 * @param	period_us 脉冲周期，单位us
		 */
		virtual void SetPulsePeriod(const UINT32 period_us)
		{
			PulsePeriod = period_us;
		}
		




		/*
		 * @brief	脉宽调制器设置脉冲周期
		 * @retval	脉冲周期，单位us
		 */
		virtual UINT32 GetPulsePeriod() const
		{
			return PulsePeriod;
		}





		/*
		 * @brief	脉宽调制器设置高电平脉宽
		 * @param	width 脉冲宽度，单位us，该值请不要超过脉冲周期
		 * @retval	脉冲宽度，单位us
		 * @ovride	为获得更好性能，派生类请重写该方法
		 */
		virtual void SetPulseWidth(const UINT32 width)
		{
			PulseWidth = width;
		}
		virtual UINT32 GetPulseWidth() const
		{
			return PulseWidth;
		}





		/*
		 * @brief	PWM极性配置
		 * @param	pol 极性，可以是PWMPOL枚举中任何一个
		 *				@arg ActiveLow 成功比较时低电平(LowTruePulse)
		 *				@arg ActiveHigh 成功比较时高电平(HighTruePulse)
		 */
		virtual void SetPolarity(const PWMPOL pol)
		{
			ActivePolarity = pol;
		}





	public:


		PinType OutputChannel;
		volatile UINT32 PulsePeriod;
		volatile UINT32 PulseWidth;
		volatile PWMPOL ActivePolarity;

	};
}

#endif  /*_ADX_HA_SOFTPULSEWIDTHMODULATOR_H_*/
