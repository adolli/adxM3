
#ifndef _ADX_APP_DIRECTIONCONTROL_H_
#define _ADX_APP_DIRECTIONCONTROL_H_

#include "adxM3.h"
#include <cmath>
using namespace std;

namespace adxM3
{

	using namespace ACS;


	/*
	 * @breif	�������ϵͳ
	 *			�ṩƫ���֪�ͷ���ǿ��ƣ���ο������û�ʵ��
	 * @tparam	Actuator ִ�л�����Ҫ��ʵ�����·���
	 *				@required SetAngle(INT16) ���ýǶ���������������0Ϊ����λ
	 *				@required GetAngle() ��ȡ�Ƕ���������������0Ϊ����λ
	 *			DeviationSensor
	 *			{
	 *				DeviationSensorMain
	 *				DeviationSensorSub
	 *				DeviationSensorAux
	 *			} ƫ���Ӧ����Ҫ��ɻ�ȡƫ����
	 *				@required GetResult() ��ȡƫ�������
	 *			CornerDetector ֱ��̽������Ҫ��ͬDeviationSensor��
	 *				������ValueTypeҪ��DeviationSensor::ValueType��ͬ
	 */
	template
	< 
		class Actuator, 
		class DeviationSensorMain,
		class DeviationSensorSub,
		class DeviationSensorAux,
		class CornerDetector
	>
	class DirectionControlSystem
	{
	public:

		typedef typename Actuator::DirValueType DirValueType;
		typedef typename DeviationSensorMain::ValueType DeviationValueType;
		typedef typename ArithTraits<typename DeviationSensorMain::ValueType>::MaxBitsUnsigned DistanceValueType;
		



		DirectionControlSystem()
		{
		}






		/*
		 * @brief	��ȡ��ǰ������
		 * @retval	����Ƕȣ���λ��DeviationSensor::ValueType����
		 */
		DirValueType GetCurrentDirection() const
		{
			return Steerer.GetAngle();
		}




		/*
		 * @brief	С���ṹ�Բ��������ܵ���0��
		 */
		enum
		{
			LEN1 = 100,		// ǰ�����ŵ�м�࣬��λmm
			LEN2 = 125,		// ���ŵ�е�ǰ�ֳ������
		};





		Actuator& GetDirectionActuator()
		{
			return Steerer;
		}




		/*
		 * @brief	ƫ�������ƽ�����˲�
		 */
		void Sampling()
		{
			enum { SAMPLE_COUNT = 16 };
			
			typename ArithTraits<DeviationValueType>::MaxBitsSigned sample_set[4] = { 0, 0, 0, 0 };
			for (int i = 0; i < SAMPLE_COUNT; ++i)
			{
				sample_set[0] += cornerDetector.GetResult();
				sample_set[1] += MainSensor.GetResult();
				sample_set[2] += SubSensor.GetResult();
				sample_set[3] += AuxSensor.GetResult();
			}
			cornerDeviationValue = sample_set[0] / SAMPLE_COUNT;
			MainDeviationValue = sample_set[1] / SAMPLE_COUNT;
			SubDeviationValue = sample_set[2] / SAMPLE_COUNT;
			AuxDeviationValue = sample_set[3] / SAMPLE_COUNT;
		}


		DeviationValueType MainDeviationValue;
		DeviationValueType SubDeviationValue;
		DeviationValueType AuxDeviationValue;
		DeviationValueType cornerDeviationValue;



	private:

		// ִ�л���
		Actuator Steerer;

		// ƫ�����
		DeviationSensorMain MainSensor;					// ǰ��˫��90��
		DeviationSensorSub SubSensor;					// ����˫��90��
		DeviationSensorAux AuxSensor;					// ����˫��45��

		// ֱ��̽����
		CornerDetector cornerDetector;

	};







	/*
	 * @brief	��������
	 *			������һ�Դ������Ľṹ
	 * @tparam	SensorA A������
	 *			SensorB B������
	 *				������Ҫ��ʵ��SensorX::ValueType GetResult()���������Ի�ô���������
	 *
	 */
	template
	<
		class SensorA,
		class SensorB
	>
	class SensorPair
	{
	public:

		typedef typename SensorA::ValueType ValueType;


		SensorPair()
		{
			STATIC_ASSERT(sizeof(typename SensorA::ValueType) == sizeof(typename SensorB::ValueType),
				sensor_A_and_B_should_have_same_ValueType);
		}


		SensorA A;
		SensorB B;

	};





	/*
	 * @breif	���������ϵͳ
	 *			�ṩƫ���֪�ͷ���ǿ��ƣ���ο������û�ʵ��
	 * @tparam	Actuator ִ�л�����Ҫ��ʵ�����·���
	 *				@required SetAngle(INT16) ���ýǶ���������������0Ϊ����λ
	 *				@required GetAngle() ��ȡ�Ƕ���������������0Ϊ����λ
	 *			DistanceSensorPair
	 *			{
	 *				DeviationSensorMain (L/R)
	 *				DeviationSensorSub (L/R)
	 *				DeviationSensorAux (L/R)
	 *			} ƫ���Ӧ����Ҫ��ɻ�ȡ������
	 *				@required GetResult() ��ȡƫ�������
	 *			CornerDetector (L/R)ֱ��̽������Ҫ��ͬDeviationSensor��
	 *				������ValueTypeҪ��DeviationSensor::ValueType��ͬ
	 */
	template
	< 
		class DistanceSensor,
		class Actuator
	>
	class MatrixDirectionControlSystem
	{
	public:

		enum { DeviationDataSampleCount = DistanceSensor::SampleCount };

		typedef INT16 DirValueType;
		typedef typename DistanceSensor::ValueType DistanceValueType;
		typedef typename ArithTraits<DistanceValueType>::MaxBitsSigned DeviationValueType;
		



		MatrixDirectionControlSystem()
		{

		}






		/*
		 * @brief	��ȡ��ǰ������
		 */
		DirValueType GetCurrentDirection() const
		{
			return CurrentAngle;
		}
		DirValueType GetAngle() const
		{
			return CurrentAngle;
		}



		Actuator& GetDirectionActuator()
		{
			return steer;
		}



		void SetSteeringAngle(DirValueType angle)
		{
			RangeLimit(-100, <=, angle, <=, 100);
			CurrentAngle = angle;

			steer.SetAngle(angle);
		}


		// GZ3348 arr@12.25 



		/*
		 * @brief	����������
		 *			������ƫ��������Ч����
		 */
		void Sampling()
		{
			distanceSensor.Sampling(data);
		}


		DistanceValueType data[DistanceSensor::SampleCount];


	protected:

		// ����ִ����
		Actuator steer;

		// ƫ�����				
		DistanceSensor distanceSensor;


		DirValueType CurrentAngle;
	};



}

#endif  /*_ADX_APP_DIRECTIONCONTROL_H_*/
