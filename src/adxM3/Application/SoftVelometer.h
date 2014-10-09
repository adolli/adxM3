
#ifndef _ADX_APP_SOFTVELOMETER_H_
#define _ADX_APP_SOFTVELOMETER_H_

#include "../common/defs.h"

namespace adxM3
{




	/*
	 * @brief	软测速器抽象
	 *			使用一个脉冲计数器和正反计数识别信号构造测速器
	 *			为了节省资源，暂不使用DirectionIdentifier的电平变化监听功能
	 *			只要不是频繁在速度0附近正反方向切换，不会造成较大误差
	 * @tparam	CounterType 测速器核心计数单元
	 *			DirectionIdentifier 方向识别器，为GPIO引脚或EXTI类型
	 */
	template
	<
		class CounterType,
		class DirectionIdentifier
	>
	class SoftVelometer
		: public CounterType
		, protected DirectionIdentifier
	{
	protected:

		/*
		 * @required	测速器要求核心计数单元实现以下方法
		 *				GetCounter 获取计数器计数值
		 *				ResetCounter 计数器计数值清0
		 */
		using CounterType::GetCounter;
		using CounterType::ResetCounter;




		/*
		 * @required	方向识别器要求实现以下方法
		 *				SetDirection 设置端口IO方向
		 *				InputConfig 端口输入模式配置
		 */
		using DirectionIdentifier::InputConfig;
		using DirectionIdentifier::GetLevel;



		typedef typename CounterType::CounterValueType CounterValueType;
		typedef typename ArithTraits<CounterValueType>::MaxBitsSigned SignedCounterValueType;

	public:


		/*
		 * @type	DistanceType 距离类型，区分正负，为计数器计数值和类型
		 * @type	VelocityType 速度类型，区分正负，为距离类型除以时间类型
		 */
		typedef int DistanceType;
		typedef float VelocityType;
		

	

		SoftVelometer(float _period)
			: CurrentVelocityDirention(FORWARD), period(_period), distance(0), lastCounterValue(0)
		{
			// 默认上拉电阻
			InputConfig(GeneralPurposeInputOutput::PullUp);
		}




		/*
		 * @brief	获取测速器当前速度
		 * @retval	当前速度值
		 * @notice	当前方法与调用间隔有关，请严格按照所设定的period时间间隔调用此方法
		 *			否则测量会有非常大的误差
		 */
		VelocityType GetVelocity()
		{
			// 立刻读取计数值并清空寄存器，以免在处理之后代码中计数器值变化造成测量误差
			SignedCounterValueType counted = GetCounter();
			ResetCounter();

			// 获取当前电平
			CurrentVelocityDirention = GetLevel() == 0 ? BACKWARD : FORWARD;
			counted *= CurrentVelocityDirention;
			
			// 累加距离
			distance += counted;

			// 算出速度后清空该周期计数值
			// 为了匹配正交解码器在任意一相边沿变化都会进行计数的特性，故脉冲计数需额外乘以4
			VelocityType speed = ((float)lastCounterValue * 0.3 + (float)counted * 0.7) * 4 / period;
			lastCounterValue = counted;

			return speed;
		}



		/*
		 * @brief	获取测速器当前测过的距离总和
		 *			从上次Reset后开始算起
		 * @retval	当前速度值
		 */
		DistanceType GetDistance() const
		{
			return distance;
		}




		/*
		 * @brief	复位测速器
		 *			距离测量值与速度测量值清0
		 */
		virtual void Reset()
		{
			distance = 0;
			ResetCounter();
		}
		


		/*
		 * @brief	速度方向选项
		 */
		enum VDIR
		{
			FORWARD		= 1,
			BACKWARD	= -1
		};


	protected:


		// 当前速度方向，1为正方向，-1为反方向
		VDIR CurrentVelocityDirention;

		// 测速器采样周期
		float period;

		// 测速器测距总和
		DistanceType distance;


		SignedCounterValueType lastCounterValue;

	};

}

#endif  /*_ADX_APP_SOFTVELOMETER_H_*/
