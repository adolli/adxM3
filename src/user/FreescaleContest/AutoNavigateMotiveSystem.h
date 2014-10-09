   
#ifndef _ADX_APP_AUTONAVIGATEMOTIVESYSTEM_H_
#define _ADX_APP_AUTONAVIGATEMOTIVESYSTEM_H_


#include "DualMotorMotionControlSystem.h"
#include "DirectionControl.h"
#include "../../loki/NullType.h"




namespace adxM3
{


	/*
	 * @brief	寻迹运动系统
	 * @tparam	MotionSystem 运动系统
	 *			DirectionSystem 转向系统
	 *			 @required SetTargetDeviation(DeviationValueType) 设置目标偏差量
	 *			 @required GetDirectionActuator() 获取执行机构，返回MotionSystem::Actuator&类型
	 *			TickTimer 滴答定时器
	 */
	template
	<
		class MotionSystem,
		class DirectionSystem,
		class TickTimer,
		class Indicator = NullType,
		class Indicator_LED = NullType,
		class SystemCop = NullType,
		class BlackLineDetector = NullType
	>
	class AutoNavigateMotiveSystem
		: public MotionSystem
		, public DirectionSystem
	{
	public:


		/*
		 * @required	要求DirectionSystem带有以下类型
		 *					@type DirValueType 方向值类型
		 *					@type DistanceValueType 偏差距离值类型
		 */
		typedef typename DirectionSystem::DirValueType DirValueType;
		typedef typename DirectionSystem::DistanceValueType DistanceValueType;
		
		
		
		


		/*
		 * @required	要求MotionSystem带有以下类型
		 *					@type VelometerDistanceType 测速计距离值类型
		 *					@type VelometerDistanceType 测速计速度值类型
		 *					@type MotorVelocityType 电机速度控制量类型
		 *					@type SpeedType 描述的速度值类型
		 */
		typedef typename MotionSystem::VelometerDistanceType VelometerVelocityType;
		typedef typename MotionSystem::VelometerDistanceType VelometerDistanceType;
		typedef typename MotionSystem::MotorVelocityType MotorVelocityType;
		typedef typename MotionSystem::SpeedType SpeedType;





		using MotionSystem::LeftPowerCtrlUnit;
		using MotionSystem::RightPowerCtrlUnit;


		using DirectionSystem::data;

		using DirectionSystem::GetCurrentDirection;
		using DirectionSystem::GetDirectionActuator;
		using DirectionSystem::Sampling;
		using DirectionSystem::SetSteeringAngle;
		
		using MotionSystem::GetAvgTargetSpeed;
		using MotionSystem::GetCurrentSpeed;
		using MotionSystem::EnableAutoSpeedControl;
		using MotionSystem::DisableAutoSpeedControl;
		using MotionSystem::SpeedRegulatorTransferFunction;

		
	

	public:

		AutoNavigateMotiveSystem(float period)
			: MotionSystem(period)
			, DirectionRegulator(period)
			, SpeedDifferentialRate(0.002182867) // 精确
			, RunningStatus(StraightAhead), outOfLine(false)
			, CurrentSettingSpeed(0), SpeedReference(0)
			, SystemSteadyStateCalculationTimeConsumption(0)
			, enableAutoNavigate(false)
			, trackTimeInMillis(0)
			, systemTimeInMillis(0)
			, enableCornerTurning(true)
			, enableVariantSpeed(false)
		{
			This = this;

			struct TickTimerEventListener
			{
				static void OnTick(TickTimer* timer)
				{
					static int tick_count = 0;
					++tick_count;

					// 喂看门狗
					if (tick_count & _BV(2))
					{
						//SystemCop::FeedWatchingDog();
					}			

					if (This->AutoNavigateEnabled())
					{
						if (tick_count % 2 == 0)
						{
							This->DirectionRegulatorTransferFunction();
						}
					}
					
					
					This->SpeedDifferentialAutoCorrection();
					This->SpeedRegulatorTransferFunction();
					
					
					// 保存每次矫正消耗的时间
					This->SystemSteadyStateCalculationTimeConsumption = timer->GetTimeOut() - timer->GetTime();

					// 记录赛道时间
					if (This->SpeedReference != 0 && This->GetCurrentSpeed() >= 50)
					{
						++This->trackTimeInMillis;
					}
					This->systemTimeInMillis += 4;
				}
			};

			ticker.SetTimeOut(period * 1000000);
			ticker.AddEventListener(TickTimer::TIME_OUT, TickTimerEventListener::OnTick);



			// 设置无偏差量
			SetTargetDeviation(0);
			DirectionRegulator.OutputEnable();

			
			// 黑线检测器输入配置
			blackLineDetector.InputConfig(blackLineDetector.Floating);


			ticker.Start();

			// 开启看门狗(8ms)
			//SystemCop::EnableWatchingDog();
		}



		
		/*
		 * @brief	获取控制器处理数据占用时间
		 * @retval	时间，单位us
		 */
		UINT32 GetProcTime() const
		{
			return SystemSteadyStateCalculationTimeConsumption;
		}


		
		
		
		

		/*
		 * @brief	设置目标偏差量
		 * @param	dir 角度量，单位由DeviationSensor::ValueType决定
		 */
		void SetTargetDeviation(float deviation)
		{
			DirectionRegulator.SetTargetValue(deviation);
		}
		
		



		/*
		 * @brief	设置/获取目标速度，中轴线上的速度
		 * @param	speed 目标速度，单位mm/s
		 */
		void SetTargetSpeed(SpeedType speed)
		{
			CurrentSettingSpeed = speed;
			SpeedDifferentialAutoCorrection();
		}
		SpeedType GetTargetSpeed() const
		{
			return GetAvgTargetSpeed();
		}




		/*
		 * @brief	差速自动修正
		 */
		void SpeedDifferentialAutoCorrection()
		{
			float Dir = GetCurrentDirection();
			float SpeedDifferential = SpeedDifferentialRate * -Dir;
			MotionSystem::SetTargetSpeed(
				(float)CurrentSettingSpeed * (1 - SpeedDifferential), 
				(float)CurrentSettingSpeed * (1 + SpeedDifferential));
		}



		/*
		 * @brief	设置速度基准值
		 *			用于速度微调
		 */
		void SetSpeedReference(SpeedType speed)
		{
			SpeedReference = speed;
		}
		SpeedType GetSpeedReference() const
		{
			return SpeedReference;
		}




		/*
		 * @brief	速度档位调节
		 */
		enum SPDSTLS
		{
			NormalSpeed,
			StraightLine,
			CornerTurning,
			CurveTurning,
		};
		void SwitchSpeed(SPDSTLS spd)
		{
			if (outOfLine)
			{
				return;
			}

			switch (spd)
			{
			case NormalSpeed :
				SetTargetSpeed(SpeedReference);
				break;

			case StraightLine :
				if (enableVariantSpeed)
				{
					SetTargetSpeed(SpeedReference * 1.3);
				}
				else
				{
					SetTargetSpeed(SpeedReference);
				}
				break;

			case CornerTurning :
				if (SpeedReference > 2900)
				{
					SetTargetSpeed(2900);
				}
				else
				{
					SetTargetSpeed(SpeedReference);
				}
				break;

			case CurveTurning :
				if (SpeedReference > 2700)
				{
					SetTargetSpeed(2700);
				}
				else
				{
					SetTargetSpeed(SpeedReference);
				}
				break;

			default:;
			}
		}







		/*
		 * @data	指示器for debug
		 */
		Indicator idc;
		Indicator_LED idc2;


		BlackLineDetector blackLineDetector;


		struct Range
		{
			Range(int left = 0, int right = 0)
				: l(left), r(right)
			{
			}

			Range(const Range& rhs)
			{
				operator=(rhs);
			}
			Range& operator=(const Range& rhs)
			{
				l = rhs.l;
				r = rhs.r;
				return *this;
			}


			int GetCenter() const
			{
				return (l + r) / 2;
			}

			int GetRangeSize() const
			{
				return abs(r - l);
			}

			void SetValue(int left, int right)
			{
				l = left;
				r = right;
			}

			volatile int l;
			volatile int r;
		};



		DistanceValueType bin_data[DirectionSystem::DeviationDataSampleCount];

		Range track;

		int HighPixCount;
		float HighPixDistribute;


		/*
		 * @brief	方向调节器传递函数
		 */
		void DirectionRegulatorTransferFunction()
		{
			Sampling();

			// ccd数值放大处理
			int ccd_sum = 0;
			int ccd_max = 0;
			int ccd_max_x = 0;
			data[0] = 2;
			for (int x = 1; x < DirectionSystem::DeviationDataSampleCount; ++x)
			{
				
				data[x] = data[x] * 6 * 40 / 65535;
				data[x] = data[x] < 18 ? 0 : data[x] - 18;
				data[x] *= 6;

				if (data[x] > ccd_max)
				{
					ccd_max = data[x];
					ccd_max_x = x;
				}
				ccd_sum += data[x];
			}

			
			for (int i = 0; i < 2 && ccd_max < 50; ++i)
			{
				ccd_sum = 0;
				for (int x = 1; x < DirectionSystem::DeviationDataSampleCount; ++x)
				{
					data[x] *= 2;
					if (data[x] > ccd_max)
					{
						ccd_max = data[x];
						ccd_max_x = x;
					}
					ccd_sum += data[x];
				}
			}
				


			// 贰值化
			for (int x = 0; x < DirectionSystem::DeviationDataSampleCount; ++x)
			{
				bin_data[x] = data[x] > 25 ? 40 : 2;
			}

			// 从最大值点开始搜索搜索白线位置
			//track.SetValue(0, DirectionSystem::DeviationDataSampleCount - 1);
			Range tmpTrack;
			for (int x = ccd_max_x; x >= 2; --x)
			{
				// 左沿
				if (data[x] - data[x - 2] > 10)
				{
					tmpTrack.l = x;
					break;
				}
			}
			for (int x = ccd_max_x; x < DirectionSystem::DeviationDataSampleCount - 2; ++x)
			{
				// 右沿
				if (data[x] - data[x + 2] > 10)
				{
					tmpTrack.r = x;
					break;
				}
			}
			if (tmpTrack.l < tmpTrack.r)
			{
				track = tmpTrack;
			}

			static int cornerTurningCount = 0;
			static DirValueType dir = 0;

			// 计数贰值化为高的像素个数并判断分布
			if (systemTimeInMillis > 1000)
			{
				HighPixCount = 0;
				HighPixDistribute = 0.0;
				for (int x = 0; x < DirectionSystem::DeviationDataSampleCount; ++x)
				{
					if (bin_data[x] == 40)
					{
						++HighPixCount;
						HighPixDistribute += x;
					}
				}
				HighPixDistribute /= HighPixCount;

				if (HighPixCount > 50)
				{
					SetTargetSpeed(0);
				}
				else if (HighPixCount > 25 && abs(HighPixDistribute - DirectionSystem::DeviationDataSampleCount / 2) > 10)
				{
					RunningStatus = CornerTurningLR;
					dir = sgn(HighPixDistribute - DirectionSystem::DeviationDataSampleCount / 2) * -100;
				}
			}

			
			
			int deviation = track.GetCenter() - DirectionSystem::DeviationDataSampleCount / 2;

			if (RunningStatus == StraightAhead)
			{
				idc.Off();
				if (track.GetRangeSize() > 25)
				{
					RunningStatus = CornerTurningLR;
					dir = sgn(deviation) * -100;
				}
				else
				{
					dir = DirectionRegulator.ClosedLoopGain(deviation);
				}
			}
			else if (RunningStatus == CornerTurningLR)
			{
				++cornerTurningCount;
				idc.On();
				if (cornerTurningCount > 40 && abs(deviation) < 20 && track.GetRangeSize() < 16)
				{
					cornerTurningCount = 0;
					RunningStatus = StraightAhead;
					dir = DirectionRegulator.ClosedLoopGain(deviation);
				}
			}
			else
			{
				//SetTargetSpeed(0);
			}
			
			SetSteeringAngle(DirFilter(-dir));
			
		}







		// 方向调节器
		// 正常轴线调节器
		PDElement<DirValueType, float> DirectionRegulator;

		FLT::KalmanFilter<DirValueType, 0.30> DirFilter;


		/*
		 * @brief	自动寻迹使能
		 */
		void EnableAutoNavigate()
		{
			enableAutoNavigate = true;
		}
		void DisableAutoNavigate()
		{
			enableAutoNavigate = false;
		}
		bool AutoNavigateEnabled()
		{
			return enableAutoNavigate;
		}



		/*
		 * @brief	获取小车运行状态
		 */ 
		const char* GetRunningStatus() const
		{
			switch (RunningStatus)
			{
			case CornerTurningLR :
				return "CT LR";

			case StraightAhead :
				return "StraightAhead";

			case CornerTurningLeft :
				return "CT Left";

			case CornerTurningRight :
				return "CT Right";

			case BigSTurningLeft :
				return "BS Left";

			case BigSTurningRight :
				return "BS Right";

			case LineLost :
				return "Line Lost";

			default:
				return "Error status";
			}
		}




		int GetTrackTime() const
		{
			return trackTimeInMillis;
		}
		int GetSystemTimeInMillis() const
		{
			return systemTimeInMillis;
		}



		void DisableCornerTurning()
		{
			enableCornerTurning = false;
		}



		void EnableVariantSpeed()
		{
			enableVariantSpeed = true;
		}


	private:


		// 差速比率
		float SpeedDifferentialRate;


		// 运行状态选项
		enum RSTA
		{
			StraightAhead,
			CornerTurningLR,
			CornerTurningRight,
			CornerTurningLeft,
			BigSTurningLeft,
			BigSTurningRight,
			LineLost,
		};




		// 运行状态
		RSTA RunningStatus;


		// 脱线标志
		bool outOfLine;



		// 目标速度（车轴线上的速度）
		SpeedType CurrentSettingSpeed;
		SpeedType SpeedReference;


		// 核心滴答定时器
		TickTimer ticker;


		// singleton 实例指针
		static AutoNavigateMotiveSystem* This;


		// 系统稳态计算时间消耗，单位us
		UINT32 SystemSteadyStateCalculationTimeConsumption;

		
		// 自动寻迹使能控制
		bool enableAutoNavigate;


		// 赛道一圈寻迹时间
		volatile int trackTimeInMillis;
		volatile int systemTimeInMillis;


		// 直角转弯使能
		bool enableCornerTurning;


		// 变速控制
		bool enableVariantSpeed;


	public:

		int _CCD_centerValue;
	};




	template
	<
		class MotionSystem,
		class DirectionSystem,
		class TickTimer,
		class Indicator,
		class Indicator_LED,
		class SystemCop,
		class BlackLineDetector
	>
	AutoNavigateMotiveSystem<MotionSystem, DirectionSystem, TickTimer, Indicator, Indicator_LED, SystemCop, BlackLineDetector>* 
	AutoNavigateMotiveSystem<MotionSystem, DirectionSystem, TickTimer, Indicator, Indicator_LED, SystemCop, BlackLineDetector>::This = NULL;


}

#endif  /*_ADX_APP_AUTONAVIGATEMOTIVESYSTEM_H_*/
