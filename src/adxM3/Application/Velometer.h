
#ifndef _ADX_APP_VELOMETER_H_
#define _ADX_APP_VELOMETER_H_

#include "../common/defs.h"

namespace adxM3
{


	/*
	 * @brief	����������
	 * @tparam	CounterType ���������ļ�����Ԫ
	 */
	template
	<
		class CounterType
	>
	class Velometer
		: public CounterType
	{
		

		/*
		 * @required	������Ҫ����ļ�����Ԫʵ�����·���
		 *				GetCounter ��ȡ����������ֵ
		 *				ResetCounter ����������ֵ��0
		 *				GetCounterDirection ��ȡ��������������
		 */
		using CounterType::GetCounter;
		using CounterType::ResetCounter;
		//using CounterType::AddEventListener;
		using CounterType::GetCounterDirection;


		/*
		 * @type	���ļ�����Ԫֵ����
		 */
		typedef typename CounterType::CounterValueType CounterValueType;



		/*
		 * @type	���ļ�����Ԫ������������
		 */
		typedef typename CounterType::DIR CounterDirectionType;


	public:

		/*
		 * @type	DistanceType �������ͣ�����������Ϊ����������ֵ������
		 * @type	VelocityType �ٶ����ͣ�����������Ϊ�������ͳ���ʱ������
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
			// ��ʱ����
			//AddEventListener(Timer::TIME_OUT, DecoderEventListener::OnOverFlow);
		}




		/*
		 * @brief	��ȡ��������ǰ�ٶ�
		 * @retval	��ǰ�ٶ�ֵ
		 * @notice	��ǰ��������ü���йأ����ϸ������趨��periodʱ�������ô˷���
		 *			����������зǳ�������
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
		void Reset()
		{
			distance = 0;
			ResetCounter();
		}
		

	protected:

		// ��������������
		float period;

		// ����������ܺ�
		DistanceType distance;

		// �ϴμ�������ֵ
		CounterValueType lastCounterValue;

		// �������
		int FlowCount;

	};

}

#endif  /*_ADX_APP_VELOMETER_H_*/
