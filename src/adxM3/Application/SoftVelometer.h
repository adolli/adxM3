
#ifndef _ADX_APP_SOFTVELOMETER_H_
#define _ADX_APP_SOFTVELOMETER_H_

#include "../common/defs.h"

namespace adxM3
{




	/*
	 * @brief	�����������
	 *			ʹ��һ���������������������ʶ���źŹ��������
	 *			Ϊ�˽�ʡ��Դ���ݲ�ʹ��DirectionIdentifier�ĵ�ƽ�仯��������
	 *			ֻҪ����Ƶ�����ٶ�0�������������л���������ɽϴ����
	 * @tparam	CounterType ���������ļ�����Ԫ
	 *			DirectionIdentifier ����ʶ������ΪGPIO���Ż�EXTI����
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
		 * @required	������Ҫ����ļ�����Ԫʵ�����·���
		 *				GetCounter ��ȡ����������ֵ
		 *				ResetCounter ����������ֵ��0
		 */
		using CounterType::GetCounter;
		using CounterType::ResetCounter;




		/*
		 * @required	����ʶ����Ҫ��ʵ�����·���
		 *				SetDirection ���ö˿�IO����
		 *				InputConfig �˿�����ģʽ����
		 */
		using DirectionIdentifier::InputConfig;
		using DirectionIdentifier::GetLevel;



		typedef typename CounterType::CounterValueType CounterValueType;
		typedef typename ArithTraits<CounterValueType>::MaxBitsSigned SignedCounterValueType;

	public:


		/*
		 * @type	DistanceType �������ͣ�����������Ϊ����������ֵ������
		 * @type	VelocityType �ٶ����ͣ�����������Ϊ�������ͳ���ʱ������
		 */
		typedef int DistanceType;
		typedef float VelocityType;
		

	

		SoftVelometer(float _period)
			: CurrentVelocityDirention(FORWARD), period(_period), distance(0), lastCounterValue(0)
		{
			// Ĭ����������
			InputConfig(GeneralPurposeInputOutput::PullUp);
		}




		/*
		 * @brief	��ȡ��������ǰ�ٶ�
		 * @retval	��ǰ�ٶ�ֵ
		 * @notice	��ǰ��������ü���йأ����ϸ������趨��periodʱ�������ô˷���
		 *			����������зǳ�������
		 */
		VelocityType GetVelocity()
		{
			// ���̶�ȡ����ֵ����ռĴ����������ڴ���֮������м�����ֵ�仯��ɲ������
			SignedCounterValueType counted = GetCounter();
			ResetCounter();

			// ��ȡ��ǰ��ƽ
			CurrentVelocityDirention = GetLevel() == 0 ? BACKWARD : FORWARD;
			counted *= CurrentVelocityDirention;
			
			// �ۼӾ���
			distance += counted;

			// ����ٶȺ���ո����ڼ���ֵ
			// Ϊ��ƥ������������������һ����ر仯������м��������ԣ������������������4
			VelocityType speed = ((float)lastCounterValue * 0.3 + (float)counted * 0.7) * 4 / period;
			lastCounterValue = counted;

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
			ResetCounter();
		}
		


		/*
		 * @brief	�ٶȷ���ѡ��
		 */
		enum VDIR
		{
			FORWARD		= 1,
			BACKWARD	= -1
		};


	protected:


		// ��ǰ�ٶȷ���1Ϊ������-1Ϊ������
		VDIR CurrentVelocityDirention;

		// ��������������
		float period;

		// ����������ܺ�
		DistanceType distance;


		SignedCounterValueType lastCounterValue;

	};

}

#endif  /*_ADX_APP_SOFTVELOMETER_H_*/
