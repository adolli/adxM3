
#ifndef _ADX_APP_PIDELEMENT_H_
#define _ADX_APP_PIDELEMENT_H_

#include "../common/defs.h"
#include "../common/TypeTraits.h"
#include <cmath>

namespace adxM3
{


	/*
	 * @brief	automatic control system
	 */
	namespace ACS
	{



		/*
		 * @brief	环节基类
		 */
		template< class OUTPUT, class INPUT >
		struct ElementBase
		{



			typedef typename ArithTraits<OUTPUT>::Signed DELTA_OUTPUT;
			typedef typename ArithTraits<INPUT>::MaxBitsSigned ErrorType;



			ElementBase(float period)
				: T(period), Error_n1(0), Error_n2(0), CurrentValue(0), TargetValue(0)
				, outputEnabled(false)
			{
			}



			/*
			 * @brief	闭环增益函数
			 */
			virtual DELTA_OUTPUT DeltaClosedLoopGain(INPUT feedbackValue) = 0;
			virtual OUTPUT ClosedLoopGain(INPUT feedbackValue) = 0;


			
			/*
			 * @brief	设置目标速度值
			 * @param	target_speed 目标速度，单位毫米每秒
			 */
			void SetTargetValue(INPUT target)
			{
				TargetValue = target;
			}



			/*
			 * @brief	获取当前速度值
			 * @retval	当前速度值，单位 mm/s
			 */
			INPUT GetCurrentValue() const
			{
				return CurrentValue;
			}



			/*
			 * @brief	使能/禁止环节
			 */
			void OutputEnable()
			{
				outputEnabled = true;
			}
			void OutputDisable()
			{
				outputEnabled = false;
			}




			/*
			 * @brief	复位误差
			 */
			void ResetErrors()
			{
				Error_n1 = Error_n2 = ErrorType(0);
			}



		protected:


			// 采样周期
			float T;

			// 输入上一次误差、输入上一次的上一次误差
			ErrorType Error_n1, Error_n2;

		public:

			// 当前/目标速度值，单位mm/s
			volatile INPUT CurrentValue;
			volatile INPUT TargetValue;


		protected:

			// 使能状态
			volatile bool outputEnabled;

		};







		/*
		 * @brief	PD环节
		 *			给定一个反馈输入，该环节可输出一个闭环增益
		 *			需设定好采样周期和pd参数
		 *			
		 *			使用前请设定好周期、PID参数、目标值
		 *			并且在一个周期后调用DeltaClosedLoopGain方法，传入实测值作为反馈值
		 *			该方法返回一个误差调节量
		 */
		template< class OUTPUT, class INPUT >
		struct PDElement
			: public ElementBase<OUTPUT, INPUT>
		{

			typedef typename ArithTraits<OUTPUT>::Signed DELTA_OUTPUT;
			typedef typename ArithTraits<INPUT>::MaxBitsSigned ErrorType;


			using ElementBase<OUTPUT, INPUT>::outputEnabled;
			using ElementBase<OUTPUT, INPUT>::Error_n1;
			using ElementBase<OUTPUT, INPUT>::Error_n2;
			using ElementBase<OUTPUT, INPUT>::CurrentValue;
			using ElementBase<OUTPUT, INPUT>::TargetValue;


			PDElement(float period)
				: ElementBase<OUTPUT, INPUT>(period), Kp(0), Kd(0)
			{

			}




			/*
			 * @brief	PID闭环增益调节偏移量
			 * @param	feedbackValue 反馈量
			 * @retval	调节器偏移量
			 */
			virtual OUTPUT ClosedLoopGain(INPUT feedbackValue)
			{
				CurrentValue = feedbackValue;
				const ErrorType Error = TargetValue - CurrentValue;		// 本次误差量
				const ErrorType dError = Error - Error_n1;
				OUTPUT Output = Kp * Error + Kd * dError;

				// 保存误差量数据
				Error_n2 = Error_n1;
				Error_n1 = Error;

				if (!outputEnabled) 
				{
					return OUTPUT(0);
				}
				else 
				{
					return Output;
				}
			}


			virtual DELTA_OUTPUT DeltaClosedLoopGain(INPUT feedbackValue)
			{
				return DELTA_OUTPUT(0);
			}




			
			/*
			 * @brief	设置PD参数值
			 * @param	kp 比例系数
			 *			td 微分时间常数
			 */
			void SetParameters(float kp, float kd)
			{
				Kp = kp;
				Kd = kd;
			}




			float GetKp() const 
			{
				return Kp;
			}

		protected:

			// PD参数
			float Kp, Kd; 

			
		};




		struct UsingTiTd {};
		struct UsingKiKd {};	


		template< class OUTPUT, class INPUT, class ParamPolicy = UsingTiTd >
		struct PIDElement;




		/*
		 * @brief	PID环节
		 *			给定一个反馈输入，该环节可输出一个闭环增益
		 *			需设定好采样周期和pid参数
		 *			
		 *			使用前请设定好周期、PID参数、目标值
		 *			并且在一个周期后调用DeltaClosedLoopGain方法，传入实测值作为反馈值
		 *			该方法返回一个误差调节量
		 */
		template< class OUTPUT, class INPUT >
		struct PIDElement
		<
			OUTPUT,
			INPUT,
			UsingTiTd
		>
			: public ElementBase<OUTPUT, INPUT>
		{

			typedef typename ArithTraits<OUTPUT>::Signed DELTA_OUTPUT;
			typedef typename ArithTraits<INPUT>::MaxBitsSigned ErrorType;
			

			using ElementBase<OUTPUT, INPUT>::outputEnabled;
			using ElementBase<OUTPUT, INPUT>::T;
			using ElementBase<OUTPUT, INPUT>::Error_n1;
			using ElementBase<OUTPUT, INPUT>::Error_n2;
			using ElementBase<OUTPUT, INPUT>::CurrentValue;
			using ElementBase<OUTPUT, INPUT>::TargetValue;


			PIDElement(float period)
				: ElementBase<OUTPUT, INPUT>(period), Kp(0), Ti(0), Td(0)
			{
				
			}






			/*
			 * @brief	PID闭环增益调节偏移量
			 * @param	feedbackValue 反馈量
			 * @retval	调节器偏移量
			 */
			virtual DELTA_OUTPUT DeltaClosedLoopGain(INPUT feedbackValue)
			{
				CurrentValue = feedbackValue;
				const ErrorType Error = TargetValue - CurrentValue;		// 本次误差量
				const float A = Kp * (1 + T / Ti + Td / T);
				const float B = Kp * (1 + 2 * Td / T);
				const float C = Kp * Td / T;
				DELTA_OUTPUT deltaOutput = A * Error - B * Error_n1 + C * Error_n2;


				// 保存误差量数据
				Error_n2 = Error_n1;
				Error_n1 = Error;

				if (!outputEnabled) 
				{
					return DELTA_OUTPUT(0);
				}
				else
				{
					return deltaOutput;
				}
			}






			

			/*
			 * @brief	设置PID参数值
			 * @param	kp 比例系数
			 *			ti 积分时间常数
			 *			td 微分时间常数
			 */
			void SetParameters(float kp, float ti, float td)
			{
				Kp = kp;
				Ti = ti;
				Td = td;
			}





		protected:
				
			// PID参数
			float Kp, Ti, Td; 


		private:


			virtual OUTPUT ClosedLoopGain(INPUT feedbackValue)
			{
				return OUTPUT(0);
			}

		};




		/*
		 * @brief	PID环节
		 *			给定一个反馈输入，该环节可输出一个闭环增益
		 *			需设定好采样周期和pid参数
		 *			
		 *			使用前请设定好周期、PID参数、目标值
		 *			并且在一个周期后调用DeltaClosedLoopGain方法，传入实测值作为反馈值
		 *			该方法返回一个误差调节量
		 */
		template< class OUTPUT, class INPUT >
		struct PIDElement
		<
			OUTPUT,
			INPUT,
			UsingKiKd
		>
			: public ElementBase<OUTPUT, INPUT>
		{

			typedef typename ArithTraits<OUTPUT>::Signed DELTA_OUTPUT;
			typedef typename ArithTraits<INPUT>::MaxBitsSigned ErrorType;
			

			using ElementBase<OUTPUT, INPUT>::outputEnabled;
			using ElementBase<OUTPUT, INPUT>::T;
			using ElementBase<OUTPUT, INPUT>::Error_n1;
			using ElementBase<OUTPUT, INPUT>::Error_n2;
			using ElementBase<OUTPUT, INPUT>::CurrentValue;
			using ElementBase<OUTPUT, INPUT>::TargetValue;


			PIDElement(float period)
				: ElementBase<OUTPUT, INPUT>(period), Kp(0), Ki(0), Kd(0), SumError(0)
			{
				
			}






			/*
			 * @brief	PID闭环增益调节偏移量
			 * @param	feedbackValue 反馈量
			 * @retval	调节器偏移量
			 */
			virtual DELTA_OUTPUT DeltaClosedLoopGain(INPUT feedbackValue)
			{
				CurrentValue = feedbackValue;
				const ErrorType Error = TargetValue - CurrentValue;		// 本次误差量
				
				const ErrorType dError = Error - Error_n1;
				const ErrorType d2Error = Error - 2 * Error_n1 + Error_n2;
				DELTA_OUTPUT deltaOutput = Kp * dError + Ki * Error + Kd * d2Error;

				// 保存误差量数据
				Error_n2 = Error_n1;
				Error_n1 = Error;

				if (!outputEnabled) 
				{
					return DELTA_OUTPUT(0);
				}
				else
				{
					return deltaOutput;
				}
			}




			virtual OUTPUT ClosedLoopGain(INPUT feedbackValue)
			{
				CurrentValue = feedbackValue;
				const ErrorType Error = TargetValue - CurrentValue;		// 本次误差量

				SumError += Error;
				const ErrorType dError = Error - Error_n1;
				OUTPUT Output = Kp * Error + Ki * SumError + Kd * dError;


				// 保存误差量数据
				Error_n2 = Error_n1;
				Error_n1 = Error;

				if (!outputEnabled) 
				{
					return OUTPUT(0);
				}
				else
				{
					return Output;
				}
			}




			

			/*
			 * @brief	设置PID参数值
			 * @param	kp 比例系数
			 *			ti 积分时间常数
			 *			td 微分时间常数
			 */
			void SetParameters(float kp, float ki, float kd)
			{
				Kp = kp;
				Ki = ki;
				Kd = kd;
			}





		protected:
				
			// PID参数
			float Kp, Ki, Kd; 

			ErrorType SumError;


		private:


			

		};






		/*
		 * @brief	PID环节
		 *			给定一个反馈输入，该环节可输出一个闭环增益
		 *			需设定好采样周期和pid参数
		 *			
		 *			使用前请设定好周期、PID参数、目标值
		 *			并且在一个周期后调用DeltaClosedLoopGain方法，传入实测值作为反馈值
		 *			该方法返回一个误差调节量
		 *			
		 * @tparam	ALPHA 为惯性滞后因子 ALPHA := Tf / (Tf + T); Tf是惯性环节时间常数
		 */
		template< class OUTPUT, class INPUT, float ALPHA = 0.5 >
		struct IncompletePIDElement
			: public PIDElement<OUTPUT, INPUT>
		{

			typedef typename ArithTraits<OUTPUT>::Signed DELTA_OUTPUT;
			typedef typename ArithTraits<INPUT>::MaxBitsSigned ErrorType;


		protected:

			using PIDElement<OUTPUT, INPUT>::outputEnabled;
			using PIDElement<OUTPUT, INPUT>::Kp;
			using PIDElement<OUTPUT, INPUT>::Ti;
			using PIDElement<OUTPUT, INPUT>::Td;
			using PIDElement<OUTPUT, INPUT>::T;
			using PIDElement<OUTPUT, INPUT>::Error_n2;
			using PIDElement<OUTPUT, INPUT>::Error_n1;


		public:

			using PIDElement<OUTPUT, INPUT>::TargetValue;
			using PIDElement<OUTPUT, INPUT>::CurrentValue;

			IncompletePIDElement(float period)
				: PIDElement<OUTPUT, INPUT>(period)
				, PrevDeltaOutput(0)
			{

			}





			/*
			 * @brief	PID闭环增益调节偏移量
			 * @param	feedbackValue 反馈量
			 * @retval	调节器偏移量
			 */
			virtual DELTA_OUTPUT DeltaClosedLoopGain(INPUT feedbackValue)
			{
				CurrentValue = feedbackValue;
				const ErrorType Error = TargetValue - CurrentValue;		// 本次误差量
				const float A = Kp * (1 + T / Ti + Td / T);
				const float B = Kp * (1 + 2 * Td / T);
				const float C = Kp * Td / T;
				
				// Δu'(k)项
				DELTA_OUTPUT derivedDeltaOutput = A * Error - B * Error_n1 + C * Error_n2;

				DELTA_OUTPUT deltaOutput = ALPHA * PrevDeltaOutput + (1 - ALPHA) * derivedDeltaOutput;
				PrevDeltaOutput = deltaOutput;

				// 保存误差量数据
				Error_n2 = Error_n1;
				Error_n1 = Error;

				if (!outputEnabled)
				{
					return DELTA_OUTPUT(0);
				}
				else 
				{
					return deltaOutput;
				}
			}


		protected:

			DELTA_OUTPUT PrevDeltaOutput;


		private:


			virtual OUTPUT ClosedLoopGain(INPUT feedbackValue)
			{
				return OUTPUT(0);
			}


		};





		/*
		 * @brief	参数归一化PID环节
		 *			该环节Ti与Td参数由Kp自动确定
		 *			给定一个反馈输入，该环节可输出一个闭环增益
		 *			需设定好采样周期和pid参数
		 *			
		 *			使用前请设定好周期、PID参数（仅Kp）、目标值
		 *			并且在一个周期后调用DeltaClosedLoopGain方法，传入实测值作为反馈值
		 *			该方法返回一个误差调节量
		 */
		template< class OUTPUT, class INPUT >
		struct UniformizedPIDElement
			: public ElementBase<OUTPUT, INPUT>
		{


		protected:

			using ElementBase<OUTPUT, INPUT>::outputEnabled;
			using ElementBase<OUTPUT, INPUT>::T;
			using ElementBase<OUTPUT, INPUT>::Error_n2;
			using ElementBase<OUTPUT, INPUT>::Error_n1;
			using ElementBase<OUTPUT, INPUT>::CurrentValue;
			using ElementBase<OUTPUT, INPUT>::TargetValue;


		public:

			typedef typename ArithTraits<OUTPUT>::Signed DELTA_OUTPUT;
			typedef typename ArithTraits<INPUT>::MaxBitsSigned ErrorType;
			


			UniformizedPIDElement(float period)
				: ElementBase<OUTPUT, INPUT>(period), Kp(0)
			{
				
			}



			/*
			 * @brief	PID闭环增益调节偏移量
			 * @param	feedbackValue 反馈量
			 * @retval	调节器偏移量
			 */
			virtual DELTA_OUTPUT DeltaClosedLoopGain(INPUT feedbackValue)
			{
				CurrentValue = feedbackValue;
				const ErrorType Error = TargetValue - CurrentValue;		// 本次误差量
				
				// 参数归一化PID增益表达式
				DELTA_OUTPUT deltaOutput = Kp * (2.45 * Error - 3.5 * Error_n1 + 1.25 * Error_n2);

				// 保存误差量数据
				Error_n2 = Error_n1;
				Error_n1 = Error;

				if (!outputEnabled)
				{
					return DELTA_OUTPUT(0);
				}
				else
				{
					return deltaOutput;
				}
			}



			

			/*
			 * @brief	设置PID参数值
			 * @param	kp 比例系数
			 *			ti 积分时间常数
			 *			td 微分时间常数
			 */
			void SetParameters(float kp)
			{
				Kp = kp;
			}




		protected:
			
			
			// PID参数
			float Kp; 



		private:


			virtual OUTPUT ClosedLoopGain(INPUT feedbackValue)
			{
				return OUTPUT(0);
			}

		};





		/*
		 * @brief	模糊参数归一化PID环节
		 *			该环节Ti与Td参数由Kp自动确定，并根据误差的变化量自动调整Kp值
		 *			给定一个反馈输入，该环节可输出一个闭环增益
		 *			需设定好采样周期和pid参数
		 *			
		 *			使用前请设定好周期、PID参数（仅Kp）、目标值
		 *			并且在一个周期后调用DeltaClosedLoopGain方法，传入实测值作为反馈值
		 *			该方法返回一个误差调节量
		 */
		template< class OUTPUT, class INPUT >
		struct FuzzyUniformizedPIDElement
			: public UniformizedPIDElement<OUTPUT, INPUT>
		{


			typedef typename ArithTraits<OUTPUT>::Signed DELTA_OUTPUT;
			typedef typename ArithTraits<INPUT>::MaxBitsSigned ErrorType;
			

		protected:

			using UniformizedPIDElement<OUTPUT, INPUT>::TargetValue;
			using UniformizedPIDElement<OUTPUT, INPUT>::CurrentValue;
			using UniformizedPIDElement<OUTPUT, INPUT>::outputEnabled;
			using UniformizedPIDElement<OUTPUT, INPUT>::Kp;
			using UniformizedPIDElement<OUTPUT, INPUT>::Error_n2;
			using UniformizedPIDElement<OUTPUT, INPUT>::Error_n1;


		public:

			FuzzyUniformizedPIDElement(float period)
				: UniformizedPIDElement<OUTPUT, INPUT>(period)
				, DeltaErrorRate(0)
				, DeltaDeltaErrorRate(0)
			{
				
			}



			/*
			 * @brief	PID闭环增益调节偏移量
			 * @param	feedbackValue 反馈量
			 * @retval	调节器偏移量
			 */
			virtual DELTA_OUTPUT DeltaClosedLoopGain(INPUT feedbackValue)
			{		
				CurrentValue = feedbackValue;
				const ErrorType Error = TargetValue - CurrentValue;		// 本次误差量
				ErrorType PrevDeltaErrorRate = DeltaErrorRate;			// 保存上次误差变化率
				
				DeltaErrorRate = (Error - Error_n1) / Error_n1;			// 计算本次误差变化率
				DeltaDeltaErrorRate = (DeltaErrorRate - PrevDeltaErrorRate) / PrevDeltaErrorRate;	// 计算本次误差变化率的变化率

				// 模糊决策
				
				float AdjustedKp = 0;

				// 误差变化率不大时，不需修正
				if (abs(DeltaErrorRate) < 0.2)
				{
					AdjustedKp = Kp;
				}
				else
				{
					// 变化率0.8限幅
					if (DeltaErrorRate > 0.8)
					{
						DeltaErrorRate = 0.8;
					}
					else if (DeltaErrorRate < -0.8)
					{
						DeltaErrorRate = -0.8;
					}
					AdjustedKp = Kp * (1 + DeltaErrorRate * 0.8);
				}
					
				
				// 参数归一化PID增益表达式
				DELTA_OUTPUT deltaOutput = AdjustedKp * (2.45 * Error - 3.5 * Error_n1 + 1.25 * Error_n2);

				// 保存误差量数据
				Error_n2 = Error_n1;
				Error_n1 = Error;

				if (!outputEnabled) 
				{
					return DELTA_OUTPUT(0);
				}
				else
				{
					return deltaOutput;
				}
			}



			// 误差变化率与误差变化率的变化率
			ErrorType DeltaErrorRate, DeltaDeltaErrorRate;


		
		};



		
	} /* namespace ACS */

} /* namespace adxM3 */

#endif  /*_ADX_APP_PIDELEMENT_H_*/
