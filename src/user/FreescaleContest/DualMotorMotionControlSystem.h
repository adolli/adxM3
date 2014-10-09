
 
#ifndef _ADX_APP_DUALMOTORMOTIONCONTROLSYSTEM_H_
#define _ADX_APP_DUALMOTORMOTIONCONTROLSYSTEM_H_

#include "adxM3.h"

namespace adxM3
{
	
	
	// ʹ���Զ�����ϵͳ
	using namespace ACS;




	/*
	 * @brief	��������ϵͳ
	 *			�ջ��ٶȿ���
	 * 			MotorDriverҪ��ʵ�����·���
	 * 				@required SetSpeed(MotorDriver::VelocityType) ���õ���ٶȿ���������ֵ
	 * 				@required MotorDriver::VelocityType GetSpeed() ��ȡ����ٶȿ���������ֵ
	 * 			VelometerҪ��ʵ�����·���
	 * 				@required Velometer::DistanceType GetCounter() ��ȡ�������������ֵ
	 * 				@required ResetCounter() ����������ֵ����
	 */
	template
	< 
		class MotorDriver,
		class Velometer
	>
	class PowerControlSystem
	{
	public:

		typedef typename Velometer::DistanceType VelometerDistanceType;
		typedef typename Velometer::VelocityType VelometerVelocityType;
		typedef typename MotorDriver::VelocityType MotorVelocityType;





		/*
		 * @brief	�����ٶ�
		 * @param	speed �ٶȷ�������Χ[-1,1]
		 */
		void SetSpeed(MotorVelocityType speed)
		{
			RangeLimit(-1, <=, speed, <=, 1);

			// ���浱ǰ����ٶȿ������趨ֵ�����Get������Ч��
			CurrentSettingSpeed = speed;
			Motor.SetSpeed(speed);
		}



		/*
		 * @brief	��ȡ�ٶ�
		 */
		MotorVelocityType GetSpeed() const
		{
			return CurrentSettingSpeed;
		}


		

		/*
		 * @brief	����ϵͳ���캯��
		 * @param	period ϵͳ�������ڣ���λs
		 */
		PowerControlSystem(float period)
			: SpeedRegulator(period), PositionRegulator(period)
			, SpeedEncoder(period), CurrentSettingSpeed(0.0)
		{
		}




		/*
		 * @brief	����ϵͳ����װ�������������뻻�㳣��
		 */
		enum
		{
			//CountPerMeter = 19825		// 500 ��������������С��ǰ��һ���������ֵ
			CountPerMeter = 57872		// 4096 �߾���ʽ��������С��ǰ��һ���������ֵ
		};



		/*
		 * @brief	��ȡ�����ٶ�ֵ
		 * @retval	����ϵͳ�ٶ�ֵ����λ����ÿ��
		 */
		VelometerVelocityType GetSpeedEncoderSpeed()
		{
			CurrentVelocityMeasured = SpeedEncoder.GetVelocity() * 1000 / CountPerMeter;
			return CurrentVelocityMeasured;
		}

		Velometer& GetSpeedEncoder()
		{
			return SpeedEncoder;
		}
		MotorDriver& debug_GetMotor()
		{
			return Motor;
		}
		
		
		
		/*
		 * @brief	��ȡ��ǰ�н�����
		 * @retval	�����������ֵ���޵�λ
		 */
		VelometerDistanceType GetDistance()
		{
			return SpeedEncoder.GetDistance();
		}





		/*
		 * @brief	�ٶȵ��������ݺ���
		 */
		void ExcuteTransferFunction()
		{
			// λ�÷������û���һ��δʹ�ܣ�
			//VelometerDistanceType dist = GetDistance();
			//VelometerDistanceType deltaTargetSpeed = PositionRegulator.DeltaClosedLoopGain(dist);
			//SpeedRegulator.TargetValue += deltaTargetSpeed;

			// �ٶȷ���
			VelometerVelocityType speed = GetSpeedEncoderSpeed();

			// �ٶ���������
			if (abs(speed - SpeedRegulator.TargetValue) < 5)
			{
				speed = SpeedRegulator.TargetValue;
			}
			
			// �ٶȵ������ıջ����滷��Ҫִ�У����ڴӻ��ڶ�ȡ�ٶ�ֵ
			MotorVelocityType deltaSpeed = SpeedRegulator.DeltaClosedLoopGain(speed);
			SetSpeed(GetSpeed() + deltaSpeed);
		}




		/*
		 * @brief	ʹ��/��ֹ�ٶȵ�����
		 */
		void EnableSpeedRegulator()
		{
			SpeedRegulator.OutputEnable();
		}
		void DisableSpeedRegulator()
		{
			SpeedRegulator.OutputDisable();
		}

		



		// PID �ٶȵ�����
		PIDElement<MotorVelocityType, VelometerVelocityType, UsingKiKd> SpeedRegulator;
		


		// PID λ�õ�����
		UniformizedPIDElement<VelometerDistanceType, VelometerDistanceType> PositionRegulator;


		// ���浱ǰ�������ٶ�ֵ
		volatile VelometerVelocityType CurrentVelocityMeasured;


	protected:
	public:


		// ���
		MotorDriver Motor;

		// �ٶȱ�����
		Velometer SpeedEncoder;


		// ��ֵΪ��ǰ�趨ֵ������ʵʱֵ
		// ��ΪPWM�����ȸ��»�Ҫ�ȴ����һ����������
		volatile MotorVelocityType CurrentSettingSpeed;


	};






	/*
	 * @brief	˫�������ϵͳ��ͬ������
	 * @tparam	LeftPCU �������Ƶ�Ԫ��Ҫ��ʵ�����·���
	 *				@required Forward() ��ת
	 *				@required Backward() ��ת
	 *				@required Brake() �ƶ�
	 *				@required SetSpeed(UINT8) �����ٶȣ��ٷ��ٶ�ֵ��
	 *				@required GetSpeed(UINT8) ��ȡ�ٶȣ��ٷ��ٶ�ֵ��
	 *			RightPCU �������Ƶ�Ԫ��Ҫ��ʵ����������ͬ
	 */
	template
	< 
		class LeftPCU, 
		class RightPCU
	>
	class AxisSynchronouslyControl
	{
	public:
		
		typedef typename LeftPCU::VelometerDistanceType VelometerDistanceType;
		typedef typename LeftPCU::VelometerVelocityType VelometerVelocityType;
		typedef typename LeftPCU::MotorVelocityType MotorVelocityType;
		typedef MotorVelocityType SpeedType;





		/*
		 * @brief	ͬ����ƹ��캯��
		 * @param	period ϵͳ�������ڣ���λs
		 */
		AxisSynchronouslyControl(float period)
			: LeftPowerCtrlUnit(period), RightPowerCtrlUnit(period)
		{
		}


		



		/*
		 * @brief	�ƶ�
		 */
		void Brake()
		{
			SetTargetSpeed(0);
			//while (abs(LeftPowerCtrlUnit.SpeedRegulator.GetCurrentValue()) > 50);
			//LeftPowerCtrlUnit.SpeedRegulator.OutputDisable();
			//RightPowerCtrlUnit.SpeedRegulator.OutputDisable();
			LeftPowerCtrlUnit.Brake();
			RightPowerCtrlUnit.Brake();
		}



		/*
		 * @brief	�����ٶ�
		 * @param	speed_mmps �ٶȣ���λ����ÿ��
		 */
		void SetTargetSpeed(SpeedType speedL_mmps, SpeedType speedR_mmps = -1)
		{
			LeftPowerCtrlUnit.SpeedRegulator.SetTargetValue(speedL_mmps);
			RightPowerCtrlUnit.SpeedRegulator.SetTargetValue(speedL_mmps);
			//LeftPowerCtrlUnit.SpeedRegulator.OutputEnable();
			//RightPowerCtrlUnit.SpeedRegulator.OutputEnable();
		}



		/*
		 * @brief	��ȡ�ٶȣ���λmm/s
		 */
		SpeedType GetCurrentSpeed() const
		{
			// ˫��ͬ�٣�ֻȡ��һ
			return LeftPowerCtrlUnit.SpeedRegulator.GetCurrentValue();
		}



		/*
		 * @brief	���˻ع����˻����
		 */
		void debug_LoopBack()
		{
			LeftPowerCtrlUnit.PositionRegulator.OutputEnable();
			RightPowerCtrlUnit.PositionRegulator.OutputEnable();
			while (abs(LeftPowerCtrlUnit.GetDistance()) > 150 
				|| abs(LeftPowerCtrlUnit.SpeedRegulator.GetCurrentValue()) > 50);
			LeftPowerCtrlUnit.PositionRegulator.OutputDisable();
			RightPowerCtrlUnit.PositionRegulator.OutputDisable();
			Brake();
		}



		/*
		 * @brief	�ٶȵ��������ݺ���
		 */
		void SpeedRegulatorTransferFunction()
		{
			LeftPowerCtrlUnit.ExcuteTransferFunction();
			RightPowerCtrlUnit.ExcuteTransferFunction();
		}


	
		// �������Ƶ�Ԫ
		LeftPCU LeftPowerCtrlUnit;
		RightPCU RightPowerCtrlUnit;


	};







	
	/*
	 * @brief	˫�������ϵͳ���첽����
	 * @tparam	LeftPCU �������Ƶ�Ԫ��Ҫ��ʵ�����·���
	 *				@required Forward() ��ת
	 *				@required Backward() ��ת
	 *				@required Brake() �ƶ�
	 *				@required SetSpeed(UINT8) �����ٶȣ��ٷ��ٶ�ֵ��
	 *				@required GetSpeed(UINT8) ��ȡ�ٶȣ��ٷ��ٶ�ֵ��
	 *			RightPCU �������Ƶ�Ԫ��Ҫ��ʵ����������ͬ
	 */
	template
	< 
		class LeftPCU, 
		class RightPCU
	>
	class AxisAsynchronouslyControl
	{
	public:


		typedef typename LeftPCU::VelometerDistanceType VelometerDistanceType;
		typedef typename LeftPCU::VelometerVelocityType VelometerVelocityType;
		typedef typename LeftPCU::MotorVelocityType MotorVelocityType;
		typedef MotorVelocityType SpeedType;





		/*
		 * @brief	ͬ����ƹ��캯��
		 * @param	period ϵͳ�������ڣ���λs
		 */
		AxisAsynchronouslyControl(float period)
			: LeftPowerCtrlUnit(period), RightPowerCtrlUnit(period)
		{
		}


		



		/*
		 * @brief	�ƶ�
		 */
		void Brake()
		{
			SetTargetSpeed(0, 0);
		}



		/*
		 * @brief	�����ٶ�
		 * @param	speed_mmps �ٶȣ���λ����ÿ��
		 */
		void SetTargetSpeed(SpeedType speedL_mmps, SpeedType speedR_mmps)
		{
			LeftPowerCtrlUnit.SpeedRegulator.SetTargetValue(speedL_mmps);
			RightPowerCtrlUnit.SpeedRegulator.SetTargetValue(speedR_mmps);
		}



		/*
		 * @brief	��ȡ�����õ�ƽ���ٶȣ��������ϵ�ƽ��ֵ
		 * @retval	�ٶȣ���λ����ÿ��
		 */
		SpeedType GetAvgTargetSpeed() const
		{
			return (LeftPowerCtrlUnit.SpeedRegulator.TargetValue 
				+  RightPowerCtrlUnit.SpeedRegulator.TargetValue) / 2;
		}




		/*
		 * @brief	��ȡ�ٶȣ�������ƽ��ֵ����λmm/s
		 */
		SpeedType GetCurrentSpeed() const
		{
			return (LeftPowerCtrlUnit.SpeedRegulator.GetCurrentValue() 
				+  RightPowerCtrlUnit.SpeedRegulator.GetCurrentValue()) / 2;
		}




		/*
		 * @brief	���˻ع����˻����
		 */
		void debug_LoopBack()
		{
			LeftPowerCtrlUnit.PositionRegulator.OutputEnable();
			RightPowerCtrlUnit.PositionRegulator.OutputEnable();
			while (abs(LeftPowerCtrlUnit.GetDistance()) > 150 && abs(RightPowerCtrlUnit.GetDistance()) > 150 
				|| abs(LeftPowerCtrlUnit.SpeedRegulator.GetCurrentValue()) > 50);
			LeftPowerCtrlUnit.PositionRegulator.OutputDisable();
			RightPowerCtrlUnit.PositionRegulator.OutputDisable();
			Brake();
		}

	


		/*
		 * @brief	ʹ��/��ֹ�Զ��ٶȿ���
		 *			�ջ��ٶȵ��������ٿ���
		 */
		void EnableAutoSpeedControl()
		{
			LeftPowerCtrlUnit.EnableSpeedRegulator();
			RightPowerCtrlUnit.EnableSpeedRegulator();
		}
		void DisableAutoSpeedControl()
		{
			LeftPowerCtrlUnit.DisableSpeedRegulator();
			RightPowerCtrlUnit.DisableSpeedRegulator();
		}




		/*
		 * @brief	�ٶȵ��������ݺ���
		 */
		void SpeedRegulatorTransferFunction()
		{
			LeftPowerCtrlUnit.ExcuteTransferFunction();
			RightPowerCtrlUnit.ExcuteTransferFunction();
		}




		// �������Ƶ�Ԫ
		LeftPCU LeftPowerCtrlUnit;
		RightPCU RightPowerCtrlUnit;



	private:


	};










	/*
	 * @brief	˫�������ʽ���ٶ���ϵͳ���첽����
	 * @tparam	LeftPCU �������Ƶ�Ԫ��Ҫ��ʵ�����·���
	 *				@required Forward() ��ת
	 *				@required Backward() ��ת
	 *				@required Brake() �ƶ�
	 *				@required SetSpeed(UINT8) �����ٶȣ��ٷ��ٶ�ֵ��
	 *				@required GetSpeed(UINT8) ��ȡ�ٶȣ��ٷ��ٶ�ֵ��
	 *			RightPCU �������Ƶ�Ԫ��Ҫ��ʵ����������ͬ
	 */
	template
	< 
		class LeftPCU, 
		class RightPCU
	>
	class AxisPassivelyControl
	{
	public:


		typedef typename LeftPCU::VelometerDistanceType VelometerDistanceType;
		typedef typename LeftPCU::VelometerVelocityType VelometerVelocityType;
		typedef typename LeftPCU::MotorVelocityType MotorVelocityType;
		typedef MotorVelocityType SpeedType;





		/*
		 * @brief	ͬ����ƹ��캯��
		 * @param	period ϵͳ�������ڣ���λs
		 */
		AxisPassivelyControl(float period)
			: UnionSpeedRegulator(period), LeftPowerCtrlUnit(period), RightPowerCtrlUnit(period)
		{
		}


		



		/*
		 * @brief	�ƶ�
		 */
		void Brake()
		{
			SetTargetSpeed(0, 0);
		}



		/*
		 * @brief	�����ٶ�
		 * @param	speed_mmps �ٶȣ���λ����ÿ��
		 */
		void SetTargetSpeed(SpeedType speedL_mmps, SpeedType speedR_mmps)
		{
			UnionSpeedRegulator.SetTargetValue((speedL_mmps + speedR_mmps) / 2);
		}



		/*
		 * @brief	��ȡ�����õ�ƽ���ٶȣ��������ϵ�ƽ��ֵ
		 * @retval	�ٶȣ���λ����ÿ��
		 */
		SpeedType GetAvgTargetSpeed() const
		{
			return UnionSpeedRegulator.TargetValue;
		}




		/*
		 * @brief	��ȡ�ٶȣ�������ƽ��ֵ����λmm/s
		 */
		SpeedType GetCurrentSpeed() const
		{
			return UnionSpeedRegulator.GetCurrentValue();
		}




		/*
		 * @brief	���˻ع����˻����
		 */
		void debug_LoopBack()
		{
		}

	


		/*
		 * @brief	ʹ��/��ֹ�Զ��ٶȿ���
		 *			�ջ��ٶȵ��������ٿ���
		 */
		void EnableAutoSpeedControl()
		{
			UnionSpeedRegulator.OutputEnable();
		}
		void DisableAutoSpeedControl()
		{
			UnionSpeedRegulator.OutputDisable();
		}






		/*
		 * @brief	�ٶȵ��������ݺ���
		 */
		void SpeedRegulatorTransferFunction()
		{
			// �ٶȷ���
			VelometerVelocityType velocityL = LeftPowerCtrlUnit.GetSpeedEncoderSpeed();
			VelometerVelocityType velocityR = RightPowerCtrlUnit.GetSpeedEncoderSpeed();
			VelometerVelocityType avgVelocity = (velocityL + velocityR) / 2; 

			
			// �ٶ���������
			if (abs(avgVelocity - UnionSpeedRegulator.TargetValue) < 5)
			{
				avgVelocity = UnionSpeedRegulator.TargetValue;
			}

			// �ٶȵ������ıջ����滷��Ҫִ�У����ڴӻ��ڶ�ȡ�ٶ�ֵ
			MotorVelocityType deltaSpeed_pwm = UnionSpeedRegulator.DeltaClosedLoopGain(avgVelocity);

			if (abs(velocityR - velocityL) / abs(avgVelocity) > 0.438)
			{
				return;
			}

			// ִ���Զ����ٵ���		
			//float chasu = 1;
			float chasu2 = 0.000001;

			//if (deltaSpeed_pwm >= 0)
			{
				if (velocityL - velocityR > 20)
				{
					LeftPowerCtrlUnit.SetSpeed(LeftPowerCtrlUnit.GetSpeed() + deltaSpeed_pwm);

					MotorVelocityType motorRSpdCalc = RightPowerCtrlUnit.GetSpeed() + deltaSpeed_pwm - (avgVelocity - velocityR) * chasu2;
					RightPowerCtrlUnit.SetSpeed(motorRSpdCalc);
				}
				else if (velocityL - velocityR < -20)
				{
					RightPowerCtrlUnit.SetSpeed(RightPowerCtrlUnit.GetSpeed() + deltaSpeed_pwm);

					MotorVelocityType motorLSpdCalc = LeftPowerCtrlUnit.GetSpeed() + deltaSpeed_pwm - (avgVelocity - velocityL) * chasu2;
					LeftPowerCtrlUnit.SetSpeed(motorLSpdCalc);
				}
				else
				{
					LeftPowerCtrlUnit.SetSpeed(LeftPowerCtrlUnit.GetSpeed() + deltaSpeed_pwm);
					RightPowerCtrlUnit.SetSpeed(RightPowerCtrlUnit.GetSpeed() + deltaSpeed_pwm);
				}
			}
			/*else
			{
				if ()
				{
				}
			}*/
		}



		// �����ٶȵ�����
		PIDElement<MotorVelocityType, VelometerVelocityType, UsingKiKd> UnionSpeedRegulator;



		// �������Ƶ�Ԫ
		LeftPCU LeftPowerCtrlUnit;
		RightPCU RightPowerCtrlUnit;



	private:



	};













	/*
	 * @brief	˫�������ϵͳ
	 * @tparam	LeftMotor ������Ҫ��ʵ�����·���
	 *				@required Forward() ��ת
	 *				@required Backward() ��ת
	 *				@required Brake() �ƶ�
	 *				@required SetSpeed(UINT8) �����ٶȣ��ٷ��ٶ�ֵ��
	 *				@required GetSpeed(UINT8) ��ȡ�ٶȣ��ٷ��ٶ�ֵ��
	 *			RightMotor �ҵ����Ҫ��ʵ����������ͬ
	 *			AxisSyncPolicy ��ͬ������
	 */
	template
	<
		class LPCU,
		class RPCU,
		template< class, class > class AxisSyncPolicy = AxisAsynchronouslyControl
	>
	class DualMotorMotionControlSystem
		: public AxisSyncPolicy<LPCU, RPCU>
	{
	public:


		/*
		 * @required	Ҫ��AxisSyncPolicy������������
		 *					@type VelometerDistanceType ���ټƾ���ֵ����
		 *					@type VelometerVelocityType ���ټ��ٶ�ֵ����
		 *					@type MotorVelocityType ����ٶȿ���������
		 */
		typedef typename AxisSyncPolicy<LPCU, RPCU>::VelometerDistanceType VelometerDistanceType;
		typedef typename AxisSyncPolicy<LPCU, RPCU>::VelometerVelocityType VelometerVelocityType;
		typedef typename AxisSyncPolicy<LPCU, RPCU>::MotorVelocityType MotorVelocityType;



		/*
		 * @required	Ҫ��AxisSyncPolicy�����������ݳ�Ա
		 *					@data LPCU LeftPowerCtrlUnit �������Ƶ�Ԫ
		 *					@data RPCU RightPowerCtrlUnit �Ҷ������Ƶ�Ԫ
		 */
		using AxisSyncPolicy<LPCU, RPCU>::LeftPowerCtrlUnit;
		using AxisSyncPolicy<LPCU, RPCU>::RightPowerCtrlUnit;



		/*
		 * @required	Ҫ��AxisSyncPolicy�������·���
		 *					@method EnableAutoSpeedControl ʹ���Զ��ٶȿ���
		 *					@method DisableAutoSpeedControl ��ֹ�Զ��ٶȿ���
		 */
		using AxisSyncPolicy<LPCU, RPCU>::EnableAutoSpeedControl;
		using AxisSyncPolicy<LPCU, RPCU>::DisableAutoSpeedControl;



		/*
		 * @brief	˫����˶�����ϵͳ��׼���캯��
		 * @param	period ��ɢϵͳ���ڣ���λs
		 */
		DualMotorMotionControlSystem(float period)
			: AxisSyncPolicy<LPCU, RPCU>(period)
		{

		}


		

	};


}


#endif  /*_ADX_APP_DUALMOTORMOTIONCONTROLSYSTEM_H_*/
