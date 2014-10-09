
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
	 * @brief	舵机驱动（PWM信号角度控制）
	 * @tparam	Period PWM信号周期，单位us
	 *			MinWidth 最小PWM信号脉宽，单位us
	 *			MaxWidth 最大PWM信号脉宽，单位us
	 *			minAngle 舵机相对于中轴最小角度，单位度
	 *			maxAngle 舵机相对于中轴最大角度，单位度
	 *			SignalType 控制信号类型，要求为周期固定占空比可调的信号，要求实现以下方法
	 *				@required EnableOutput() 输出使能控制
	 *				@required SetPulseWidth(UINT32 wid) 设置脉宽
	 *					@arg wid 脉宽时长，单位us
	 *				@required SignalType(float duty) 构造函数要求一个参数
	 *					@arg duty 占空比，范围0~1浮点数
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
		 * @brief	使能控制
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
		 * @brief	设置舵机角度
		 * @param	angle 舵机指向角度，指向正中央时为0，范围不要超出最小角与最大角之间
		 */
		void SetAngle(DirValueType angle)
		{
			// 限定范围
			RangeLimit(minAngle, <=, angle, <=, maxAngle);

			// 加快响应速度，不使用浮点数
			PulseWidthSignal.SetPulseWidth(
				(angle - minAngle) * PulseWidthDiff / AngleDiff + minWidth);

			CurrentAngle = angle;
		}



		/*
		 * @brief	设置舵机角度
		 * @retval	当前舵机指向的角度
		 */
		DirValueType GetAngle() const
		{
			//return float(PulseWidthSignal.GetPulseWidth() - minWidth) * AngleDiff / PulseWidthDiff + minAngle;
			return CurrentAngle;
		}



		/*
		 * @brief	快速设定到最小/最大角度
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
		
		// 为避免PWM信号同步性上的问题，保存当前角度值
		DirValueType CurrentAngle;


	};



}

#endif  /*_ADX_APP_SERVO_H_*/
