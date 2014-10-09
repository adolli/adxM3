 
#ifndef _ADX_K60_UART_H_
#define _ADX_K60_UART_H_

#include "adxM3.h"
#include "../../System/System.h"
#include "../PeripheralBase.h"
#include "../PeripheralInfo.h"
#include "../Port/GlobalPort.h"
#include "../Port/SignalPin.h"
#include "../DataFlow/DMA.h"


namespace K60
{

	/*
	 * @breif	通用异步通讯器外设
	 */
	template< UINT32 BASE >
	class Peripheral< UART_INFO<BASE> >
		: public adxM3::AsynchronousCommunicator
	{
	public:

		enum
		{
			UseBufferedTransfer = 1
		};


		typedef UART_INFO<BASE> PeripheralInfo;
		typedef ClockGateController<typename PeripheralInfo::ClockSourceGate> ClockSource;




		/*
		 * @brief	UART停止位配置/获取
		 * @param	stb 停止位枚举
		 *				@arg STOPBITS_05
		 *				@arg STOPBITS_10
		 *				@arg STOPBITS_15
		 *				@arg STOPBITS_20
		 * @retval	设置方法中，成功配置则返回1，否则返回0
		 *			获取方法中，不支持停止位返回STOPBITS_10，没有对应的停止位则返回匹配值最近的
		 */
		virtual int SetStopBits(const STB stb)
		{
			if (stb == AsynchronousCommunicator::STOPBITS_10)
			{
				return 1;
			}
			return 0;
		}
		virtual STB GetStopBits() const
		{
			return AsynchronousCommunicator::STOPBITS_10;
		}





		/*
		 * @brief	UART接收接口方法
		 * @param	in 接收的数据存放的位置
		 *			发送只支持字节，超过字节的数据类型将分割成多个字节连续发送
		 *			接收前请务必配置好数据位
		 * @retval	接收成功返回true，否则返回false
		 */
		virtual bool Receive(UINT8& in)
		{
			while (!DataAvailable());
			in = PeripheralInfo::BaseAccess()->D;
			return true;
		}





		/*
		 * @brief	UART接收机接收到有效数据判断
		 * @retval	接收机接收到有效数据则返回true，否则返回false
		 */
		virtual bool DataAvailable() const
		{
			return PeripheralInfo::BaseAccess()->S1 & _BV(UART_BitBandType::S1_RDRF);
		}



		/*
		 * @brief	UART接收机接收数据完成判断
		 * @retval	接收机接收数据完成则返回true，否则返回false
		 * @notice	当数据已经全部从外设的接收缓冲区中读入内存后则视为接收完成
		 */
		virtual bool ReceiveCompleted() const
		{
			// 测试S1[RDRF]位是否等于0
			return (~PeripheralInfo::BaseAccess()->S1) & _BV(UART_BitBandType::S1_RDRF);
		}



		/*
		 * @brief	UART接收机使能控制（开启/关闭接收机，检测使能状态）
		 * @retval	成功开启或关闭则返回1，否则返回0
		 * @notcie	【非全双工】模式下EnableReceive时请不要使用EnableTransmitter方法
		 *			DisableReceiver时请不要使用DisableTransmitter方法
		 */
		virtual int EnableReceiver()
		{
			RxPin.MultiplexConfig(RxSignal::MUX_ALT);
			PeripheralInfo::BaseAccess()->C2 |= _BV(UART_BitBandType::C2_RE);
			return 1;
		}
		virtual int DisableReceiver()
		{
			PeripheralInfo::BaseAccess()->C2 &= ~_BV(UART_BitBandType::C2_RE);
			RxPin.MultiplexConfig(PinMultiplexing::GPIO);
			return 1;
		}
		virtual bool IsReceiverEnabled() const
		{
			return PeripheralInfo::BaseAccess()->C2 & _BV(UART_BitBandType::C2_RE);
		}




		/*
		 * @brief	UART发送接口方法
		 * @param	out 待发送数据的地址
		 *			UART只支持按字节传输，由SetDataBits方法设定数据位
		 * @retval	发送成功返回true，否则返回false
		 */
		virtual bool Send(const UINT8 out)
		{
			while (!ReadyToSend() || !SendComplete());
			PeripheralInfo::BaseAccess()->D = out;
			return true;
		}



		/*
		 * @brief	UART发送接口方法（DMA）
		 * @param	buffer 待发送数据的地址
		 *			bufferSize 传输字节数
		 * @retval	发送成功返回true，否则返回false
		 */
		virtual bool Send(const void* buffer, size_type bufferSize)
		{	
			//typedef typename PeripheralInfo::DMAController DMAController;

			//// 这里留着，以后要调试多缓冲队列传输
			//while (!ReadyToSend() || !SendComplete());
			//Peripheral<DMAController> DMAC;

			//DMAC.template TransferRequest<UINT8* const, UINT8*>(
			//	(void*)&PeripheralInfo::BaseAccess()->D, buffer, (UINT16)bufferSize, 
			//	PeripheralInfo::DMATransmitRequest);

			//// 提交传输请求后保存链尾通道号
			//LastSubmittedDMAChannel = DMAC.GetAllocatedChannelIndex();
			//
			//PeripheralInfo::BaseAccess()->C5 |= _BV(UART_BitBandType::C5_TDMAS);
			//PeripheralInfo::BaseAccess()->C2 |= _BV(UART_BitBandType::C2_TIE);
			// 

			// 暂时使用阻塞发送模式，以后再修复DMA问题
			const UINT8* buf = static_cast<const UINT8*>(buffer);
			while(bufferSize--)
			{
				Send(*buf++);
			}

			return true;
		}
		



		/*
		 * @brief	UART发送机就绪判断
		 * @retval	发送机就绪则返回true，否则返回false
		 */
		virtual bool ReadyToSend() const
		{
			return PeripheralInfo::BaseAccess()->S1 & _BV(UART_BitBandType::S1_TDRE);
		}



		/*
		 * @brief	UART发送机发送完成判断
		 * @retval	发送机发送完成则返回true，否则返回false
		 */
		virtual bool SendComplete() const
		{
			return PeripheralInfo::BaseAccess()->S1 & _BV(UART_BitBandType::S1_TC);
		}



		/*
		 * @brief	UART发送机使能控制（开启/关闭接收机，检测使能状态）
		 * @retval	成功开启或关闭则返回1，否则返回0
		 * @notcie	非全双工模式下EnableTransmitter时请不要使用EnableReceive方法
		 *			DisableReceiver时请不要使用DisableTransmitter方法
		 */
		virtual int EnableTransmitter()
		{
			TxPin.MultiplexConfig(TxSignal::MUX_ALT);
			PeripheralInfo::BaseAccess()->C2 |= _BV(UART_BitBandType::C2_TE);
			return 1;
		}
		virtual int DisableTransmitter()
		{
			PeripheralInfo::BaseAccess()->C2 &= ~_BV(UART_BitBandType::C2_TE);
			TxPin.MultiplexConfig(PinMultiplexing::GPIO);
			return 1;
		}
		virtual bool IsTransmitterEnabled() const
		{
			return PeripheralInfo::BaseAccess()->C2 & _BV(UART_BitBandType::C2_TE);
		}





		/*
		 * @brief	UART波特率配置
		 * @param	baudrate 波特率值，根据实际情况设置波特率，单位bps
		 * @retval	成功配置则返回1，否则返回0
		 */
		virtual int SetBaudRate(const unsigned int baudrate)
		{
			UINT32 ModuleClockFreq = PeripheralInfo::ModuleClock::Frequency;
			
			// 这是一个神奇的算法，regval低5位就是BRFA的值，剩下的就是BD的值
			UINT32 regval = (ModuleClockFreq << 1) / baudrate;

			// regval 构成
			// SBR_High  SBR_Low  BRFA
			// [17:13]   [12:5]   [4:0]
			UINT8 BRFA = regval & _BF(4, 0);
			UINT8 BDH_SBR = (regval & _BF(17, 13)) >> 13;
			UINT8 BDL_SBR = (regval & _BF(12, 5)) >> 5;

			PeripheralInfo::BaseAccess()->BDH &= ~_BF(4, 0);	// clear SBR bits
			PeripheralInfo::BaseAccess()->BDH |= BDH_SBR;		// set SBR bits
			PeripheralInfo::BaseAccess()->BDL = BDL_SBR;		// write SBR low register
			PeripheralInfo::BaseAccess()->C4 &= ~_BF(4, 0);		// clear BRFA bits
			PeripheralInfo::BaseAccess()->C4 |= BRFA;			// set BRFA bits

			return 1;
		}
		


		/*
		 * @brief	UART获取波特率
		 * @retval	返回波特率值，单位bps
		 */
		virtual unsigned int GetBaudRate() const
		{
			UINT32 ModuleClockFreq = PeripheralInfo::ModuleClock::Frequency;
			UINT8 BRFA = PeripheralInfo::BaseAccess()->C4 & _BF(4, 0);
			UINT8 BDH_SBR = PeripheralInfo::BaseAccess()->BDH & _BF(4, 0);
			UINT8 BDL_SBR = PeripheralInfo::BaseAccess()->BDL;
			
			// 波特率寄存器合成值
			UINT32 regval = (BDH_SBR << 13) | (BDL_SBR << 5) | BRFA;

			return (ModuleClockFreq << 1) / regval;
		}

		

		/*
		 * @brief	UART数据位配置
		 * @param	datebits 数据位，单次传输中数据的有效位数
		 * @retval	成功配置则返回1，否则返回0
		 * @notice	UART只有8位或9位配置能力
		 */
		virtual int SetDataBits(const unsigned int databits)
		{
			if (databits == 8)
			{
				PeripheralInfo::BaseAccess()->C1 &= ~_BV(UART_BitBandType::C1_M);
				return 1;
			}
			else if (databits == 9)
			{
				PeripheralInfo::BaseAccess()->C1 |= _BV(UART_BitBandType::C1_M);
				return 1;
			}
			return 0;
		}



		/*
		 * @brief	UART获取数据位
		 * @retval	返回单次通讯中传输的有效数据位数
		 */
		virtual unsigned int GetDataBits() const
		{
			if (!(PeripheralInfo::BaseAccess()->C1 & _BV(UART_BitBandType::C1_M)))
			{
				return 9;
			}
			else
			{
				return 8;
			}
		}




		/*
		 * @brief	UART校验模式设置
		 * @param	parity 校验模式枚举，可以是PARITY枚举的任何一个
		 *				@arg PARITY_NONE
		 *				@arg PARITY_EVEN
		 *				@arg PARITY_ODD
		 * @retval	成功配置则返回1，否则返回0
		 */
		virtual int SetParityMode(const PARITY parity)
		{
			switch (parity)
			{
			case PARITY_EVEN :
				PeripheralInfo::BaseAccess()->C1 &= ~_BV(UART_BitBandType::C1_PT);
				PeripheralInfo::BaseAccess()->C1 |= _BV(UART_BitBandType::C1_PE);
				return 1;

			case PARITY_ODD :
				PeripheralInfo::BaseAccess()->C1 |= _BV(UART_BitBandType::C1_PT);
				PeripheralInfo::BaseAccess()->C1 |= _BV(UART_BitBandType::C1_PE);
				return 1;

			case PARITY_NONE :
				PeripheralInfo::BaseAccess()->C1 &= ~_BV(UART_BitBandType::C1_PE);
				return 1;

			default:
				return 0;
			}
		}



		/*
		 * @brief	UART获取校验模式
		 * @retval	当前的校验模式
		 */
		virtual PARITY GetParityMode() const
		{
			if (!(PeripheralInfo::BaseAccess()->C1 & _BV(UART_BitBandType::C1_PE)))
			{
				return PARITY_NONE;
			}
			else
			{
				if (!(PeripheralInfo::BaseAccess()->C1 & _BV(UART_BitBandType::C1_PT)))
				{
					return PARITY_EVEN;
				}
				else
				{
					return PARITY_ODD;
				}
			}
		}



		/*
		 * @brief	UART发送校验字（一般用于CRC校验模式最后发送一个校验字）
		 * @ovride	UART不支持该方法
		 */
		virtual int SendParityWord()
		{
			return 0;
		}




		/*
		 * @brief	UART获取错误状态
		 * @ovride	无错误时返回0，否则按照ERT中的错误按位或组合
		 * @notice	当前尚未实现此功能
		 */
		virtual int GetErrors() const
		{
			return 0;
		}





		/*
		 * @brief	UART设置首传输位
		 * @param	firstbit 首传输位枚举
		 *				@arg MSB 最高位先传
		 *				@arg LSB 最低位先传
		 * @retval	成功配置则返回1，否则返回0
		 */
		virtual int SetFirstBit(const FSB firstbit)
		{
			if (firstbit == MSB)
			{
				PeripheralInfo::BaseAccess()->S2 |= _BV(UART_BitBandType::S2_MSBF);
			}
			else
			{
				PeripheralInfo::BaseAccess()->S2 &= ~_BV(UART_BitBandType::S2_MSBF);
			}
			return 1;
		}



		/*
		 * @brief	UART获取首传输位模式
		 * @retval	当前首传输位模式
		 */
		virtual FSB GetFirstBit() const
		{
			if (PeripheralInfo::BaseAccess()->S2 & _BV(UART_BitBandType::S2_MSBF))
			{
				return MSB;
			}
			else
			{
				return LSB;
			}
		}





		/*
		 * @brief	UART主从模式设置
		 * @param	mode 模式枚举（只有主模式）
		 *				@arg MASTER 主从通讯器主机模式
		 * @retval	成功配置则返回1，否则返回0
		 */
		virtual int SetMode(const MSM mode)
		{
			switch (mode)
			{
			case MASTER :
				return 1;
				
			default:
				return 0;
			}
		}



		/*
		 * @brief	UART主从模式设置
		 * @retval	UART永远都是主模式
		 */
		virtual MSM GetMode() const
		{
			return MASTER;
		}





		/*
		 * @brief	通讯器设置传输工作模式
		 * @param	transmission_mode 传输工作模式枚举
		 *				@arg FullDuplex 全双工
		 *				@arg HalfDuplex_Tx/HalfDuplex_Rx 半双工发/半双工收
		 *				@arg Simplex_Tx/Simplex_Rx 单工发/单工收
		 * @retval	成功配置则返回1，否则返回0
		 * @ovride	若派生类通讯器不支持某些模式则返回0
		 */
		virtual int SetTransmissionMode(const TRM transmission_mode)
		{
			switch (transmission_mode)
			{
			case FullDuplex :
				EnableReceiver();
				EnableTransmitter();
				PeripheralInfo::BaseAccess()->C1 &= ~_BV(UART_BitBandType::C1_LOOPS);
				return 1;


				// 半双工说明
				// C1[LOOPS] 决定接收机是否与RXD信号引脚连接，SET时不连接，RESET时连接
				// C1[RSRC]  决定接收机输入信号来自TXD引脚信号还是发送机输出信号，
				//			 SET时连接到TXD引脚信号，RESET时连接到发送机输出信号
				// C3[TXDIR] 决定TXD信号引脚的方向，SET时为输出方向，RESET时为输入方向

			case HalfDuplex_Rx :
				// 半双工仍需打开发送机与接收机
				EnableReceiver();
				EnableTransmitter();

				// 配置TXD引脚为RXD所需状态

				PeripheralInfo::BaseAccess()->C1 |= _BV(UART_BitBandType::C1_LOOPS);
				PeripheralInfo::BaseAccess()->C1 |= _BV(UART_BitBandType::C1_RSRC);
				PeripheralInfo::BaseAccess()->C3 &= ~_BV(UART_BitBandType::C3_TXDIR);
				return 1;

			case HalfDuplex_Tx :
				// 半双工仍需打开发送机与接收机
				EnableReceiver();
				EnableTransmitter();

				// 配置TXD引脚为TXD所需状态

				PeripheralInfo::BaseAccess()->C1 |= _BV(UART_BitBandType::C1_LOOPS);
				PeripheralInfo::BaseAccess()->C1 |= _BV(UART_BitBandType::C1_RSRC);
				PeripheralInfo::BaseAccess()->C3 |= _BV(UART_BitBandType::C3_TXDIR);
				return 1;

			case Simplex_Rx :
				EnableReceiver();
				DisableTransmitter();
				PeripheralInfo::BaseAccess()->C1 &= ~_BV(UART_BitBandType::C1_LOOPS);
				return 1;

			case Simplex_Tx :
				EnableTransmitter();
				DisableReceiver();
				PeripheralInfo::BaseAccess()->C1 &= ~_BV(UART_BitBandType::C1_LOOPS);
				return 1;

			default:
				return 0;
			}
		}



		/*
		 * @brief	UART获取传输工作模式
		 * @retval	传输工作模式
		 */
		virtual TRM GetTransmissionMode() const
		{
			if (!(PeripheralInfo::BaseAccess()->C1 & _BV(UART_BitBandType::C1_LOOPS)))
			{
				if (!IsTransmitterEnabled())
				{
					return Simplex_Rx;
				}
				else if (!IsReceiverEnabled())
				{
					return Simplex_Tx;
				}
				else
				{
					return FullDuplex;
				}
			}
			else
			{
				// 半双工分支
				if (!(PeripheralInfo::BaseAccess()->C3 & _BV(UART_BitBandType::C3_TXDIR)))
				{
					return HalfDuplex_Rx;
				}
				else
				{
					return HalfDuplex_Tx;
				}
			}
		}





		/*
		 * @brief	事件通知开关控制
		 * @param	evt 需要开启或关闭通知的事件类型
		 * @retval	成功使能/禁止某个事件通知则返回1，否则返回0
		 */
		virtual int EnableEventNotify(EventType evt)
		{
			switch (evt)
			{
			case READY_TO_SEND :
				PeripheralInfo::BaseAccess()->C5 &= ~_BV(UART_BitBandType::C5_TDMAS);
				PeripheralInfo::BaseAccess()->C2 |= _BV(UART_BitBandType::C2_TIE);
				return 1;

			case DATE_AVAILABLE :
				PeripheralInfo::BaseAccess()->C5 &= ~_BV(UART_BitBandType::C5_RDMAS);
				PeripheralInfo::BaseAccess()->C2 |= _BV(UART_BitBandType::C2_RIE);
				return 1;

			case SEND_COMPLETED :
				PeripheralInfo::BaseAccess()->C2 |= _BV(UART_BitBandType::C2_TCIE);
				return 1;

			default:
				return 0;
			}
		}
		virtual int DisableEventNotify(EventType evt)
		{
			switch (evt)
			{
			case READY_TO_SEND :
				PeripheralInfo::BaseAccess()->C2 &= ~_BV(UART_BitBandType::C2_TIE);
				return 1;

			case DATE_AVAILABLE :
				PeripheralInfo::BaseAccess()->C2 &= ~_BV(UART_BitBandType::C2_RIE);
				return 1;

			case SEND_COMPLETED :
				PeripheralInfo::BaseAccess()->C2 &= ~_BV(UART_BitBandType::C2_TCIE);
				return 1;

			default:
				return 0;
			}
		}



		/*
		 * @brief	添加事件监听器，监听事件对象的某个事件
		 *			并开启该事件通知
		 * @param	evt 对象的事件
		 *			FnPtr 触发该事件时的处理函数，由派生类在实现时转换到所需的类型
		 * @retval	成功注册某个事件监听器则返回1，否则返回0
		 */
		virtual int AddEventListener(EventType evt, void* FnPtr)
		{
			switch (evt)
			{
			case DATE_AVAILABLE :
				PeripheralInfo::BaseAccess()->C5 &= ~_BV(UART_BitBandType::C5_RDMAS);
				PeripheralInfo::BaseAccess()->C2 |= _BV(UART_BitBandType::C2_RIE);
				return 1;

			case SEND_COMPLETED :
				sendCompletedISR = FnPtr;
				PeripheralInfo::BaseAccess()->C2 |= _BV(UART_BitBandType::C2_TCIE);
				return 1;

			case READY_TO_SEND :
				readyToSendISR = FnPtr;
				PeripheralInfo::BaseAccess()->C5 &= ~_BV(UART_BitBandType::C5_TDMAS);
				PeripheralInfo::BaseAccess()->C2 |= _BV(UART_BitBandType::C2_TIE);
				return 1;

			default:
				return 0;
			}
		}
		void AddEventListener(const ReceiveListener<UINT8>* rl)
		{
			dataAvailableListenerWrapper.SetEventListener(rl);
		}




		/*
		 * @brief	外设开启/关闭接口方法
		 * @retval	派生类无法实现与执行效果相符时请返回0，否则返回1
		 *			e.g. 当调用EnablePeripheral方法若某外设始终处于开启状态则返回1
		 *				 当调用DisablePeripheral方法若某外设始终处于开启状态无法关闭时请返回0
		 */
		virtual int EnablePeripheral()
		{
			// 设为当MCU处于STOP mode 下【不】停止模块时钟
			PeripheralInfo::BaseAccess()->C1 &= ~_BV(UART_BitBandType::C1_UARTSWAI);
			return 1;
		}
		virtual int DisablePeripheral()
		{
			// 设为当MCU处于STOP mode 下停止模块时钟
			PeripheralInfo::BaseAccess()->C1 |= _BV(UART_BitBandType::C1_UARTSWAI);
			return 1;
		}






		Peripheral(	const TRM iom = FullDuplex, 
					const UINT32 baudRate = 115200, 
					const PARITY parity = PARITY_NONE, 
					const UINT8 dataBits = 8, 
					const STB stopBits = STOPBITS_10)
					: LastSubmittedDMAChannel(-1)
		{
			// 注册全局实例指针
			PeripheralInfo::InstancePtr = this;
			
			// 开启模块时钟
			ClockSource::ClockOn();


			// 停止位
			SetStopBits(stopBits);

			// 数据位
			SetDataBits(dataBits);

			// 校验位
			SetParityMode(parity);

			// 波特率配置（参考固件库提供的算法）
			SetBaudRate(baudRate);


			// 传输工作配置
			SetTransmissionMode(iom);


			// NVIC
			CM4::$NVIC::SetPriority(
				PeripheralInfo::TranceiveIQR, 
				PeripheralInfo::TranceiveInteruptPriority);
			CM4::$NVIC::EnableIRQ(PeripheralInfo::TranceiveIQR);
			

			// 最后使能外设
			EnablePeripheral();

		}



		virtual ~Peripheral()
		{
			ClockSource::ClockOff();
		}



		/*
		 * @brief	中断回调函数槽
		 */
		adxM3::Function< void(void*) > sendCompletedISR;
		adxM3::Function< void(void*) > readyToSendISR;
		
		// 用户监听器
		EventListenerWrapper< ReceiveListener<UINT8> > dataAvailableListenerWrapper;

	protected:
	

		typedef typename PeripheralInfo::TxSignal TxSignal;
		typedef typename PeripheralInfo::RxSignal RxSignal;
		
		// 必须生成信号引脚实例，必须有实例才能自动开启Port模块时钟
		// 构造时并不立刻切换MUX，等到收发通信时根据需要自动切换
		SignalPin<TxSignal, RetainCurrentMultiplexChannel> TxPin;
		SignalPin<RxSignal, RetainCurrentMultiplexChannel> RxPin;


	private:

		INT8 LastSubmittedDMAChannel;

	};

}


extern "C"
{
	void UART0_RX_TX_IRQHandler(void);
	void UART1_RX_TX_IRQHandler(void);
	void UART2_RX_TX_IRQHandler(void);
	void UART3_RX_TX_IRQHandler(void);
	void UART4_RX_TX_IRQHandler(void);
	void UART5_RX_TX_IRQHandler(void);
};

#endif  /*_ADX_K60_UART_H_*/
