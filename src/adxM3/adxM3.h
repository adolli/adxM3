

/**
 * @brief	adxM3是一个以arm cortex-m3为核心的处理器的抽象模型接口，
 *			可作为同核心不同处理器之间代码移植的中间平台，
 *			实现用户代码0修改的快速平台移植。
 *			并对外设模型进一步开发了效用代码库。
 *
 */


#ifndef _ADXM3_H_
#define _ADXM3_H_



/*
 * @folder	common
 * @brief	硬件抽象层无关的通用类
 *			包含基本类型定义、基本数据结构、基本流、图形描述、辅助功能等
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
 * @brief	硬件抽象架构，包含对各种外设或器件的抽象描述，硬件访问的接口
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
 * @brief	对硬件抽象的进一步功能设计，包含通讯器流、数据采集、效用控制等
 */

#include "Utility/Stream/StreamBase.h"
#include "Utility/Stream/InputStream.h"
#include "Utility/Stream/OutputStream.h"
#include "Utility/Stream/IOStream.h"
#include "Utility/Stream/TextStream.h"



/*
 * @folder	Component Model
 * @brief	部件与模块的抽象，使用adxM3所提供的抽象硬件搭建的各个部件与模块
 *			当实现了抽象硬件接口后，抽象部件则可使用
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
 * @brief	adxM3测试用例
 */

#include "example/example.h"




#endif  /*_ADXM3_H_*/
