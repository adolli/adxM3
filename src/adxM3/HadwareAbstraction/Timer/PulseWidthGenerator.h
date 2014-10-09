
#ifndef _ADX_K60_PULSEWIDTHGENERATOR_H_
#define _ADX_K60_PULSEWIDTHGENERATOR_H_

#include "../../common/defs.h"
#include "../GPIO/GeneralPurposeInputOutputPin.h"

namespace adxM3
{



	/*
	 * @brief	可编程宽度脉冲发生器 Programmable Width Pulse Generator
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
	class PulseWidthGenerator
		: public TimerType
	{

	protected:

		using TimerType::AddEventListener;
		using TimerType::RemoveEventListener;
		using TimerType::StartOnce;
		using TimerType::SetTimeOut;
		using TimerType::TimerIsRunning;

	public:


	

		/*
		 * @brief	脉冲极性选项
		 */
		enum PT
		{
			PositivePulse,
			NegativePulse
		};
	
		
		
		
		PulseWidthGenerator(PT pol = PositivePulse)
			: enable(false), lastPulseWidth(0)
		{
			pin.SetDirection(GeneralPurposeInputOutput::OUT);
			pin.OutputConfig(GeneralPurposeInputOutput::PushPull, GeneralPurposeInputOutput::UltraHighSpeed);
			pin.SetDriveStrength(GeneralPurposeInputOutput::HighStrength);
			SetPulsePolarity(pol);

			struct TimerEventListener
			{
				static void OnTimeout(Timer* _This)
				{
					PulseWidthGenerator* This = static_cast<PulseWidthGenerator*>(_This);
					This->pin.Toggle();
				}
			};
			AddEventListener(Timer::TIME_OUT, TimerEventListener::OnTimeout);
		}



		



		/*
		 * @brief	设置脉冲极性
		 * @param	pol 极性，PT可以是PT枚举中的任何一个
		 *				@arg PositivePulse 产生脉冲时电平为1，空闲时电平为0
		 *				@arg NegativePulse 产生脉冲时电平为0，空闲时电平为1
		 */
		void SetPulsePolarity(PT pol)
		{
			STATIC_ASSERT(PositivePulse == 0 && NegativePulse == 1,
				enumerate_value_is_not_campacted_with_this_configuration);

			// 正脉冲的GPIO空闲时为低电平
			pin.SetLevel(pol);
		}





		/*
		 * @brief	产生脉冲
		 * @param	width 脉冲宽度，单位us
		 * @retval	成功产生返回1，否则返回0，若前一次脉冲仍未结束，则为产生失败
		 */
		int GeneratePulse(UINT32 width)
		{
			if (TimerIsRunning() || !enable)
			{
				return 0;
			}

			lastPulseWidth = width;
			SetTimeOut(width);
			pin.Toggle();
			StartOnce();
			return 1;
		}



		/*
		 * @brief	获取上次产生的脉宽
		 */
		UINT32 GetLastPulseWidth() const
		{
			return lastPulseWidth;
		}




		void EnableOutput()
		{
			enable = true;
		}
		void DisableOutput()
		{
			enable = false;
		}


	protected:

		PinType pin;
		bool enable;
		UINT32 lastPulseWidth;

	};

}


#endif  /*_ADX_K60_PWPG_H__*/
