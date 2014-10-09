  
#ifndef _ADX_CM_NRF24L01_H_
#define _ADX_CM_NRF24L01_H_

#include "../HadwareAbstraction/Communicator/SynchronousCommunicator.h"
#include "../HadwareAbstraction/GPIO/GeneralPurposeInputOutputPin.h"
#include "../HadwareAbstraction/Timer/BlockingDelayController.h"


namespace adxM3
{


	
	enum 
	{
		nRF_ADDR_WIDTH = 5  // 5 BYTES TX address width
	};



	class nRF24L01
	{
	public:
		typedef UINT8 STAWORD;										// nRF状态字

	private:



		/*
		 * nRF24L01命令枚举
		 */
		enum CMD
		{
			READ_REG				= 0x00,							// 读寄存器
			WRITE_REG				= 0x20,							// 写寄存器
			RD_RX_PLOAD				= 0x61,							// 读取接收数据指令
			WR_TX_PLOAD				= 0xA0,							// 写待发数据指令
			FLUSH_TX				= 0xE1,							// 冲洗发送 FIFO指令
			FLUSH_RX				= 0xE2,							// 冲洗接收 FIFO指令
			REUSE_TX_PL				= 0xE3,							// 定义重复装载数据指令
			NOP						= 0xFF							// 保留
		};




		/*
		 * nRF24L01寄存器地址枚举
		 */
		enum REG 
		{
			CONFIG					= 0x00,							// 配置收发状态，CRC校验模式以及收发状态响应方式
			EN_AA					= 0x01,							// 自动应答功能设置
			EN_RXADDR				= 0x02,							// 可用信道设置
			SETUP_AW				= 0x03,							// 收发地址宽度设置
			SETUP_RETR				= 0x04,							// 自动重发功能设置
			RF_CH					= 0x05,							// 工作频率(通道)设置
			RF_SETUP				= 0x06,							// 发射速率、功耗功能设置
			STATUS					= 0x07,							// 状态寄存器
			OBSERVE_TX				= 0x08,							// 发送监测功能
			CD						= 0x09,							// 地址检测           
			RX_ADDR_P0				= 0x0A,							// 频道0接收数据地址
			RX_ADDR_P1				= 0x0B,							// 频道1接收数据地址
			RX_ADDR_P2				= 0x0C,							// 频道2接收数据地址
			RX_ADDR_P3				= 0x0D,							// 频道3接收数据地址
			RX_ADDR_P4				= 0x0E,							// 频道4接收数据地址
			RX_ADDR_P5				= 0x0F,							// 频道5接收数据地址
			TX_ADDR					= 0x10,							// 发送地址寄存器
			RX_PW_P0				= 0x11,							// 接收频道0接收数据长度
			RX_PW_P1				= 0x12,							// 接收频道0接收数据长度
			RX_PW_P2				= 0x13,							// 接收频道0接收数据长度
			RX_PW_P3				= 0x14,							// 接收频道0接收数据长度
			RX_PW_P4				= 0x15,							// 接收频道0接收数据长度
			RX_PW_P5				= 0x16,							// 接收频道0接收数据长度
			FIFO_STATUS				= 0x17							// FIFO状态寄存器设置
		};

	
	public:

		/*
		 * CONFIG 寄存器位定义
		 */
		enum CFG
		{
			Tx						= 0x00,
			Rx						= 0x01,
			PWR_UP					= 0x02,
			CRCOCT					= 0x04,
			EN_CRC					= 0x08,
			IT_MaxRetry				= 0x10,
			IT_DataSent				= 0x20,
			IT_DataReceived			= 0x40
		};




		/*
		 * STATUS 寄存器位定义
		 */
		enum STA
		{
			FLAG_DataReceived		= 0x40,
			FLAG_DataSent			= 0x20,
			FLAG_MaxRetry			= 0x10,
			FLAG_TxFull			= 0x01
		};



		/*
		 * FIFO_STATUS 寄存器位定义
		 */
		enum FIFOSTA
		{
			FLAG_RxEmpty			= 0x01,
			FLAG_RxFull				= 0x02,
			FLAG_TxEmpty			= 0x10,
			FLAG_TxFIFOFull			= 0x20,
			FLAG_TxReuse			= 0x40
		};




		/*
		 * RF_SETUP 寄存器位定义
		 */
		enum RF
		{
			LNA_HCURR				= 0x01,							// 低噪声增益使能位
			PWR_0dBm				= 0x06,							// 0dBm增益
			PWR_n6dBm				= 0x04,							// -6dBm增益
			PWR_n12dBm				= 0x02,							// -12dBm增益
			PWR_n18dBm				= 0x00,							// -18dBm增益
			DataRate_2Mbps			= 0x08,							// 2Mbps传输速率
			DataRate_1Mbps			= 0x00							// 1Mbps传输速率
		};


		

		/*
		 * nRF24L01接收通道定义
		 */
		enum PIPE
		{
			PIPE0					= 0x00,
			PIPE1					= 0x01,
			PIPE2					= 0x02,
			PIPE3					= 0x03,
			PIPE4					= 0x04,
			PIPE5					= 0x05
		};


	// 转换到 RxPipe 接收通道接收长度寄存器地址
	#define REG_PIPE_PayLoadWidth(P)	((P)+0x11)

	// 转换到 RxPipe 接收通道接收地址寄存器地址
	#define REG_PIPE_RxAddress(P)		((P)+0x0A)

	// 转换到 RxPipe 接收通道自动应答使能寄存器位定义
	#define AutoAck_PIPE(P)				(_BV(P))

	// 转换到 RxPipe 接收通道接收使能寄存器位定义
	#define EN_RxPIPE(P)				(_BV(P))


		enum EVT
		{
			TRANSMISSION_FAIL			= 0x10,
			TRANSMISSION_COMPLETE		= 0x20,
			RECEIVED					= 0x40
		};



// 关闭在匿名union中定义变量的警告
#pragma diag_suppress 1165


		/*
		 * @brief	接收者信息类
		 *			绑定接收者后可直接向其发送数据包
		 */
		struct ReceiverInfo
		{
			union 
			{
				BYTE AddressBytes[nRF_ADDR_WIDTH];
				struct 
				{
					BYTE Subnet;
					UINT Gateway;
				} Address;
			};
			BYTE PayLoadWidth;
		};



		/*
		 * @brief	发送者信息类
		 *			绑定发送者后可接收到其发来的数据包
		 */
		struct TransmitterInfo
		{
			union 
			{
				BYTE AddressBytes[nRF_ADDR_WIDTH];
				struct 
				{
					BYTE Subnet;
					UINT Gateway;
				} Address;
			};
			PIPE TargetPipe;
			BYTE PayLoadWidth;
		};



		/*
		 * @brief	无线数据包结构
		 * @tparam	T 用户数据结构
		 *			PacketSize 用户数据包大小（字节数），请不要超过32
		 */
		template< class T, BYTE PacketSize = sizeof(T) >
		union Packet
		{
			BYTE Pack[PacketSize];
			T Unpack;

			Packet()
			{
				STATIC_ASSERT(PacketSize <= 32,
					nRF24L01_packet_size_should_be_less_than_32_bytes);
				
				for (int i = 0; i < PacketSize; ++i)
				{
					Pack[i] = 0;
				}
			}
		};




		nRF24L01(SynchronousCommunicator& spi,
				 GeneralPurposeInputOutputPin& _CE,
				 GeneralPurposeInputOutputPin& _IRQ,
				 BlockingDelayController& delay)
				 : SPI(spi), CE(_CE), IRQ(_IRQ), MCU(delay), PayLoadWidth(1)
		{

			// SPI interface config
			SPI.SetClockPolarity(SynchronousCommunicator::CPOL_LOW);
			SPI.SetClockPhase(SynchronousCommunicator::CPHA_1st_EDGE);
			SPI.SetMode(SynchronousCommunicator::MASTER);
			SPI.SetTransmissionMode(SynchronousCommunicator::FullDuplex);
			SPI.SetBaudRate(9000000);								// MAX 9MHz
			SPI.SetFirstBit(SynchronousCommunicator::MSB);
			SPI.SetDataBits(8);

			// Pin config
			CE.SetDirection(GeneralPurposeInputOutput::OUT);
			CE.OutputConfig(GeneralPurposeInputOutput::PushPull, GeneralPurposeInputOutput::HighSpeed);


			// 无线电参数设定
			RFConfig(40, LNA_HCURR|PWR_0dBm|DataRate_2Mbps);

			CE = 0;
			FlushTx();
			FlushRx();
		}


		~nRF24L01()
		{
			FlushTx();
			FlushRx();

			// 模块停机省电
			WriteReg(WRITE_REG|CONFIG, 0);
		}




		/*
		 * @brief	向缓冲区写入或者读出一个数据包
		 * @param	pBuf 待写入或待读出数据缓冲区指针
		 * @retval	返回状态寄存器的值
		 */
		STAWORD	TransmitPacket(const BYTE* pBuf)
		{
			CE = 0;
			while (!ReadyToSend());
			STAWORD status = WriteBuf(WR_TX_PLOAD, pBuf, PayLoadWidth);	// 装载数据 
			CE = 1;																// 启动发送		
			MCU.Delay_us(10);													// 10us后开始发送
			return status;
		}
		STAWORD ReceivePacket(BYTE* pBuf)
		{
			CE = 0;
			while (!DataAvailable());
			STAWORD status = ReadBuf(RD_RX_PLOAD, pBuf, PayLoadWidth);		// 从接收缓冲区读出数据
			return status;
		}




		/*
		 * @brief	配置无线电参数
		 * @param	channel RF频道，范围：0~125
					setup RF工作参数（写入RF_SETUP寄存器的值）
		 */
		void RFConfig(BYTE channel, BYTE setup)
		{
			WriteReg(WRITE_REG|RF_CH, channel);								// 设置 RF 频道
			WriteReg(WRITE_REG|RF_SETUP, setup);							// TX_PWR,Datarate,LNA
		}



		/*
		 * @brief	设置接收通道数据包长度
		 * @param	pipex 通道x，取值RxPipe，x取0~5
					pl_width 接收长度，范围1~32
		 */
		void SetPayLoadWidth(PIPE pipex, BYTE pl_width)
		{
			WriteReg(WRITE_REG|REG_PIPE_PayLoadWidth(pipex), pl_width);			// 设置接收数据长度
			PayLoadWidth = pl_width;
		}


		
		/*
		 * @brief	读取接收通道数据包接收长度
		 * @param	pipex 通道x，取值RxPipe，x取0~5
		 * @retval 	接收长度，范围1~32
		 */
		BYTE GetPayLoadWidth(PIPE pipex)
		{
			PayLoadWidth = ReadReg(READ_REG|pipex);
			return PayLoadWidth;
		}




		/*
		 * @brief	读取当前nRF模块状态
		 * @retval 	状态寄存器值
		 */
		STAWORD GetStatus()
		{
			return WriteReg(NOP, NOP);
		}
		UINT8 GetFIFOStatus()
		{
			return ReadReg(FIFO_STATUS);
		}




		/*
		 * @brief	冲洗收发缓冲区
		 * @retval 	状态寄存器值
		 */
		STAWORD FlushTx()
		{
			CE = 0;
			return WriteReg(FLUSH_TX, 0xFF);
		}
		STAWORD FlushRx()
		{
			CE = 0;
			return WriteReg(FLUSH_RX, 0xFF);
		}
		
		
		
		/*
		 * @brief	开启和关闭nRF事件通知
		 * @param 	evt nRF事件名称，可以是任意事件的按位或组合
		 */
		STAWORD EnableEventNotify(UINT8 evt)
		{
			IRQ.SetDirection(GeneralPurposeInputOutput::IN);
			IRQ.InputConfig(GeneralPurposeInputOutput::Floating);
			BYTE regval = ReadReg(READ_REG|CONFIG);
			regval &= (UINT8)~evt;
			return WriteReg(WRITE_REG|CONFIG, regval);
		}
		STAWORD DisableEventNotify(UINT8 evt)
		{
			BYTE regval = ReadReg(READ_REG|CONFIG);
			regval |= evt;	
			return WriteReg(WRITE_REG|CONFIG, regval);
		}
		
		


		/*
		 * @brief	清除中断标记
		 * @param	flag STA枚举中的值的任意按位或组合
		 */
		void ClearFlag(UINT8 flag)
		{
			WriteReg(WRITE_REG|STATUS, flag);
		}




		/*
		 * @brief	设定发送数据去往的目标
		 * @param	receiver 接收机信息
		 */
		nRF24L01& operator[](ReceiverInfo& receiver)
		{
			CE = 0;
			WriteBuf(WRITE_REG|TX_ADDR, receiver.AddressBytes, nRF_ADDR_WIDTH);	// 写接收机地址
			WriteBuf(WRITE_REG|RX_ADDR_P0, receiver.AddressBytes, nRF_ADDR_WIDTH);	// 本机接收地址Pipe0为接收机地址，以自动应答接收机接收信号
			//WriteReg(WRITE_REG|EN_RXADDR, EN_RxPIPE(PIPE0));					// 接收缓冲区 Pipe0 使能

			// 发送机只需要设置好发送的长度，接收机才需要设置接收长度寄存器
			PayLoadWidth = receiver.PayLoadWidth;
	
			//WriteReg(WRITE_REG|EN_AA, AutoAck_PIPE(PIPE0));					// Pipe0 启用自动应答
	
			//WriteReg(WRITE_REG|SETUP_RETR, 0x1A);								// 设置自动重发间隔时间:500us + 86us;最大自动重发次数:10次
			BYTE regval = ReadReg(READ_REG|CONFIG);
			regval &= (UINT8)~0x0F;
			regval |= Tx|PWR_UP|CRCOCT|EN_CRC;
			WriteReg(WRITE_REG|CONFIG, regval);
			return *this;
		}



		/*
		 * @brief	设定接收数据来自的目标
		 * @param	transmitter 发送机信息
		 */
		nRF24L01& operator[](TransmitterInfo& transmitter)
		{
			CE = 0;
			WriteBuf(WRITE_REG|REG_PIPE_RxAddress(transmitter.TargetPipe), transmitter.AddressBytes, nRF_ADDR_WIDTH);
			SetPayLoadWidth(transmitter.TargetPipe, transmitter.PayLoadWidth);
			//WriteReg(WRITE_REG|EN_AA, AutoAck_PIPE(PIPE0));				// Enable Auto.Ack:Pipe0
			WriteReg(WRITE_REG|EN_RXADDR, EN_RxPIPE(PIPE0));				// Enable Pipe0
			
			BYTE regval = ReadReg(READ_REG|CONFIG);
			regval &= (UINT8)~0x0F;
			regval |= Rx|PWR_UP|CRCOCT|EN_CRC;
			WriteReg(WRITE_REG|CONFIG, regval);
			
			CE = 1;
			MCU.Delay_us(130);	// 130us后进入接收模式
			return *this;
		}
		


			
		/*
		 * @brief	阻塞等待发送或者接收完成
		 * @param	pBuf 待写入或待读出数据缓冲区指针
		 * @retval	自身，用以级联
		 */
		nRF24L01& operator<<(const BYTE* pBuf)
		{
			int timeout = 0;
			STAWORD status = TransmitPacket(pBuf);
			while (!(status & (FLAG_DataSent|FLAG_MaxRetry)))
			{
				++timeout;
				status = GetStatus();
				if (timeout > 20000)
				{
					break;
				}
			}
			WriteReg(WRITE_REG|STATUS, status);						// 清除标记
			return *this;
		}
		nRF24L01& operator>>(BYTE* pBuf)
		{
			STAWORD status = ReceivePacket(pBuf);
			WriteReg(WRITE_REG|STATUS, status);						// 清除标记
			return *this;
		}




		//CALLBACK		EXTIISR	 &	OnEvent(void* capture = NULL);




		/*
		 * @brief	状态检测函数，通过读取STATUS寄存器或者FIFO_STATUS寄存器判断是否满足条件
		 * @retval	true如果满足，false如果不满足
		 */
		bool DataAvailable()
		{
			STAWORD status = GetStatus();
			return status & FLAG_DataReceived ? true : (status & 0x0e) == 0x0e ? false : true;
		}
		bool SendComplete()
		{
			STAWORD status = GetStatus();
			return status & FLAG_DataSent ? true : false;
		}
		bool SendFail()
		{
			STAWORD status = GetStatus();
			return status & FLAG_MaxRetry ? true : false;
		}
		bool ReadyToSend()
		{
			STAWORD status = GetStatus();
			return status & FLAG_TxFull ? false : true;
		}
		bool ReadyToReceive()
		{
			BYTE fifo_status = GetFIFOStatus();
			return fifo_status & FLAG_RxFull ? false : true;
		}


	private:


		/*
		 * @brief	写寄存器值
		 * @param	reg 寄存器地址
					val	写入的值
		 * @retval	返回nRF24L01状态寄存器的值
		 */
		STAWORD	WriteReg(const BYTE reg, const BYTE val)
		{
			// 先传出reg，再传出val，返回的高字节为status word，所以向右移动8位
			BYTE tx[2] = { reg, val };
			BYTE rx[2] = { 0 };
			
			SPI.SynchronouslyTranceive(tx, rx, 2);
			return rx[0];
		}



		/*
		 * @brief	读寄存器值
		 * @param	reg 寄存器地址
		 * @retval	寄存器的值
		 */
		BYTE ReadReg(const BYTE reg)
		{
			// 先传出reg，再传出空白的字节，返回的高字节为status word，正好低字节是regval
			BYTE tx[2] = { reg, 0xFF };
			BYTE rx[2] = { 0 };
			
			SPI.SynchronouslyTranceive(tx, rx, 2);
			return rx[1];
		}



		/*
		 * @brief	读写nRF24L01缓冲区
		 * @param	reg 寄存器地址
					pBuf 待写入或待读出数据缓冲区指针
					len 读写长度
		 * @retval	状态寄存器的值
		 */
		STAWORD	WriteBuf(const BYTE reg, const BYTE* pBuf, const BYTE len)
		{
			// 申请一个缓冲区，由于nrf单次发送不超过32字节，
			// 所以在堆栈放一个33字节的接收缓冲区和发送缓冲区
			// 以上目的为了适应SynchronousCommunicator的接口方法
			// 发送缓冲区第一字节为reg地址，接下来len个字节为待发送的内容
			// 会将pBuf中的内容全拷贝进去
			BYTE tx_buf[33] = { 0 };
			BYTE rx_buf[33] = { 0 };
			tx_buf[0] = reg;
			for (int i = 1; i <= len; ++i)
			{
				tx_buf[i] = pBuf[i - 1];
			}
			
			SPI.SynchronouslyTranceive(tx_buf, rx_buf, len + 1);
			return rx_buf[0];    
		}
		STAWORD	ReadBuf(const BYTE reg, BYTE* pBuf, const BYTE len)
		{
			BYTE rx_buf[33] = { 0 };
			rx_buf[0] = reg;
			
			// 以下相当于使用rx_buf交换信息
			SPI.SynchronouslyTranceive(rx_buf, rx_buf, len + 1);
			for (int i = 1; i <= len; ++i)
			{
				// 把接收到的数据拷贝回pBuf
				pBuf[i - 1] = rx_buf[i];
			}
			return rx_buf[0];
		}
	




	protected:

		SynchronousCommunicator& SPI;						// SPI通讯接口
		GeneralPurposeInputOutputPin& CE;					// 模块选择active low
		GeneralPurposeInputOutputPin& IRQ;					// 中断信号线at rising edge
		BlockingDelayController& MCU;						// 核心控制器提供延时功能
		
		// 接收通道接收数据长度
		BYTE PayLoadWidth;											
	};

}

#endif  /*_ADX_CM_NRF24L01_H_*/
