
#ifndef _ADX_APP_ABSVELOMETER_H_
#define _ADX_APP_ABSVELOMETER_H_

namespace adxM3
{


	/*
	 * @brief	绝对式测速器
	 * @tparam	CounterType 编码计数器类型，该类型为绝对式计数器，只能读数，不能复位
	 * @notice	由于是绝对式编码器，为了避免转了多圈无法计数的情况下，采样周期需要比较短
	 */
	template
	<
		class CounterType
	>
	class AbsVelometer
		: public CounterType
	{
	protected:
		
		/*
		 * @required	测速器要求核心计数单元实现以下方法（保护）
		 *				GetCounter 获取计数器计数值
		 */
		using CounterType::GetCounter;



		typedef typename CounterType::CounterValueType CounterValueType;
		typedef typename ArithTraits<CounterValueType>::MaxBitsSigned SignedCounterValueType;



		/*
		 * @brief	计数单元计数最大值，用于溢出判断
		 *			计数器两次读数最大差值用于溢出和方向判断
		 */
		enum 
		{ 
			PulsePerRound = SignedCounterValueType(CounterType::CounterMaxValue),
			PulseMaxDifference = PulsePerRound / 3,
			PreOverFlowLimit = PulsePerRound - PulseMaxDifference,			// 计数器快要上溢预判值
			PreUnderFlowLimit = PulseMaxDifference							// 计数器快要下溢预判值
		};




	public:



		/*
		 * @type	DistanceType 距离类型，区分正负，为计数器计数值和类型
		 * @type	VelocityType 速度类型，区分正负，为距离类型除以时间类型
		 */
		typedef int DistanceType;
		typedef float VelocityType;





		AbsVelometer(float _period)
			: period(_period), distance(0)
			, Counter_prev1(0), Counter_prev2(0), Counter_prev3(0)
			, speedDirection(1)
		{	
			// 首次启动编码器时记录下当前刻度值
			Counter_prev3 = Counter_prev2 = Counter_prev1 = GetCounter();
		}



		/*
		 * @brief	获取测速器当前速度
		 * @retval	当前速度值
		 * @notice	当前方法与调用间隔有关，请严格按照所设定的period时间间隔调用此方法
		 *			否则测量会有非常大的误差
		 */
		VelocityType GetVelocity()
		{
			SignedCounterValueType counted = 0;
			CounterValueType counter = GetCounter();

			// 溢出判定用连续两侧读回来的值，计算计数的脉冲用与前3次的差
			if (	Counter_prev1 > PreOverFlowLimit && counter < PreUnderFlowLimit
				||	Counter_prev2 > PreOverFlowLimit && Counter_prev1 < PreUnderFlowLimit
				||	Counter_prev3 > PreOverFlowLimit && Counter_prev2 < PreUnderFlowLimit
				)
			{
				// 上溢判定
				counted = SignedCounterValueType(counter) - SignedCounterValueType(Counter_prev3) + PulsePerRound;
			}
			else if (counter > PreOverFlowLimit && Counter_prev1 < PreUnderFlowLimit
				||	 Counter_prev1 > PreOverFlowLimit && Counter_prev2 < PreUnderFlowLimit
				||	 Counter_prev2 > PreOverFlowLimit && Counter_prev3 < PreUnderFlowLimit
				)
			{
				// 下溢判定
				counted = SignedCounterValueType(counter) - SignedCounterValueType(Counter_prev3) - PulsePerRound;
			}
			else
			{
				counted = SignedCounterValueType(counter) - SignedCounterValueType(Counter_prev3);
			}
			Counter_prev3 = Counter_prev2;
			Counter_prev2 = Counter_prev1;
			Counter_prev1 = counter;

			VelocityType speed = samplingFilter(counted) / period / 3 * speedDirection;
			distance += speedDirection * counted;

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
		}



		void ReverseSpeedEncoderDirection()
		{
			speedDirection *= -1;
		}




	protected:

		// 测速器采样周期
		float period;

		// 上溢次数计数
		// 通过溢出次数乘以一圈的脉冲数加上当前脉冲刻度即可得出总距离
		int distance;

	private:

		// 保存上次计数器值，两次作差得出位移
		CounterValueType Counter_prev1;
		CounterValueType Counter_prev2;
		CounterValueType Counter_prev3;

		// 使用卡尔曼滤波器作为采样滤波器
		FLT::KalmanFilter<SignedCounterValueType, 0.1> samplingFilter;

		// 速度方向，等效于串联一个反向环节
		int speedDirection;
	};


}

#endif  /*_ADX_APP_ABSVELOMETER_H_*/
