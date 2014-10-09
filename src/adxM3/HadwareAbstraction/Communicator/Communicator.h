
#ifndef _COMMUNICATOR_H_
#define _COMMUNICATOR_H_

#include "../../common/defs.h"
#include "../Event/EventObject.h"
#include "../GPIO/GeneralPurposeInputOutputPin.h"
#include "../PeripheralBahaviour/PeripheralOnOff.h"

namespace adxM3
{








	/*
	 * @brief	通讯类外设接收机抽象
	 *			每个接收机包含一个接收引脚
	 *			并声明了接收接口方法
	 */
	class Receiver
	{
	public:

		typedef unsigned int size_type;



		/*
		 * @brief	通讯器接收接口方法
		 * @param	in 接收的数据存放区的地址
		 *			接收帧支持字节、半字、字 类型数据，由通讯外设根据实际情况实现
		 *			接收前请务必配置好数据位
		 * @notice	在派生类中实现时如果不支持请返回false
		 * @ovride	派生类至少需要实现UINT8数据类型的接收方法，
		 *			其余类型默认分割成多个字节逐个接收，低字节在前（小端模式）
		 *			派生类如果支持别的半字或字的接收，请override该方法
		 * @retval	接收成功返回true，否则返回false
		 */
		virtual bool Receive(UINT8& in) = 0;
		virtual bool Receive(UINT16& in)
		{
			return Receive(&in, sizeof(in));
		}
		virtual bool Receive(UINT32& in)
		{
			return Receive(&in, sizeof(in));
		}
		virtual bool Receive(void* buffer, size_type bufferSize)
		{
			UINT8* _buffer = static_cast<UINT8*>(buffer);
			while (bufferSize--)
			{
				Receive(*_buffer++);
			}
			while (!ReceiveCompleted());
			return true;
		}



		/*
		 * @brief	通讯器接收机接收到有效数据判断
		 * @retval	接收机接收到有效数据则返回true，否则返回false
		 * @ovride	派生类通讯器务必实现是接口，否则无法保证基本的通讯功能
		 */
		virtual bool DataAvailable() const = 0;



		/*
		 * @brief	通讯器接收机接收数据完成判断
		 * @retval	接收机接收数据完成则返回true，否则返回false
		 * @notice	当数据已经全部从外设的接收缓冲区中读入内存后则视为接收完成
		 */
		virtual bool ReceiveCompleted() const = 0;



		/*
		 * @brief	通讯器接收机使能控制（开启/关闭接收机，检测使能状态）
		 * @retval	成功开启或关闭则返回1，否则返回0
		 * @ovride	派生类根据情况重写该方法，若无法实现接收机开启或关闭请返回0
		 * @notcie	【非全双工】模式下EnableReceive时请不要使用EnableTransmitter方法
		 *			DisableReceiver时请不要使用DisableTransmitter方法
		 */
		virtual int EnableReceiver() = 0;
		virtual int DisableReceiver() = 0;
		virtual bool IsReceiverEnabled() const = 0;


	};




	/*
	 * @brief	通讯类外设发送机抽象
	 *			每个发送机包含一个发送引脚
	 *			并声明了发送接口方法
	 */
	class Transmitter
	{
	public:

		typedef unsigned int size_type;



		/*
		 * @brief	通讯器发送接口方法
		 * @param	out 待发送数据的地址
		 *			发送帧支持字节、半字、字 类型数据，由通讯外设根据实际情况实现
		 *			发送前请务必配置好数据位
		 * @notice	在派生类中实现时如果不支持请返回false
		 * @ovride	派生类至少需要实现UINT8数据类型的发送方法，
		 *			其余类型默认分割成多个字节逐个发送，低字节在前（小端模式）
		 *			派生类如果支持别的半字或字的发送，请override该方法
		 * @retval	发送成功返回true，否则返回false
		 */
		virtual bool Send(const UINT8 out) = 0;
		virtual bool Send(const UINT16 out)
		{
			return Send(&out, sizeof(out));
		}
		virtual bool Send(const UINT32 out)
		{
			return Send(&out, sizeof(out));
		}
		virtual bool Send(const void* buffer, size_type bufferSize)
		{
			const UINT8* _buffer = static_cast<const UINT8*>(buffer);
			while (bufferSize--)
			{
				Send(*_buffer++);
			}
			while (!SendComplete());
			return true;
		}



		/*
		 * @brief	通讯器发送机就绪判断
		 * @retval	发送机就绪则返回true，否则返回false
		 * @ovride	派生类通讯器若无法判断是否就绪请根据实际情况返回true或false
		 *			若不支持此功能请返回true
		 */
		virtual bool ReadyToSend() const = 0;



		/*
		 * @brief	通讯器发送机发送完成判断
		 * @retval	发送机发送完成则返回true，否则返回false
		 * @ovride	派生类通讯器若无法判断是否发送完成请根据实际情况返回true或false
		 *			若不支持此功能请返回true
		 */
		virtual bool SendComplete() const = 0;



		/*
		 * @brief	通讯器发送机使能控制（开启/关闭接收机，检测使能状态）
		 * @retval	成功开启或关闭则返回1，否则返回0
		 * @ovride	派生类根据情况重写该方法，若无法实现发送机开启或关闭请返回0
		 * @notcie	非全双工模式下EnableTransmitter时请不要使用EnableReceive方法
		 *			DisableReceiver时请不要使用DisableTransmitter方法
		 */
		virtual int EnableTransmitter() = 0;
		virtual int DisableTransmitter() = 0;
		virtual bool IsTransmitterEnabled() const = 0;



	};




	/*
	 * @brief	通讯类外设的基类，通讯装置要使用通讯流请实现该接口
	 *			该接口包含基本收发控制与收发状态判断方法
	 *			通讯通讯器一定也是事件对象，派生类中需要实现事件控制的方法
	 */
	class Communicator 
		: public EventObject
		, public Receiver
		, public Transmitter
		, public PeripheralOnOff
	{

	public:


		enum
		{
			UseBufferedTransfer = 1
		};


		typedef unsigned int size_type;



		/*
		 * @brief	收发模式选项
		 */
		enum IOM
		{
			Rx					= 0x01,
			Tx					= 0x02
		};




		/*
		 * @brief	通讯器事件类型定义
		 *			adxM3所支持的异步通讯器所有事件都在此
		 */
		enum EVT
		{
			READY_TO_SEND,									// 发送机就绪
			DATE_AVAILABLE,									// 接收到了有效数据
			SEND_COMPLETED,									// 发送完成
			CLEAR_TO_SEND,									// CTS标志
			RECEIVE_BUFFER_OVERFLOW,						// 接收缓冲区溢出
			TRANSMIT_BUFFER_OVERFLOW,						// 发送缓冲区溢出
			BUS_IDLE,										// 总线空闲
			PARITY_ERROR,									// 校验错误
			FRAME_ERROR,									// 帧错误
			EVT_COUNT
		};

		 

	protected:

		virtual ~Communicator() {}

	};




	
	/*
	 * @brief	可配置通讯类外设
	 *			在该类中定义所有通讯器功能配置的接口方法
	 *			包括参数配置和参数读取
	 */
	class ConfigurableCommunicator
		: public Communicator
	{
	public:



		/*
		 * @brief	通讯器波特率配置
		 * @param	baudrate 波特率值，根据实际情况设置波特率，单位bps
		 * @retval	成功配置则返回1，否则返回0
		 * @ovride	派生类务必实现波特率的配置
		 *			若派生类通讯器没有连续波特率可配置能力，可配置为最近似的波特率值
		 */
		virtual int SetBaudRate(const unsigned int baudrate) = 0;
		


		/*
		 * @brief	通讯器获取波特率
		 * @retval	返回波特率值，单位bps
		 * @ovride	若派生类不支持该方法请返回0
		 */
		virtual unsigned int GetBaudRate() const = 0;

		

		/*
		 * @brief	通讯器数据位配置
		 * @param	datebits 数据位，单次传输中数据的有效位数
		 * @retval	成功配置则返回1，否则返回0
		 * @notice	数据位通常为7,8,16,32,64等
		 * @ovride	若派生类通讯器没有连续数据位可配置能力，可配置为最近似的数据位值
		 */
		virtual int SetDataBits(const unsigned int databits) = 0;



		/*
		 * @brief	通讯器获取数据位
		 * @retval	返回单次通讯中传输的有效数据位数
		 * @ovride	若派生类不支持该方法请返回0
		 */
		virtual unsigned int GetDataBits() const = 0;



		/*
		 * @brief	校验模式选项
		 *			adxM3所支持的常规校验模式
		 */
		enum PARITY
		{
			PARITY_NONE,				// 无校验
			PARITY_EVEN,				// 偶校验
			PARITY_ODD,					// 奇校验
			PARITY_SPACE,				// 空格校验
			PARITY_MARK,				// 标志校验
			PARITY_CRC					// 冗余循环校验
		};



		/*
		 * @brief	通讯器校验模式设置
		 * @param	parity 校验模式枚举，可以是PARITY枚举的任何一个
		 *				@arg PARITY_NONE
		 *				@arg PARITY_EVEN
		 *				...
		 *				@arg PARITY_CRC
		 * @retval	成功配置则返回1，否则返回0
		 * @ovride	若派生类通讯器不支持某些模式则返回0
		 */
		virtual int SetParityMode(const PARITY parity) = 0;



		/*
		 * @brief	通讯器获取校验模式
		 * @retval	当前的校验模式
		 * @ovride	若派生类通讯器不支持该方法则返回PARITY_NONE
		 */
		virtual PARITY GetParityMode() const = 0;



		/*
		 * @brief	通讯器发送校验字（一般用于CRC校验模式最后发送一个校验字）
		 * @ovride	若派生类通讯器不支持该方法则返回0
		 */
		virtual int SendParityWord() = 0;



		enum ERT
		{
			ParityError		= 0x0001,
			Overflow		= 0x0002,
			Underflow		= 0x0004
		};



		/*
		 * @brief	通讯器获取错误状态
		 * @ovride	无错误时返回0，否则按照ERT中的错误按位或组合
		 */
		virtual int GetErrors() const = 0;




		/*
		 * @brief	首传输位选项
		 */
		enum FSB
		{
			MSB,						// Most Significant Bit
			LSB							// Least Significant Bit
		};



		/*
		 * @brief	通讯器设置首传输位
		 * @param	firstbit 首传输位枚举
		 *				@arg MSB 最高位先传
		 *				@arg LSB 最低位先传
		 * @retval	成功配置则返回1，否则返回0
		 * @ovride	若派生类通讯器不支持某些模式则返回0
		 */
		virtual int SetFirstBit(const FSB firstbit) = 0;



		/*
		 * @brief	通讯器获取首传输位模式
		 * @retval	当前首传输位模式
		 * @ovride	传输总有一个首传输位模式，派生类通讯器总可以实现此方法
		 */
		virtual FSB GetFirstBit() const = 0;




		/*
		 * @brief	通讯器模式配置选项
		 *			对于无主从之分的通讯器可选用SUPER模式
		 */
		enum MSM
		{
			SLAVE,						// 从机模式
			MASTER,						// 主机模式
			SUPER,						// 超级模式，既非主机又非从机
		};



		/*
		 * @brief	通讯器主从模式设置
		 * @param	mode 模式枚举
		 *				@arg SUPER 超级主机模式
		 *				@arg MASTER 主从通讯器主机模式
		 *				@arg SLAVE 主从通讯器从机模式
		 * @retval	成功配置则返回1，否则返回0
		 */
		virtual int SetMode(const MSM mode) = 0;



		/*
		 * @brief	通讯器主从模式设置
		 * @retval	派生类呜主模式从模式之分时返回SUPER，否则按实际情况返回
		 */
		virtual MSM GetMode() const = 0;



		/*
		 * @brief	传输工作模式选项
		 * @note	半双工与单工最大的区别是
		 *			单工模式下发送线路永远是发送线路，接收线路永远是接收线路
		 *			半双工模式发送与接收分时复用同一线路
		 */
		enum TRM
		{
			FullDuplex,					// 全双工
			HalfDuplex_Tx,				// 半双工发送
			HalfDuplex_Rx,				// 半双工接收
			Simplex_Tx,					// 单工发送
			Simplex_Rx					// 单工接收
		};



		/*
		 * @brief	通讯器设置传输工作模式
		 * @param	transmission_mode 传输工作模式枚举
		 *				@arg FullDuplex 全双工
		 *				@arg HalfDuplex_Tx/HalfDuplex_Rx 半双工发/半双工收
		 *				@arg Simplex_Tx/Simplex_Rx 单工发/单工收
		 * @retval	成功配置则返回1，否则返回0
		 * @ovride	若派生类通讯器不支持某些模式则返回0
		 */
		virtual int SetTransmissionMode(const TRM transmission_mode) = 0;



		/*
		 * @brief	通讯器获取传输工作模式
		 * @retval	传输工作模式
		 * @ovride	传输总有一个传输工作模式，派生类通讯器总可以实现此方法
		 */
		virtual TRM GetTransmissionMode() const = 0;




	};


}

#endif  /*_COMMUNICATOR_H_*/
