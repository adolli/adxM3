
#ifndef _ADX_CM_HBRIDGEMOTORDRIVER_H_
#define _ADX_CM_HBRIDGEMOTORDRIVER_H_

#include "../common/defs.h"
#include "../../loki/EmptyType.h"
using namespace Loki;



namespace adxM3
{


	template< class T >
	struct ForwardSignal
	{
		typedef T ResultType;
	};


	template< class T >
	struct BackwardSignal
	{
		typedef T ResultType;
	};





	template< class, class T = EmptyType > class HBridgeMotorDriver;
	



	/*
	 * @brief	直流电机驱动器HBridgeMotorDriver
	 *			（双极性驱动方式）
	 * @tparam	ForwardSignalType 前进脉冲信号，要求实现PulseWidthModulator
	 *			BackwardSignalType 后退脉冲信号，要求实现PulseWidthModulator
	 */
	template
	<
		class ForwardPulse, 
		class BackwardPulse
	> 
	class HBridgeMotorDriver
	<
		ForwardSignal<ForwardPulse>,
		BackwardSignal<BackwardPulse>
	>
	{
	public:

		/*
		 * @type	电机速度控制量为PWM占空比
		 */
		typedef float VelocityType;


		HBridgeMotorDriver()
		{
			// 初始无速度
			SetSpeed(0);

			// PWM控制信号使用中央对齐模式，减小纹波
			FW.SetAlignment(FW.CenterAligned);
			BW.SetAlignment(BW.CenterAligned);

			// PWM控制信号输出使能
			FW.EnableOutput();
			BW.EnableOutput();
		}



		/*
		 * @brief	电机设置速度
		 *			双向控制
		 * @param	speed 速度分数，范围[-1,1]，浮点
		 */
		void SetSpeed(VelocityType speed)
		{
			// 限制speed值范围
			RangeLimit(-1, <=, speed, <=, 1);
			
			// 将[-1,1] -> [0,1]
			FW.SetDutyCycle(0.5 + speed / 2);
			BW.SetDutyCycle(0.5 - speed / 2);
		}



		/*
		 * @brief	电机获取速度
		 * @retval	速度分数，范围[0,1]，浮点
		 */
		VelocityType GetSpeed() const
		{
			float FWDuty = FW.GetDutyCycle();

			// 公式 γ = 2 * ρ - 1;
			// 0.5为中心位从[0,1] 线性映射 [-1,1]
			return 2 * FWDuty - 1;
		}



	private:

		ForwardPulse FW;
		BackwardPulse BW;

	};




}

#endif  /*_ADX_CM_HBRIDGEMOTORDRIVER_H_*/
