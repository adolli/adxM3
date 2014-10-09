
#ifndef _ADX_APP_DIRECTIONCONTROL_H_
#define _ADX_APP_DIRECTIONCONTROL_H_

#include "adxM3.h"
#include <cmath>
using namespace std;

namespace adxM3
{

	using namespace ACS;


	/*
	 * @breif	方向控制系统
	 *			提供偏差感知和方向角控制，如何控制由用户实现
	 * @tparam	Actuator 执行机构，要求实现以下方法
	 *				@required SetAngle(INT16) 设置角度量，区分正负，0为中轴位
	 *				@required GetAngle() 获取角度量，区分正负，0为中轴位
	 *			DeviationSensor
	 *			{
	 *				DeviationSensorMain
	 *				DeviationSensorSub
	 *				DeviationSensorAux
	 *			} 偏差感应器，要求可获取偏差量
	 *				@required GetResult() 获取偏差采样量
	 *			CornerDetector 直角探测器，要求同DeviationSensor，
	 *				并且其ValueType要和DeviationSensor::ValueType相同
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
		 * @brief	获取当前方向量
		 * @retval	方向角度，单位由DeviationSensor::ValueType决定
		 */
		DirValueType GetCurrentDirection() const
		{
			return Steerer.GetAngle();
		}




		/*
		 * @brief	小车结构性参数（不能等于0）
		 */
		enum
		{
			LEN1 = 100,		// 前后两排电感间距，单位mm
			LEN2 = 125,		// 后排电感到前轮车轴距离
		};





		Actuator& GetDirectionActuator()
		{
			return Steerer;
		}




		/*
		 * @brief	偏差采样，平均数滤波
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

		// 执行机构
		Actuator Steerer;

		// 偏差传感器
		DeviationSensorMain MainSensor;					// 前排双侧90度
		DeviationSensorSub SubSensor;					// 后排双侧90度
		DeviationSensorAux AuxSensor;					// 后排双侧45度

		// 直角探测器
		CornerDetector cornerDetector;

	};







	/*
	 * @brief	传感器对
	 *			定义了一对传感器的结构
	 * @tparam	SensorA A传感器
	 *			SensorB B传感器
	 *				传感器要求实现SensorX::ValueType GetResult()方法，用以获得传感器读数
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
	 * @breif	矩阵方向控制系统
	 *			提供偏差感知和方向角控制，如何控制由用户实现
	 * @tparam	Actuator 执行机构，要求实现以下方法
	 *				@required SetAngle(INT16) 设置角度量，区分正负，0为中轴位
	 *				@required GetAngle() 获取角度量，区分正负，0为中轴位
	 *			DistanceSensorPair
	 *			{
	 *				DeviationSensorMain (L/R)
	 *				DeviationSensorSub (L/R)
	 *				DeviationSensorAux (L/R)
	 *			} 偏差感应器，要求可获取距离量
	 *				@required GetResult() 获取偏差采样量
	 *			CornerDetector (L/R)直角探测器，要求同DeviationSensor，
	 *				并且其ValueType要和DeviationSensor::ValueType相同
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
		 * @brief	获取当前方向量
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
		 * @brief	传感器采样
		 *			并过滤偏差过大的无效数据
		 */
		void Sampling()
		{
			distanceSensor.Sampling(data);
		}


		DistanceValueType data[DistanceSensor::SampleCount];


	protected:

		// 方向执行器
		Actuator steer;

		// 偏差传感器				
		DistanceSensor distanceSensor;


		DirValueType CurrentAngle;
	};



}

#endif  /*_ADX_APP_DIRECTIONCONTROL_H_*/
