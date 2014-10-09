
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
		 * @brief	���ڻ���
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
			 * @brief	�ջ����溯��
			 */
			virtual DELTA_OUTPUT DeltaClosedLoopGain(INPUT feedbackValue) = 0;
			virtual OUTPUT ClosedLoopGain(INPUT feedbackValue) = 0;


			
			/*
			 * @brief	����Ŀ���ٶ�ֵ
			 * @param	target_speed Ŀ���ٶȣ���λ����ÿ��
			 */
			void SetTargetValue(INPUT target)
			{
				TargetValue = target;
			}



			/*
			 * @brief	��ȡ��ǰ�ٶ�ֵ
			 * @retval	��ǰ�ٶ�ֵ����λ mm/s
			 */
			INPUT GetCurrentValue() const
			{
				return CurrentValue;
			}



			/*
			 * @brief	ʹ��/��ֹ����
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
			 * @brief	��λ���
			 */
			void ResetErrors()
			{
				Error_n1 = Error_n2 = ErrorType(0);
			}



		protected:


			// ��������
			float T;

			// ������һ����������һ�ε���һ�����
			ErrorType Error_n1, Error_n2;

		public:

			// ��ǰ/Ŀ���ٶ�ֵ����λmm/s
			volatile INPUT CurrentValue;
			volatile INPUT TargetValue;


		protected:

			// ʹ��״̬
			volatile bool outputEnabled;

		};







		/*
		 * @brief	PD����
		 *			����һ���������룬�û��ڿ����һ���ջ�����
		 *			���趨�ò������ں�pd����
		 *			
		 *			ʹ��ǰ���趨�����ڡ�PID������Ŀ��ֵ
		 *			������һ�����ں����DeltaClosedLoopGain����������ʵ��ֵ��Ϊ����ֵ
		 *			�÷�������һ����������
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
			 * @brief	PID�ջ��������ƫ����
			 * @param	feedbackValue ������
			 * @retval	������ƫ����
			 */
			virtual OUTPUT ClosedLoopGain(INPUT feedbackValue)
			{
				CurrentValue = feedbackValue;
				const ErrorType Error = TargetValue - CurrentValue;		// ���������
				const ErrorType dError = Error - Error_n1;
				OUTPUT Output = Kp * Error + Kd * dError;

				// �������������
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
			 * @brief	����PD����ֵ
			 * @param	kp ����ϵ��
			 *			td ΢��ʱ�䳣��
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

			// PD����
			float Kp, Kd; 

			
		};




		struct UsingTiTd {};
		struct UsingKiKd {};	


		template< class OUTPUT, class INPUT, class ParamPolicy = UsingTiTd >
		struct PIDElement;




		/*
		 * @brief	PID����
		 *			����һ���������룬�û��ڿ����һ���ջ�����
		 *			���趨�ò������ں�pid����
		 *			
		 *			ʹ��ǰ���趨�����ڡ�PID������Ŀ��ֵ
		 *			������һ�����ں����DeltaClosedLoopGain����������ʵ��ֵ��Ϊ����ֵ
		 *			�÷�������һ����������
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
			 * @brief	PID�ջ��������ƫ����
			 * @param	feedbackValue ������
			 * @retval	������ƫ����
			 */
			virtual DELTA_OUTPUT DeltaClosedLoopGain(INPUT feedbackValue)
			{
				CurrentValue = feedbackValue;
				const ErrorType Error = TargetValue - CurrentValue;		// ���������
				const float A = Kp * (1 + T / Ti + Td / T);
				const float B = Kp * (1 + 2 * Td / T);
				const float C = Kp * Td / T;
				DELTA_OUTPUT deltaOutput = A * Error - B * Error_n1 + C * Error_n2;


				// �������������
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
			 * @brief	����PID����ֵ
			 * @param	kp ����ϵ��
			 *			ti ����ʱ�䳣��
			 *			td ΢��ʱ�䳣��
			 */
			void SetParameters(float kp, float ti, float td)
			{
				Kp = kp;
				Ti = ti;
				Td = td;
			}





		protected:
				
			// PID����
			float Kp, Ti, Td; 


		private:


			virtual OUTPUT ClosedLoopGain(INPUT feedbackValue)
			{
				return OUTPUT(0);
			}

		};




		/*
		 * @brief	PID����
		 *			����һ���������룬�û��ڿ����һ���ջ�����
		 *			���趨�ò������ں�pid����
		 *			
		 *			ʹ��ǰ���趨�����ڡ�PID������Ŀ��ֵ
		 *			������һ�����ں����DeltaClosedLoopGain����������ʵ��ֵ��Ϊ����ֵ
		 *			�÷�������һ����������
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
			 * @brief	PID�ջ��������ƫ����
			 * @param	feedbackValue ������
			 * @retval	������ƫ����
			 */
			virtual DELTA_OUTPUT DeltaClosedLoopGain(INPUT feedbackValue)
			{
				CurrentValue = feedbackValue;
				const ErrorType Error = TargetValue - CurrentValue;		// ���������
				
				const ErrorType dError = Error - Error_n1;
				const ErrorType d2Error = Error - 2 * Error_n1 + Error_n2;
				DELTA_OUTPUT deltaOutput = Kp * dError + Ki * Error + Kd * d2Error;

				// �������������
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
				const ErrorType Error = TargetValue - CurrentValue;		// ���������

				SumError += Error;
				const ErrorType dError = Error - Error_n1;
				OUTPUT Output = Kp * Error + Ki * SumError + Kd * dError;


				// �������������
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
			 * @brief	����PID����ֵ
			 * @param	kp ����ϵ��
			 *			ti ����ʱ�䳣��
			 *			td ΢��ʱ�䳣��
			 */
			void SetParameters(float kp, float ki, float kd)
			{
				Kp = kp;
				Ki = ki;
				Kd = kd;
			}





		protected:
				
			// PID����
			float Kp, Ki, Kd; 

			ErrorType SumError;


		private:


			

		};






		/*
		 * @brief	PID����
		 *			����һ���������룬�û��ڿ����һ���ջ�����
		 *			���趨�ò������ں�pid����
		 *			
		 *			ʹ��ǰ���趨�����ڡ�PID������Ŀ��ֵ
		 *			������һ�����ں����DeltaClosedLoopGain����������ʵ��ֵ��Ϊ����ֵ
		 *			�÷�������һ����������
		 *			
		 * @tparam	ALPHA Ϊ�����ͺ����� ALPHA := Tf / (Tf + T); Tf�ǹ��Ի���ʱ�䳣��
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
			 * @brief	PID�ջ��������ƫ����
			 * @param	feedbackValue ������
			 * @retval	������ƫ����
			 */
			virtual DELTA_OUTPUT DeltaClosedLoopGain(INPUT feedbackValue)
			{
				CurrentValue = feedbackValue;
				const ErrorType Error = TargetValue - CurrentValue;		// ���������
				const float A = Kp * (1 + T / Ti + Td / T);
				const float B = Kp * (1 + 2 * Td / T);
				const float C = Kp * Td / T;
				
				// ��u'(k)��
				DELTA_OUTPUT derivedDeltaOutput = A * Error - B * Error_n1 + C * Error_n2;

				DELTA_OUTPUT deltaOutput = ALPHA * PrevDeltaOutput + (1 - ALPHA) * derivedDeltaOutput;
				PrevDeltaOutput = deltaOutput;

				// �������������
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
		 * @brief	������һ��PID����
		 *			�û���Ti��Td������Kp�Զ�ȷ��
		 *			����һ���������룬�û��ڿ����һ���ջ�����
		 *			���趨�ò������ں�pid����
		 *			
		 *			ʹ��ǰ���趨�����ڡ�PID��������Kp����Ŀ��ֵ
		 *			������һ�����ں����DeltaClosedLoopGain����������ʵ��ֵ��Ϊ����ֵ
		 *			�÷�������һ����������
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
			 * @brief	PID�ջ��������ƫ����
			 * @param	feedbackValue ������
			 * @retval	������ƫ����
			 */
			virtual DELTA_OUTPUT DeltaClosedLoopGain(INPUT feedbackValue)
			{
				CurrentValue = feedbackValue;
				const ErrorType Error = TargetValue - CurrentValue;		// ���������
				
				// ������һ��PID������ʽ
				DELTA_OUTPUT deltaOutput = Kp * (2.45 * Error - 3.5 * Error_n1 + 1.25 * Error_n2);

				// �������������
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
			 * @brief	����PID����ֵ
			 * @param	kp ����ϵ��
			 *			ti ����ʱ�䳣��
			 *			td ΢��ʱ�䳣��
			 */
			void SetParameters(float kp)
			{
				Kp = kp;
			}




		protected:
			
			
			// PID����
			float Kp; 



		private:


			virtual OUTPUT ClosedLoopGain(INPUT feedbackValue)
			{
				return OUTPUT(0);
			}

		};





		/*
		 * @brief	ģ��������һ��PID����
		 *			�û���Ti��Td������Kp�Զ�ȷ�������������ı仯���Զ�����Kpֵ
		 *			����һ���������룬�û��ڿ����һ���ջ�����
		 *			���趨�ò������ں�pid����
		 *			
		 *			ʹ��ǰ���趨�����ڡ�PID��������Kp����Ŀ��ֵ
		 *			������һ�����ں����DeltaClosedLoopGain����������ʵ��ֵ��Ϊ����ֵ
		 *			�÷�������һ����������
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
			 * @brief	PID�ջ��������ƫ����
			 * @param	feedbackValue ������
			 * @retval	������ƫ����
			 */
			virtual DELTA_OUTPUT DeltaClosedLoopGain(INPUT feedbackValue)
			{		
				CurrentValue = feedbackValue;
				const ErrorType Error = TargetValue - CurrentValue;		// ���������
				ErrorType PrevDeltaErrorRate = DeltaErrorRate;			// �����ϴ����仯��
				
				DeltaErrorRate = (Error - Error_n1) / Error_n1;			// ���㱾�����仯��
				DeltaDeltaErrorRate = (DeltaErrorRate - PrevDeltaErrorRate) / PrevDeltaErrorRate;	// ���㱾�����仯�ʵı仯��

				// ģ������
				
				float AdjustedKp = 0;

				// ���仯�ʲ���ʱ����������
				if (abs(DeltaErrorRate) < 0.2)
				{
					AdjustedKp = Kp;
				}
				else
				{
					// �仯��0.8�޷�
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
					
				
				// ������һ��PID������ʽ
				DELTA_OUTPUT deltaOutput = AdjustedKp * (2.45 * Error - 3.5 * Error_n1 + 1.25 * Error_n2);

				// �������������
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



			// ���仯�������仯�ʵı仯��
			ErrorType DeltaErrorRate, DeltaDeltaErrorRate;


		
		};



		
	} /* namespace ACS */

} /* namespace adxM3 */

#endif  /*_ADX_APP_PIDELEMENT_H_*/
