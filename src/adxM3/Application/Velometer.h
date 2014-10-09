
#ifndef _ADX_APP_VELOMETER_H_
#define _ADX_APP_VELOMETER_H_

#include "../common/defs.h"

namespace adxM3
{


	/*
	 * @brief	测速器抽象
	 * @tparam	CounterType 测速器核心计数单元
	 */
	template
	<
		class CounterType
	>
	class Velometer
		: public CounterType
	{
		

		/*
		 * @required	测速器要求核心计数单元实现以下方法
		 *				GetCounter 获取计数器计数值
		 *				ResetCounter 计数器计数值清0
		 *				GetCounterDirection 获取计数器计数方向
		 */
		using CounterType::GetCounter;
		using CounterType::ResetCounter;
		//using CounterType::AddEventListener;
		using CounterType::GetCounterDirection;


		/*
		 * @type	核心计数单元值类型
		 */
		typedef typename CounterType::CounterValueType CounterValueType;



		/*
		 * @type	核心计数单元计数方向类型
		 */
		typedef typename CounterType::DIR CounterDirectionType;


	public:

		/*
		 * @type	DistanceType 距离类型，区分正负，为计数器计数值和类型
		 * @type	VelocityType 速度类型，区分正负，为距离类型除以时间类型
		 */
		typedef typename ArithTraits<CounterValueType>::MaxBitsSigned DistanceType;
		typedef float VelocityType;
		

	

		Velometer(float _period)
			: period(_period), lastCounterValue(0), FlowCount(0)
		{
			struct DecoderEventListener
			{
				static void OnOverFlow(void* This)
				{
					Velometer* ThisCounter = static_cast<Velometer*>(This);

					if (ThisCounter->GetFlowDirection() == CounterType::OverFlow)
					{
						++ThisCounter->FlowCount;
					}
					else
					{
						--ThisCounter->FlowCount;
					}
					
				}
			};
			// 暂时保留
			//AddEventListener(Timer::TIME_OUT, DecoderEventListener::OnOverFlow);
		}




		/*
		 * @brief	获取测速器当前速度
		 * @retval	当前速度值
		 * @notice	当前方法与调用间隔有关，请严格按照所设定的period时间间隔调用此方法
		 *			否则测量会有非常大的误差
		 */
		VelocityType GetVelocity()
		{
			CounterValueType counter = GetCounter();
			ResetCounter();			

			distance += counter;
			VelocityType speed = ((float)lastCounterValue * 0.3 + (float)counter * 0.7) / period;
			lastCounterValue = counter;

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
		void Reset()
		{
			distance = 0;
			ResetCounter();
		}
		

	protected:

		// 测速器采样周期
		float period;

		// 测速器测距总和
		DistanceType distance;

		// 上次计数器的值
		CounterValueType lastCounterValue;

		// 溢出计数
		int FlowCount;

	};

}

#endif  /*_ADX_APP_VELOMETER_H_*/
