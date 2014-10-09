
#ifndef _ADX_APP_ABSVELOMETER_H_
#define _ADX_APP_ABSVELOMETER_H_

namespace adxM3
{


	/*
	 * @brief	����ʽ������
	 * @tparam	CounterType ������������ͣ�������Ϊ����ʽ��������ֻ�ܶ��������ܸ�λ
	 * @notice	�����Ǿ���ʽ��������Ϊ�˱���ת�˶�Ȧ�޷�����������£�����������Ҫ�Ƚ϶�
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
		 * @required	������Ҫ����ļ�����Ԫʵ�����·�����������
		 *				GetCounter ��ȡ����������ֵ
		 */
		using CounterType::GetCounter;



		typedef typename CounterType::CounterValueType CounterValueType;
		typedef typename ArithTraits<CounterValueType>::MaxBitsSigned SignedCounterValueType;



		/*
		 * @brief	������Ԫ�������ֵ����������ж�
		 *			���������ζ�������ֵ��������ͷ����ж�
		 */
		enum 
		{ 
			PulsePerRound = SignedCounterValueType(CounterType::CounterMaxValue),
			PulseMaxDifference = PulsePerRound / 3,
			PreOverFlowLimit = PulsePerRound - PulseMaxDifference,			// ��������Ҫ����Ԥ��ֵ
			PreUnderFlowLimit = PulseMaxDifference							// ��������Ҫ����Ԥ��ֵ
		};




	public:



		/*
		 * @type	DistanceType �������ͣ�����������Ϊ����������ֵ������
		 * @type	VelocityType �ٶ����ͣ�����������Ϊ�������ͳ���ʱ������
		 */
		typedef int DistanceType;
		typedef float VelocityType;





		AbsVelometer(float _period)
			: period(_period), distance(0)
			, Counter_prev1(0), Counter_prev2(0), Counter_prev3(0)
			, speedDirection(1)
		{	
			// �״�����������ʱ��¼�µ�ǰ�̶�ֵ
			Counter_prev3 = Counter_prev2 = Counter_prev1 = GetCounter();
		}



		/*
		 * @brief	��ȡ��������ǰ�ٶ�
		 * @retval	��ǰ�ٶ�ֵ
		 * @notice	��ǰ��������ü���йأ����ϸ������趨��periodʱ�������ô˷���
		 *			����������зǳ�������
		 */
		VelocityType GetVelocity()
		{
			SignedCounterValueType counted = 0;
			CounterValueType counter = GetCounter();

			// ����ж������������������ֵ�������������������ǰ3�εĲ�
			if (	Counter_prev1 > PreOverFlowLimit && counter < PreUnderFlowLimit
				||	Counter_prev2 > PreOverFlowLimit && Counter_prev1 < PreUnderFlowLimit
				||	Counter_prev3 > PreOverFlowLimit && Counter_prev2 < PreUnderFlowLimit
				)
			{
				// �����ж�
				counted = SignedCounterValueType(counter) - SignedCounterValueType(Counter_prev3) + PulsePerRound;
			}
			else if (counter > PreOverFlowLimit && Counter_prev1 < PreUnderFlowLimit
				||	 Counter_prev1 > PreOverFlowLimit && Counter_prev2 < PreUnderFlowLimit
				||	 Counter_prev2 > PreOverFlowLimit && Counter_prev3 < PreUnderFlowLimit
				)
			{
				// �����ж�
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
		 * @brief	��ȡ��������ǰ����ľ����ܺ�
		 *			���ϴ�Reset��ʼ����
		 * @retval	��ǰ�ٶ�ֵ
		 */
		DistanceType GetDistance() const
		{
			return distance;
		}




		/*
		 * @brief	��λ������
		 *			�������ֵ���ٶȲ���ֵ��0
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

		// ��������������
		float period;

		// �����������
		// ͨ�������������һȦ�����������ϵ�ǰ����̶ȼ��ɵó��ܾ���
		int distance;

	private:

		// �����ϴμ�����ֵ����������ó�λ��
		CounterValueType Counter_prev1;
		CounterValueType Counter_prev2;
		CounterValueType Counter_prev3;

		// ʹ�ÿ������˲�����Ϊ�����˲���
		FLT::KalmanFilter<SignedCounterValueType, 0.1> samplingFilter;

		// �ٶȷ��򣬵�Ч�ڴ���һ�����򻷽�
		int speedDirection;
	};


}

#endif  /*_ADX_APP_ABSVELOMETER_H_*/
