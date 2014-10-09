

/**
 * @brief	adxM3��һ����arm cortex-m3Ϊ���ĵĴ������ĳ���ģ�ͽӿڣ�
 *			����Ϊͬ���Ĳ�ͬ������֮�������ֲ���м�ƽ̨��
 *			ʵ���û�����0�޸ĵĿ���ƽ̨��ֲ��
 *			��������ģ�ͽ�һ��������Ч�ô���⡣
 *
 */


#ifndef _ADXM3_H_
#define _ADXM3_H_



/*
 * @folder	common
 * @brief	Ӳ��������޹ص�ͨ����
 *			�����������Ͷ��塢�������ݽṹ����������ͼ���������������ܵ�
 */
#include "common/defs.h"
#include "common/TypeTraits.h"
#include "common/auxiliary.h"
#include "common/assert.h"
#include "common/Function.h"

#include "common/graphical/color.h"
#include "common/Graphical/BasicGraphics.h"
#include "common/Graphical/graphical.h"

#include "common/DataStructures/List.h"
#include "common/DataStructures/DataBuffer.h"

#include "common/Filter/KalmanFilter.h"
#include "common/Filter/GaussianFilter.h"
#include "common/Filter/DigitalFilter.h"






/*
 * @folder	Hadware Abstraction
 * @brief	Ӳ������ܹ��������Ը�������������ĳ���������Ӳ�����ʵĽӿ�
 */
#include "HadwareAbstraction/Event/EventObject.h"
#include "HadwareAbstraction/Event/EventListenerWrapper.h"
#include "HadwareAbstraction/Event/ReceiveListener.h"
#include "HadwareAbstraction/Event/TimerUpdateListener.h"

#include "HadwareAbstraction/PeripheralBahaviour/BaseAccessible.h"
#include "HadwareAbstraction/PeripheralBahaviour/PeripheralOnOff.h"

#include "HadwareAbstraction/GPIO/MultiplexedPin.h"
#include "HadwareAbstraction/GPIO/GeneralPurposeInputOutput.h"
#include "HadwareAbstraction/GPIO/GeneralPurposeInputOutputPort.h"
#include "HadwareAbstraction/GPIO/GeneralPurposeInputOutputPin.h"
#include "HadwareAbstraction/GPIO/ExternalInterrupt.h"

#include "HadwareAbstraction/Communicator/Communicator.h"
#include "HadwareAbstraction/Communicator/AsynchronousCommunicator.h"
#include "HadwareAbstraction/Communicator/SynchronousCommunicator.h"
#include "HadwareAbstraction/Communicator/SynchronousSerialInterface.h"

#include "HadwareAbstraction/Timer/Timer.h"
#include "HadwareAbstraction/Timer/TimerIOEnabler.h"
#include "HadwareAbstraction/Timer/PulseWidthModulator.h"
#include "HadwareAbstraction/Timer/PulseWidthGenerator.h"
#include "HadwareAbstraction/Timer/SoftPulseWidthModulator.h"
#include "HadwareAbstraction/Timer/BlockingDelayController.h"

#include "HadwareAbstraction/Sensor/Sensor.h"
#include "HadwareAbstraction/Sensor/MultiChannelVoltMeter.h"




/*
 * @folder	Utility
 * @brief	��Ӳ������Ľ�һ��������ƣ�����ͨѶ���������ݲɼ���Ч�ÿ��Ƶ�
 */

#include "Utility/Stream/StreamBase.h"
#include "Utility/Stream/InputStream.h"
#include "Utility/Stream/OutputStream.h"
#include "Utility/Stream/IOStream.h"
#include "Utility/Stream/TextStream.h"



/*
 * @folder	Component Model
 * @brief	������ģ��ĳ���ʹ��adxM3���ṩ�ĳ���Ӳ����ĸ���������ģ��
 *			��ʵ���˳���Ӳ���ӿں󣬳��󲿼����ʹ��
 */

#include "ComponentModel/nRF24L01.h"
#include "ComponentModel/MAX7219.h"
#include "ComponentModel/MAX7219_MatrixLED.h"
#include "ComponentModel/NOKIA5110.h"
#include "ComponentModel/HBridgeMotorDriver.h"
#include "ComponentModel/HCTL20xx.h"
#include "ComponentModel/ILIx.h"
#include "ComponentModel/ZC_SpeedEncoder.h"
#include "ComponentModel/LinearCCD.h"
#include "ComponentModel/Displayer.h"




#include "Application/PIDElement.h"
#include "Application/Servo.h"
#include "Application/SensorMatrix.h"
#include "Application/Voltmeter.h"
#include "Application/Velometer.h"
#include "Application/SoftVelometer.h"
#include "Application/Buzzer.h"
#include "Application/AbsVelometer.h"




/*
 * @folder	example
 * @brief	adxM3��������
 */

#include "example/example.h"




#endif  /*_ADXM3_H_*/
