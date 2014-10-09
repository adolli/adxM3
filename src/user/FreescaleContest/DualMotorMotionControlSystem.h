
 
#ifndef _ADX_APP_DUALMOTORMOTIONCONTROLSYSTEM_H_
#define _ADX_APP_DUALMOTORMOTIONCONTROLSYSTEM_H_

#include "adxM3.h"

namespace adxM3
{
	
	
	// 使用自动控制系统
	using namespace ACS;




	/*
	 * @brief	动力控制系统
	 *			闭环速度控制
	 * 			MotorDriver要求实现以下方法
	 * 				@required SetSpeed(MotorDriver::VelocityType) 设置电机速度控制量分数值
	 * 				@required MotorDriver::VelocityType GetSpeed() 获取电机速度控制量分数值
	 * 			Velometer要求实现以下方法
	 * 				@required Velometer::DistanceType GetCounter() 获取编码器距离测量值
	 * 				@required ResetCounter() 编码器测量值归零
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
		 * @brief	设置速度
		 * @param	speed 速度分数，范围[-1,1]
		 */
		void SetSpeed(MotorVelocityType speed)
		{
			RangeLimit(-1, <=, speed, <=, 1);

			// 保存当前电机速度控制量设定值，提高Get方法的效率
			CurrentSettingSpeed = speed;
			Motor.SetSpeed(speed);
		}



		/*
		 * @brief	获取速度
		 */
		MotorVelocityType GetSpeed() const
		{
			return CurrentSettingSpeed;
		}


		

		/*
		 * @brief	动力系统构造函数
		 * @param	period 系统采样周期，单位s
		 */
		PowerControlSystem(float period)
			: SpeedRegulator(period), PositionRegulator(period)
			, SpeedEncoder(period), CurrentSettingSpeed(0.0)
		{
		}




		/*
		 * @brief	动力系统测速装置脉冲计数与距离换算常数
		 */
		enum
		{
			//CountPerMeter = 19825		// 500 线正交编码器，小车前进一米脉冲计数值
			CountPerMeter = 57872		// 4096 线绝对式编码器，小车前进一米脉冲计数值
		};



		/*
		 * @brief	获取编码速度值
		 * @retval	动力系统速度值，单位毫米每秒
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
		 * @brief	获取当前行进距离
		 * @retval	距离脉冲计数值，无单位
		 */
		VelometerDistanceType GetDistance()
		{
			return SpeedEncoder.GetDistance();
		}





		/*
		 * @brief	速度调节器传递函数
		 */
		void ExcuteTransferFunction()
		{
			// 位置反馈（该环节一般未使能）
			//VelometerDistanceType dist = GetDistance();
			//VelometerDistanceType deltaTargetSpeed = PositionRegulator.DeltaClosedLoopGain(dist);
			//SpeedRegulator.TargetValue += deltaTargetSpeed;

			// 速度反馈
			VelometerVelocityType speed = GetSpeedEncoderSpeed();

			// 速度死区控制
			if (abs(speed - SpeedRegulator.TargetValue) < 5)
			{
				speed = SpeedRegulator.TargetValue;
			}
			
			// 速度调节器的闭环增益环节要执行，便于从环节读取速度值
			MotorVelocityType deltaSpeed = SpeedRegulator.DeltaClosedLoopGain(speed);
			SetSpeed(GetSpeed() + deltaSpeed);
		}




		/*
		 * @brief	使能/禁止速度调节器
		 */
		void EnableSpeedRegulator()
		{
			SpeedRegulator.OutputEnable();
		}
		void DisableSpeedRegulator()
		{
			SpeedRegulator.OutputDisable();
		}

		



		// PID 速度调节器
		PIDElement<MotorVelocityType, VelometerVelocityType, UsingKiKd> SpeedRegulator;
		


		// PID 位置调节器
		UniformizedPIDElement<VelometerDistanceType, VelometerDistanceType> PositionRegulator;


		// 保存当前测量的速度值
		volatile VelometerVelocityType CurrentVelocityMeasured;


	protected:
	public:


		// 电机
		MotorDriver Motor;

		// 速度编码器
		Velometer SpeedEncoder;


		// 该值为当前设定值，并非实时值
		// 因为PWM脉冲宽度更新还要等待最多一个脉冲周期
		volatile MotorVelocityType CurrentSettingSpeed;


	};






	/*
	 * @brief	双电机动力系统轴同步控制
	 * @tparam	LeftPCU 左动力控制单元，要求实现以下方法
	 *				@required Forward() 正转
	 *				@required Backward() 反转
	 *				@required Brake() 制动
	 *				@required SetSpeed(UINT8) 设置速度（百分速度值）
	 *				@required GetSpeed(UINT8) 获取速度（百分速度值）
	 *			RightPCU 左动力控制单元，要求实现与左电机相同
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
		 * @brief	同轴控制构造函数
		 * @param	period 系统采样周期，单位s
		 */
		AxisSynchronouslyControl(float period)
			: LeftPowerCtrlUnit(period), RightPowerCtrlUnit(period)
		{
		}


		



		/*
		 * @brief	制动
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
		 * @brief	设置速度
		 * @param	speed_mmps 速度，单位毫米每秒
		 */
		void SetTargetSpeed(SpeedType speedL_mmps, SpeedType speedR_mmps = -1)
		{
			LeftPowerCtrlUnit.SpeedRegulator.SetTargetValue(speedL_mmps);
			RightPowerCtrlUnit.SpeedRegulator.SetTargetValue(speedL_mmps);
			//LeftPowerCtrlUnit.SpeedRegulator.OutputEnable();
			//RightPowerCtrlUnit.SpeedRegulator.OutputEnable();
		}



		/*
		 * @brief	获取速度，单位mm/s
		 */
		SpeedType GetCurrentSpeed() const
		{
			// 双轮同速，只取其一
			return LeftPowerCtrlUnit.SpeedRegulator.GetCurrentValue();
		}



		/*
		 * @brief	倒退回滚，退回起点
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
		 * @brief	速度调节器传递函数
		 */
		void SpeedRegulatorTransferFunction()
		{
			LeftPowerCtrlUnit.ExcuteTransferFunction();
			RightPowerCtrlUnit.ExcuteTransferFunction();
		}


	
		// 动力控制单元
		LeftPCU LeftPowerCtrlUnit;
		RightPCU RightPowerCtrlUnit;


	};







	
	/*
	 * @brief	双电机动力系统轴异步控制
	 * @tparam	LeftPCU 左动力控制单元，要求实现以下方法
	 *				@required Forward() 正转
	 *				@required Backward() 反转
	 *				@required Brake() 制动
	 *				@required SetSpeed(UINT8) 设置速度（百分速度值）
	 *				@required GetSpeed(UINT8) 获取速度（百分速度值）
	 *			RightPCU 左动力控制单元，要求实现与左电机相同
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
		 * @brief	同轴控制构造函数
		 * @param	period 系统采样周期，单位s
		 */
		AxisAsynchronouslyControl(float period)
			: LeftPowerCtrlUnit(period), RightPowerCtrlUnit(period)
		{
		}


		



		/*
		 * @brief	制动
		 */
		void Brake()
		{
			SetTargetSpeed(0, 0);
		}



		/*
		 * @brief	设置速度
		 * @param	speed_mmps 速度，单位毫米每秒
		 */
		void SetTargetSpeed(SpeedType speedL_mmps, SpeedType speedR_mmps)
		{
			LeftPowerCtrlUnit.SpeedRegulator.SetTargetValue(speedL_mmps);
			RightPowerCtrlUnit.SpeedRegulator.SetTargetValue(speedR_mmps);
		}



		/*
		 * @brief	获取所设置的平均速度，中轴线上的平均值
		 * @retval	速度，单位毫米每秒
		 */
		SpeedType GetAvgTargetSpeed() const
		{
			return (LeftPowerCtrlUnit.SpeedRegulator.TargetValue 
				+  RightPowerCtrlUnit.SpeedRegulator.TargetValue) / 2;
		}




		/*
		 * @brief	获取速度，左右轮平均值，单位mm/s
		 */
		SpeedType GetCurrentSpeed() const
		{
			return (LeftPowerCtrlUnit.SpeedRegulator.GetCurrentValue() 
				+  RightPowerCtrlUnit.SpeedRegulator.GetCurrentValue()) / 2;
		}




		/*
		 * @brief	倒退回滚，退回起点
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
		 * @brief	使能/禁止自动速度控制
		 *			闭环速度调节器匀速控制
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
		 * @brief	速度调节器传递函数
		 */
		void SpeedRegulatorTransferFunction()
		{
			LeftPowerCtrlUnit.ExcuteTransferFunction();
			RightPowerCtrlUnit.ExcuteTransferFunction();
		}




		// 动力控制单元
		LeftPCU LeftPowerCtrlUnit;
		RightPCU RightPowerCtrlUnit;



	private:


	};










	/*
	 * @brief	双电机被动式差速动力系统轴异步控制
	 * @tparam	LeftPCU 左动力控制单元，要求实现以下方法
	 *				@required Forward() 正转
	 *				@required Backward() 反转
	 *				@required Brake() 制动
	 *				@required SetSpeed(UINT8) 设置速度（百分速度值）
	 *				@required GetSpeed(UINT8) 获取速度（百分速度值）
	 *			RightPCU 左动力控制单元，要求实现与左电机相同
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
		 * @brief	同轴控制构造函数
		 * @param	period 系统采样周期，单位s
		 */
		AxisPassivelyControl(float period)
			: UnionSpeedRegulator(period), LeftPowerCtrlUnit(period), RightPowerCtrlUnit(period)
		{
		}


		



		/*
		 * @brief	制动
		 */
		void Brake()
		{
			SetTargetSpeed(0, 0);
		}



		/*
		 * @brief	设置速度
		 * @param	speed_mmps 速度，单位毫米每秒
		 */
		void SetTargetSpeed(SpeedType speedL_mmps, SpeedType speedR_mmps)
		{
			UnionSpeedRegulator.SetTargetValue((speedL_mmps + speedR_mmps) / 2);
		}



		/*
		 * @brief	获取所设置的平均速度，中轴线上的平均值
		 * @retval	速度，单位毫米每秒
		 */
		SpeedType GetAvgTargetSpeed() const
		{
			return UnionSpeedRegulator.TargetValue;
		}




		/*
		 * @brief	获取速度，左右轮平均值，单位mm/s
		 */
		SpeedType GetCurrentSpeed() const
		{
			return UnionSpeedRegulator.GetCurrentValue();
		}




		/*
		 * @brief	倒退回滚，退回起点
		 */
		void debug_LoopBack()
		{
		}

	


		/*
		 * @brief	使能/禁止自动速度控制
		 *			闭环速度调节器匀速控制
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
		 * @brief	速度调节器传递函数
		 */
		void SpeedRegulatorTransferFunction()
		{
			// 速度反馈
			VelometerVelocityType velocityL = LeftPowerCtrlUnit.GetSpeedEncoderSpeed();
			VelometerVelocityType velocityR = RightPowerCtrlUnit.GetSpeedEncoderSpeed();
			VelometerVelocityType avgVelocity = (velocityL + velocityR) / 2; 

			
			// 速度死区控制
			if (abs(avgVelocity - UnionSpeedRegulator.TargetValue) < 5)
			{
				avgVelocity = UnionSpeedRegulator.TargetValue;
			}

			// 速度调节器的闭环增益环节要执行，便于从环节读取速度值
			MotorVelocityType deltaSpeed_pwm = UnionSpeedRegulator.DeltaClosedLoopGain(avgVelocity);

			if (abs(velocityR - velocityL) / abs(avgVelocity) > 0.438)
			{
				return;
			}

			// 执行自动差速调整		
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



		// 联合速度调节器
		PIDElement<MotorVelocityType, VelometerVelocityType, UsingKiKd> UnionSpeedRegulator;



		// 动力控制单元
		LeftPCU LeftPowerCtrlUnit;
		RightPCU RightPowerCtrlUnit;



	private:



	};













	/*
	 * @brief	双电机动力系统
	 * @tparam	LeftMotor 左电机，要求实现以下方法
	 *				@required Forward() 正转
	 *				@required Backward() 反转
	 *				@required Brake() 制动
	 *				@required SetSpeed(UINT8) 设置速度（百分速度值）
	 *				@required GetSpeed(UINT8) 获取速度（百分速度值）
	 *			RightMotor 右电机，要求实现与左电机相同
	 *			AxisSyncPolicy 轴同步策略
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
		 * @required	要求AxisSyncPolicy带有以下类型
		 *					@type VelometerDistanceType 测速计距离值类型
		 *					@type VelometerVelocityType 测速计速度值类型
		 *					@type MotorVelocityType 电机速度控制量类型
		 */
		typedef typename AxisSyncPolicy<LPCU, RPCU>::VelometerDistanceType VelometerDistanceType;
		typedef typename AxisSyncPolicy<LPCU, RPCU>::VelometerVelocityType VelometerVelocityType;
		typedef typename AxisSyncPolicy<LPCU, RPCU>::MotorVelocityType MotorVelocityType;



		/*
		 * @required	要求AxisSyncPolicy带有以下数据成员
		 *					@data LPCU LeftPowerCtrlUnit 左动力控制单元
		 *					@data RPCU RightPowerCtrlUnit 右动力控制单元
		 */
		using AxisSyncPolicy<LPCU, RPCU>::LeftPowerCtrlUnit;
		using AxisSyncPolicy<LPCU, RPCU>::RightPowerCtrlUnit;



		/*
		 * @required	要求AxisSyncPolicy带有以下方法
		 *					@method EnableAutoSpeedControl 使能自动速度控制
		 *					@method DisableAutoSpeedControl 禁止自动速度控制
		 */
		using AxisSyncPolicy<LPCU, RPCU>::EnableAutoSpeedControl;
		using AxisSyncPolicy<LPCU, RPCU>::DisableAutoSpeedControl;



		/*
		 * @brief	双电机运动控制系统标准构造函数
		 * @param	period 离散系统周期，单位s
		 */
		DualMotorMotionControlSystem(float period)
			: AxisSyncPolicy<LPCU, RPCU>(period)
		{

		}


		

	};


}


#endif  /*_ADX_APP_DUALMOTORMOTIONCONTROLSYSTEM_H_*/
