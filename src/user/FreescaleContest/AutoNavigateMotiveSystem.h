   
#ifndef _ADX_APP_AUTONAVIGATEMOTIVESYSTEM_H_
#define _ADX_APP_AUTONAVIGATEMOTIVESYSTEM_H_


#include "DualMotorMotionControlSystem.h"
#include "DirectionControl.h"
#include "../../loki/NullType.h"




namespace adxM3
{


	/*
	 * @brief	Ѱ���˶�ϵͳ
	 * @tparam	MotionSystem �˶�ϵͳ
	 *			DirectionSystem ת��ϵͳ
	 *			 @required SetTargetDeviation(DeviationValueType) ����Ŀ��ƫ����
	 *			 @required GetDirectionActuator() ��ȡִ�л���������MotionSystem::Actuator&����
	 *			TickTimer �δ�ʱ��
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
		 * @required	Ҫ��DirectionSystem������������
		 *					@type DirValueType ����ֵ����
		 *					@type DistanceValueType ƫ�����ֵ����
		 */
		typedef typename DirectionSystem::DirValueType DirValueType;
		typedef typename DirectionSystem::DistanceValueType DistanceValueType;
		
		
		
		


		/*
		 * @required	Ҫ��MotionSystem������������
		 *					@type VelometerDistanceType ���ټƾ���ֵ����
		 *					@type VelometerDistanceType ���ټ��ٶ�ֵ����
		 *					@type MotorVelocityType ����ٶȿ���������
		 *					@type SpeedType �������ٶ�ֵ����
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
			, SpeedDifferentialRate(0.002182867) // ��ȷ
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

					// ι���Ź�
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
					
					
					// ����ÿ�ν������ĵ�ʱ��
					This->SystemSteadyStateCalculationTimeConsumption = timer->GetTimeOut() - timer->GetTime();

					// ��¼����ʱ��
					if (This->SpeedReference != 0 && This->GetCurrentSpeed() >= 50)
					{
						++This->trackTimeInMillis;
					}
					This->systemTimeInMillis += 4;
				}
			};

			ticker.SetTimeOut(period * 1000000);
			ticker.AddEventListener(TickTimer::TIME_OUT, TickTimerEventListener::OnTick);



			// ������ƫ����
			SetTargetDeviation(0);
			DirectionRegulator.OutputEnable();

			
			// ���߼������������
			blackLineDetector.InputConfig(blackLineDetector.Floating);


			ticker.Start();

			// �������Ź�(8ms)
			//SystemCop::EnableWatchingDog();
		}



		
		/*
		 * @brief	��ȡ��������������ռ��ʱ��
		 * @retval	ʱ�䣬��λus
		 */
		UINT32 GetProcTime() const
		{
			return SystemSteadyStateCalculationTimeConsumption;
		}


		
		
		
		

		/*
		 * @brief	����Ŀ��ƫ����
		 * @param	dir �Ƕ�������λ��DeviationSensor::ValueType����
		 */
		void SetTargetDeviation(float deviation)
		{
			DirectionRegulator.SetTargetValue(deviation);
		}
		
		



		/*
		 * @brief	����/��ȡĿ���ٶȣ��������ϵ��ٶ�
		 * @param	speed Ŀ���ٶȣ���λmm/s
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
		 * @brief	�����Զ�����
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
		 * @brief	�����ٶȻ�׼ֵ
		 *			�����ٶ�΢��
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
		 * @brief	�ٶȵ�λ����
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
		 * @data	ָʾ��for debug
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
		 * @brief	������������ݺ���
		 */
		void DirectionRegulatorTransferFunction()
		{
			Sampling();

			// ccd��ֵ�Ŵ���
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
				


			// ��ֵ��
			for (int x = 0; x < DirectionSystem::DeviationDataSampleCount; ++x)
			{
				bin_data[x] = data[x] > 25 ? 40 : 2;
			}

			// �����ֵ�㿪ʼ������������λ��
			//track.SetValue(0, DirectionSystem::DeviationDataSampleCount - 1);
			Range tmpTrack;
			for (int x = ccd_max_x; x >= 2; --x)
			{
				// ����
				if (data[x] - data[x - 2] > 10)
				{
					tmpTrack.l = x;
					break;
				}
			}
			for (int x = ccd_max_x; x < DirectionSystem::DeviationDataSampleCount - 2; ++x)
			{
				// ����
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

			// ������ֵ��Ϊ�ߵ����ظ������жϷֲ�
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







		// ���������
		// �������ߵ�����
		PDElement<DirValueType, float> DirectionRegulator;

		FLT::KalmanFilter<DirValueType, 0.30> DirFilter;


		/*
		 * @brief	�Զ�Ѱ��ʹ��
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
		 * @brief	��ȡС������״̬
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


		// ���ٱ���
		float SpeedDifferentialRate;


		// ����״̬ѡ��
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




		// ����״̬
		RSTA RunningStatus;


		// ���߱�־
		bool outOfLine;



		// Ŀ���ٶȣ��������ϵ��ٶȣ�
		SpeedType CurrentSettingSpeed;
		SpeedType SpeedReference;


		// ���ĵδ�ʱ��
		TickTimer ticker;


		// singleton ʵ��ָ��
		static AutoNavigateMotiveSystem* This;


		// ϵͳ��̬����ʱ�����ģ���λus
		UINT32 SystemSteadyStateCalculationTimeConsumption;

		
		// �Զ�Ѱ��ʹ�ܿ���
		bool enableAutoNavigate;


		// ����һȦѰ��ʱ��
		volatile int trackTimeInMillis;
		volatile int systemTimeInMillis;


		// ֱ��ת��ʹ��
		bool enableCornerTurning;


		// ���ٿ���
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
