
#ifndef _PULSEWIDTHMODULATOR_H_
#define _PULSEWIDTHMODULATOR_H_

#include "../../common/defs.h"


namespace adxM3
{

	/*
	 * @brief	脉宽调制器抽象模型
	 *			具有脉宽调制信号输出功能的外设可实现该类
	 */
	class PulseWidthModulator
	{
	public:


		/*
		 * @brief	脉宽调制器设置占空比（百分比，精确到1%）
		 * @param	duty 占空比，取值范围[0,1]，此方法只取用小数部分
		 * @param	duty_percent 占空比，取值范围[0,100]，
		 *				重载一个非浮点的版本，实现时请不要涉及浮点运算
		 */
		virtual void SetDutyCycle(float duty) = 0;
		virtual void SetDutyCyclePercent(const UINT8 duty_percent)
		{
			return;
		}



		/*
		 * @brief	脉宽调制器获取占空比（百分比，精确到1%）
		 * @retval	占空比，取值范围[0,1]
		 */
		virtual float GetDutyCycle() const = 0;
		virtual UINT8 GetDutyCyclePercent() const = 0;



		/*
		 * @brief	脉宽调制器设置脉冲周期
		 *			调节脉冲周期不会改变占空比
		 * @param	period_us 脉冲周期，单位us
		 */
		virtual void SetPulsePeriod(const UINT32 period_us) = 0;
		




		/*
		 * @brief	脉宽调制器设置脉冲周期
		 * @retval	脉冲周期，单位us
		 */
		virtual UINT32 GetPulsePeriod() const = 0;




		/*
		 * @brief	脉宽调制器设置方波频率
		 * @param	freq 脉冲（方波）周期，单位Hz，频率请不要超过1MHz
		 * @retval	脉冲（方波）周期，单位Hz
		 * @ovride	若需要频率超过1MHz派生类请重写该方法
		 */
		virtual void SetFrequency(const UINT32 freq)
		{
			SetPulsePeriod(1000000 / freq);
		}
		virtual UINT32 GetFrequency() const
		{
			return 1000000 / GetPulsePeriod();
		}



		/*
		 * @brief	脉宽调制器设置高电平脉宽
		 * @param	width 脉冲宽度，单位us，该值请不要超过脉冲周期
		 * @retval	脉冲宽度，单位us
		 * @ovride	为获得更好性能，派生类请重写该方法
		 */
		virtual void SetPulseWidth(const UINT32 width)
		{
			SetDutyCycle((float)width / GetPulsePeriod());
		}
		virtual UINT32 GetPulseWidth() const
		{
			return GetDutyCycle() * GetPulsePeriod();
		}



		/*
		 * @brief	PWM极性选项
		 */
		enum PWMPOL
		{
			ActiveLow,
			ActiveHigh
		};



		/*
		 * @brief	PWM极性配置
		 * @param	pol 极性，可以是PWMPOL枚举中任何一个
		 *				@arg ActiveLow 成功比较时低电平(LowTruePulse)
		 *				@arg ActiveHigh 成功比较时高电平(HighTruePulse)
		 */
		virtual void SetPolarity(const PWMPOL pol) = 0;




		/*
		 * @brief	PWM对齐模式选项
		 */
		enum PWMALIGN
		{
			EdgeAligned,
			CenterAligned
		};




		virtual ~PulseWidthModulator() {}

	};

}

#endif  /*_PULSEWIDTHMODULATOR_H_*/
