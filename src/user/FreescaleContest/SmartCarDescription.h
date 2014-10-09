
#ifndef _USER_SMARTCARDESCRIPTION_H_
#define _USER_SMARTCARDESCRIPTION_H_

#include "adxM3.h"
#include "adx_K60.h"

using namespace K60;
using namespace adxM3;


#include "DirectionControl.h"
#include "DualMotorMotionControlSystem.h"
#include "AutoNavigateMotiveSystem.h"


namespace FreescaleContest
{





	namespace NewCar
	{

		typedef Peripheral<UART4> URTType;
		typedef Peripheral<PIT>::CH<1> Timer1Type;
		typedef Peripheral<PIT>::CH<2> Timer2Type;


		// FTM0脉冲周期锁定为 100us 当前仅用于控制电机 10kHz
		const UINT32 MotorPWMPeriod = 100;
		typedef Peripheral<FTM0> MotorFTMType;

		// futaba舵机使用14.285ms PWM周期
		const UINT32 ServoPWMPeriod = 14285;
		typedef Peripheral<FTM1> ServoFTMType;



		typedef Voltmeter< Peripheral<_ADC0_>::CH<1> > BatteryMonitorType;

		typedef MultiFrequencyBeeper< Peripheral<GPIOE>::ConstPin<26> > BeeperType;

		typedef Peripheral<GPIOA>::ConstPin<19> LEDIndicatorType;



		typedef AutoNavigateMotiveSystem
		<
			DualMotorMotionControlSystem
			<

				/* LMotor Driver */
				PowerControlSystem
				<
					HBridgeMotorDriver
					<
						ForwardSignal< Peripheral<FTM0>::PWM<6> >,
						BackwardSignal< Peripheral<FTM0>::PWM<7> >
					>,
					AbsVelometer
					<
						ZC_AbsSpeedEncoder
						<
							0xFFF,
							SynchronousSerialInterface
							<
								_CSn< Peripheral<GPIOE>::ConstPin<5> >,
								_CLK< Peripheral<GPIOB>::ConstPin<9> >,
								_DAT< Peripheral<GPIOA>::ConstPin<5> >,
								SystemType
							>
						>
					>		
				>,

				/* RMotor Driver */
				PowerControlSystem
				<
					HBridgeMotorDriver
					<
						ForwardSignal< Peripheral<FTM0>::PWM<5> >,
						BackwardSignal< Peripheral<FTM0>::PWM<4> >
					>,
					AbsVelometer
					<
						ZC_AbsSpeedEncoder
						<
							0xFFF,
							SynchronousSerialInterface
							<
								_CSn< Peripheral<GPIOE>::ConstPin<6> >,
								_CLK< Peripheral<GPIOB>::ConstPin<9> >,
								_DAT< Peripheral<GPIOA>::ConstPin<5> >,
								SystemType
							>
						>
					>		
				>,

				/* axis sync control policy */
				AxisAsynchronouslyControl
			>,
			MatrixDirectionControlSystem
			<
				/* CCD sensor */
				LinearCCD
				<
					128,
					Peripheral<GPIOB>::ConstPin<17>,
					Peripheral<GPIOD>::ConstPin<0>,
					Peripheral<_ADC0_>::CH<0>,
					SystemType
				>,

				/* actuator */
				Servo
				<
					MinPulseWidth<1315 - 40 + 3>,	// 1456 - 80
					MaxPulseWidth<1531 - 20 + 3>,	// 1640 + 80
					MinAngle<-100>,
					MaxAngle<100>,
					ControlSignal
					< 
						Peripheral<FTM1>::PWM<0>
					>
				>
			>,

			/* Tick Timer */
			Peripheral<PIT>::CH<0>,

			/* Beeper indicator */
			Beeper< Peripheral<GPIOE>::ConstPin<26> >,

			/* LED indecator */ 
			LEDIndicatorType,

			/* System Cop */
			SystemType,

			/* black line detector */
			Peripheral<GPIOE>::ConstPin<0>

		> iCarType; 




		// 传感器值类型选择拨码开关线路定义
		typedef Peripheral<GPIOC>::ConstPin<17> iCarSensorValueTypeSwitchType;


		/* tree counter */
		typedef Peripheral<EXTIE>::PinSource<0> TreeCounter;


	} /* namespace NewCar */




}


#endif  /*_USER_SMARTCARDESCRIPTION_H_*/
