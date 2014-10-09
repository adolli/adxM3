
#ifndef _ADX_APP_SERVO_H_
#define _ADX_APP_SERVO_H_

#include "../common/defs.h"

namespace adxM3
{


	template< UINT32 Width >
	struct MinPulseWidth
	{
		enum { ResultValue = Width };
	};


	template< UINT32 Width >
	struct MaxPulseWidth
	{
		enum { ResultValue = Width };
	};

	template< INT32 Width >
	struct MinAngle
	{
		enum { ResultValue = Width };
	};


	template< INT32 Width >
	struct MaxAngle
	{
		enum { ResultValue = Width };
	};


	template< class T >
	struct ControlSignal
	{
		typedef T ResultType;
	};





	template< class, class, class, class, class > class Servo;



	/*
	 * @brief	���������PWM�źŽǶȿ��ƣ�
	 * @tparam	Period PWM�ź����ڣ���λus
	 *			MinWidth ��СPWM�ź�������λus
	 *			MaxWidth ���PWM�ź�������λus
	 *			minAngle ��������������С�Ƕȣ���λ��
	 *			maxAngle ���������������Ƕȣ���λ��
	 *			SignalType �����ź����ͣ�Ҫ��Ϊ���ڹ̶�ռ�ձȿɵ����źţ�Ҫ��ʵ�����·���
	 *				@required EnableOutput() ���ʹ�ܿ���
	 *				@required SetPulseWidth(UINT32 wid) ��������
	 *					@arg wid ����ʱ������λus
	 *				@required SignalType(float duty) ���캯��Ҫ��һ������
	 *					@arg duty ռ�ձȣ���Χ0~1������
	 *			
	 */
	template
	< 
		UINT32 minWidth,
		UINT32 maxWidth,
		INT32 minAngle,
		INT32 maxAngle,
		class SignalType
	> 
	class Servo
	<
		MinPulseWidth<minWidth>,
		MaxPulseWidth<maxWidth>,
		MinAngle<minAngle>,
		MaxAngle<maxAngle>,
		ControlSignal<SignalType>
	>
	{
	public:


		typedef INT32 DirValueType;


		Servo()
			: CurrentAngle(0)
		{
			STATIC_ASSERT(minWidth < maxWidth,
				min_pulse_width_should_less_than_max_pulse_width);
			STATIC_ASSERT(minAngle < maxAngle,
				min_angle_should_less_than_max_angle);

			SetAngle(0);
		}




		/*
		 * @brief	ʹ�ܿ���
		 */
		void Enable()
		{
			PulseWidthSignal.EnableOutput();
		}
		void Disable()
		{
			PulseWidthSignal.DisableOutput();
		}





		enum
		{
			AngleDiff		= maxAngle - minAngle,
			PulseWidthDiff	= maxWidth - minWidth
		};



		/*
		 * @brief	���ö���Ƕ�
		 * @param	angle ���ָ��Ƕȣ�ָ��������ʱΪ0����Χ��Ҫ������С��������֮��
		 */
		void SetAngle(DirValueType angle)
		{
			// �޶���Χ
			RangeLimit(minAngle, <=, angle, <=, maxAngle);

			// �ӿ���Ӧ�ٶȣ���ʹ�ø�����
			PulseWidthSignal.SetPulseWidth(
				(angle - minAngle) * PulseWidthDiff / AngleDiff + minWidth);

			CurrentAngle = angle;
		}



		/*
		 * @brief	���ö���Ƕ�
		 * @retval	��ǰ���ָ��ĽǶ�
		 */
		DirValueType GetAngle() const
		{
			//return float(PulseWidthSignal.GetPulseWidth() - minWidth) * AngleDiff / PulseWidthDiff + minAngle;
			return CurrentAngle;
		}



		/*
		 * @brief	�����趨����С/���Ƕ�
		 */
		void SetToMinAngle()
		{
			PulseWidthSignal.SetPulseWidth(minWidth);
			CurrentAngle = minAngle;
		}
		void SetToMaxAngle()
		{
			PulseWidthSignal.SetPulseWidth(maxWidth);
			CurrentAngle = maxAngle;
		}



		UINT32 debug_GetPulseWidth() const
		{
			return PulseWidthSignal.GetPulseWidth();
		}



	private:

		SignalType PulseWidthSignal;
		
		// Ϊ����PWM�ź�ͬ�����ϵ����⣬���浱ǰ�Ƕ�ֵ
		DirValueType CurrentAngle;


	};



}

#endif  /*_ADX_APP_SERVO_H_*/
