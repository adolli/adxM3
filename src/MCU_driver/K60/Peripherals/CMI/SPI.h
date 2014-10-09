
#ifndef _ADX_K60_SPI_H_
#define _ADX_K60_SPI_H_

#include "adxM3.h"
#include "../../System/System.h"
#include "../PeripheralBase.h"
#include "../PeripheralInfo.h"
#include "../Port/GlobalPort.h"
#include "../Port/SignalPin.h"

namespace K60
{

	/*
	 * @breif	串行外设通讯接口外设
	 */
	template< UINT32 BASE >
	class Peripheral< SPI_INFO<BASE> >
		: public adxM3::SynchronousCommunicator
	{
	public:


		typedef SPI_INFO<BASE> PeripheralInfo;
		typedef ClockGateController<typename PeripheralInfo::ClockSourceGate> ClockSource;


		Peripheral(	const MSM mode = MASTER, 
					const unsigned int databits = 16,
					const UINT32 baudrate = 11250000,
					const FSB firstbit = MSB,
					const CPOL polarity = CPOL_LOW,
					const CPHA phase = CPHA_1st_EDGE)
		{
			// 注册全局实例指针
			PeripheralInfo::InstancePtr = this;
			
			// 开启模块时钟
			ClockSource::ClockOn();

			EnablePeripheral();

			// 停机进行配置
			PeripheralInfo::BaseAccess()->MCR.HALT = 1;

			SetMode(mode);
			SetBaudRate(baudrate);
			SetDataBits(databits);
			SetFirstBit(firstbit);
			SetClockPolarity(polarity);
			SetClockPhase(phase);

			// 多个连续传输之间的PCS inactive电平最低持续时间暂时设为177ns
			// DT value == 8
			// tDT == 177ns
			//PeripheralInfo::BaseAccess()->CTAR[0].DT = 2;
			
			// 连续传输之间的每个字之间的间隔设为177ns
			// ASC value == 8
			// tASC == 177ns
			//PeripheralInfo::BaseAccess()->CTAR[0].ASC = 2;

			//PeripheralInfo::BaseAccess()->CTAR[0].CSSCK = 2;


			// 所有PCS信号非活动时为高
			PeripheralInfo::BaseAccess()->MCR.PCSIS = 0x3F;

			// 使能硬件缓冲模式
			PeripheralInfo::BaseAccess()->MCR.DIS_TXF = 0;
			PeripheralInfo::BaseAccess()->MCR.DIS_RXF = 0;

			// 清除FIFO
			PeripheralInfo::BaseAccess()->MCR.CLR_TXF = 1;
			PeripheralInfo::BaseAccess()->MCR.CLR_RXF = 1;
			

			// 清除SR寄存器标志位
			PeripheralInfo::BaseAccess()->SR.EOQF = 1;
			PeripheralInfo::BaseAccess()->SR.TFUF = 1;
			PeripheralInfo::BaseAccess()->SR.TFFF = 1;
			PeripheralInfo::BaseAccess()->SR.RFOF = 1;
			PeripheralInfo::BaseAccess()->SR.RFDF = 1;


			// 配置NVIC
			CM4::$NVIC::SetPriority(PeripheralInfo::PeripheralIRQ,
				PeripheralInfo::InteruptPriority);
			CM4::$NVIC::EnableIRQ(PeripheralInfo::PeripheralIRQ);

			// 取消停机模式
			PeripheralInfo::BaseAccess()->MCR.HALT = 0;
		}

		virtual ~Peripheral()
		{
			ClockSource::ClockOff();
		}




		/*
		 * @brief	SPI配置时钟极性
		 * @param	polarity 时钟极性枚举
		 *				@arg CPOL_LOW
		 *				@arg CPOL_HIGH
		 * @retval	设置成功则返回1，否则返回0
		 */
		virtual int SetClockPolarity(const CPOL polarity)
		{
			STATIC_ASSERT(CPOL_HIGH == 1 && CPOL_LOW == 0,
				enumerate_value_is_not_campacted_with_this_configuration);
			PeripheralInfo::BaseAccess()->CTAR[0].CPOL = polarity;
			return 1;
		}



		/*
		 * @brief	SPI获取时钟极性
		 * @retval	polarity 时钟极性枚举
		 */
		virtual CPOL GetClockPolarity() const
		{
			STATIC_ASSERT(CPOL_HIGH == 1 && CPOL_LOW == 0,
				enumerate_value_is_not_campacted_with_this_configuration);
			return CPOL(PeripheralInfo::BaseAccess()->CTAR[0].CPOL);
		}




		/*
		 * @brief	SPI配置时钟相位
		 * @param	phase 时钟相位枚举
		 *				@arg CPHA_1st_EDGE
		 *				@arg CPHA_2nd_EDGE
		 * @retval	设置成功则返回1，否则返回0
		 */
		virtual int SetClockPhase(const CPHA phase)
		{
			STATIC_ASSERT(CPHA_1st_EDGE == 0 && CPHA_2nd_EDGE == 1,
				enumerate_value_is_not_campacted_with_this_configuration);
			PeripheralInfo::BaseAccess()->CTAR[0].CPHA = phase;
			return 1;
		}



		/*
		 * @brief	SPI获取时钟相位
		 * @retval	phase 时钟相位枚举
		 */
		virtual CPHA GetClockPhase() const
		{
			return CPHA(PeripheralInfo::BaseAccess()->CTAR[0].CPHA);
		}




		enum { FSYS = PeripheralInfo::ModuleClock::Frequency };



		/*
		 * @brief	SPI波特率配置
		 * @param	baudrate 波特率值，K60 SPI外设可以设置以下波特率值 @BusCloclk == 90MHz
		 *				@arg 45,000,000		// bus div 2
		 *				@arg 22,500,000		// bus div 4
		 *				@arg 15,000,000		// bus div 6
		 *				@arg 11,250,000		// bus div 8
		 *				@arg 5,625,000		// bus div 16
		 *				@arg 2,812,500
		 *				@arg 1,406,250
		 *				@arg 703,125
		 *				@arg 351,563
		 *				@arg 175,781
		 *				...
		 *				@arg 2747
		 *				@arg 5,625,000 ~ 2747 其中可选任意值但不是所有都能配置到，会自动配置到近似的
		 * @retval	成功配置则返回1，否则返回0
		 */
		virtual int SetBaudRate(const unsigned int baudrate)
		{
			PeripheralInfo::BaseAccess()->CTAR[0].DBR = 1;		// double baudrate enable
			
			const UINT16 ScalerTbl[] = 
				{ 2, 4, 6, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768 };
			const UINT8 PrescalerTbl[] = 
				{ 2, 3, 5, 7 };

			UINT32 totalScaler = FSYS * 2 / baudrate;
			UINT32 br = 0;
			UINT32 pbr = 0;
			UINT32 min_diff = 0xFFFFFFFF;
			for(int scalerIdx = 0; scalerIdx <= 0x0F; ++scalerIdx)
			{
				for(int prescalerIdx = 0; prescalerIdx <= 0x03; ++prescalerIdx)
				{
					UINT32 difference = abs(int(ScalerTbl[scalerIdx] * PrescalerTbl[prescalerIdx] - totalScaler));
					if(difference < min_diff)
					{
						// 记录更接近波特率的配置
						min_diff = difference;
						br = scalerIdx;
						pbr = prescalerIdx;

						// 正好匹配标记
						if(min_diff == 0)
						{
							break;
						}
					}
				}
				if (min_diff == 0)
				{
					break;
				}
			}

			PeripheralInfo::BaseAccess()->CTAR[0].PBR = pbr;
			PeripheralInfo::BaseAccess()->CTAR[0].BR = br;
			return 1;
		}



		/*
		 * @brief	SPI获取波特率
		 * @retval	返回波特率值，单位bps
		 */
		virtual unsigned int GetBaudRate() const
		{
			const UINT16 ScalerTbl[] = 
				{ 2, 4, 6, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768 };
			const UINT8 PrescalerTbl[] = 
				{ 2, 3, 5, 7 };

			UINT32 pbr = PeripheralInfo::BaseAccess()->CTAR[0].PBR;
			UINT32 dbr = PeripheralInfo::BaseAccess()->CTAR[0].DBR;
			UINT32 br = PeripheralInfo::BaseAccess()->CTAR[0].BR;
			return FSYS * (1 + dbr) / PrescalerTbl[pbr] / ScalerTbl[br];
		}

		

		/*
		 * @brief	SPI数据位配置
		 * @param	datebits 数据位，单次传输中数据的有效位数
		 * @retval	成功配置则返回1，否则返回0
		 * @notice	数据位通常为7,8,16,32,64等
		 */
		virtual int SetDataBits(const unsigned int databits)
		{
			if (databits < 4)
			{
				return 0;
			}
			PeripheralInfo::BaseAccess()->CTAR[0].FMSZ = databits - 1;
			return 1;
		}



		/*
		 * @brief	K60 SPI外设可以设置以下波特率值获取数据位
		 * @retval	返回单次通讯中传输的有效数据位数
		 */
		virtual unsigned int GetDataBits() const
		{
			return PeripheralInfo::BaseAccess()->CTAR[0].FMSZ + 1;
		}




		/*
		 * @brief	SPI校验模式设置
		 * @param	parity 校验模式枚举，可以是以下值之一
		 *				@arg PARITY_NONE
		 *				@arg PARITY_CRC
		 * @retval	成功配置则返回1，否则返回0
		 */
		virtual int SetParityMode(const PARITY parity)
		{
			// 当前尚未支持
			return 0;
		}



		/*
		 * @brief	SPI获取校验模式
		 * @retval	当前的校验模式
		 */
		virtual PARITY GetParityMode() const
		{
			return PARITY_NONE;
		}



		/*
		 * @brief	SPI发送校验字（一般用于CRC校验模式最后发送一个校验字）
		 */
		virtual int SendParityWord()
		{
			return 0;
		}



		/*
		 * @brief	SPI获取错误状态
		 * @ovride	无错误时返回0，否则按照ERT中的错误按位或组合
		 */
		virtual int GetErrors() const
		{
			return 0;
		}




		/*
		 * @brief	SPI设置首传输位
		 * @param	firstbit 首传输位枚举
		 *				@arg MSB 最高位先传
		 *				@arg LSB 最低位先传
		 * @retval	成功配置则返回1，否则返回0
		 * @ovride	若派生类通讯器不支持某些模式则返回0
		 */
		virtual int SetFirstBit(const FSB firstbit)
		{
			STATIC_ASSERT(MSB == 0 && LSB == 1,
				enumerate_value_is_not_campacted_with_this_configuration);
			PeripheralInfo::BaseAccess()->CTAR[0].LSBFE = firstbit;
			return 1;
		}



		/*
		 * @brief	SPI获取首传输位模式
		 * @retval	当前首传输位模式
		 * @ovride	传输总有一个首传输位模式，派生类通讯器总可以实现此方法
		 */
		virtual FSB GetFirstBit() const
		{
			STATIC_ASSERT(MSB == 0 && LSB == 1,
				enumerate_value_is_not_campacted_with_this_configuration);
			return FSB(PeripheralInfo::BaseAccess()->CTAR[0].LSBFE);
		}



		/*
		 * @brief	SPI主从模式设置
		 * @param	mode 模式枚举
		 *				@arg MASTER 主从通讯器主机模式
		 *				@arg SLAVE 主从通讯器从机模式
		 * @retval	成功配置则返回1，否则返回0
		 */
		virtual int SetMode(const MSM mode)
		{
			STATIC_ASSERT(MASTER == 1 && SLAVE == 0,
				enumerate_value_is_not_campacted_with_this_configuration);
			PeripheralInfo::BaseAccess()->MCR.MSTR = mode;
			return 1;
		}



		/*
		 * @brief	SPI主从模式设置
		 * @retval	主模式或从模式枚举值
		 */
		virtual MSM GetMode() const
		{
			STATIC_ASSERT(MASTER == 1 && SLAVE == 0,
				enumerate_value_is_not_campacted_with_this_configuration);
			return MSM(PeripheralInfo::BaseAccess()->MCR.MSTR);
		}





		/*
		 * @brief	SPI设置传输工作模式
		 * @param	transmission_mode 传输工作模式枚举
		 *				@arg FullDuplex 全双工
		 *				@arg HalfDuplex_Tx/HalfDuplex_Rx 半双工发/半双工收
		 *				@arg Simplex_Tx/Simplex_Rx 单工发/单工收
		 * @retval	成功配置则返回1，否则返回0
		 */
		virtual int SetTransmissionMode(const TRM transmission_mode)
		{
			return 0;
		}



		/*
		 * @brief	SPI获取传输工作模式
		 * @retval	全双工
		 */
		virtual TRM GetTransmissionMode() const
		{
			return FullDuplex;
		}


	

	private:

		union PUSHR_Type
		{
			struct
			{
				__IO uint16_t TXDATA;				// TXDATA register
				unsigned PCS		: 6;
				unsigned			: 4;
				unsigned CTCNT		: 1;
				unsigned EOQ		: 1;
				unsigned CTAS		: 3;
				unsigned CONT		: 1;
			} Cfg;
			UINT32 PUSH_Value;
		};
		

	public:



		/*
		 * @brief	SPI收发接口方法
		 * @param	out 待发送数据的地址
		 *			in 接收的数据存放区的地址
		 *			_TxBuf 待发送数据缓冲区
		 *			_RxBuf 接收缓冲区（由使用者自行分配）
		 *			bufferSize 多字节收发的发送缓冲区大小
		 *			发送帧支持字节、半字、字 类型数据，由通讯外设根据实际情况实现
		 *			收发前请务必配置好数据位
		 * @notice	在派生类同步通讯器中如果没收到数据可返回0
		 *			多字节收发方法中单次发送一个字节还是一个半字由派生类外设根据当前配置决定
		 * @retval	返回接收到的数据/缓冲区
		 */
		virtual UINT8 SynchronouslyTranceive(const UINT8 out)
		{
			PUSHR_Type push = { 0 };
			if (GetMode() == MASTER)
			{
				push.Cfg.CONT = 0;				// 非连续传输
				push.Cfg.EOQ = 1;				// 最后一个数据
				push.Cfg.CTCNT = 1;				// 清空传输计数器
				push.Cfg.PCS = _BV(0);			// 默认PCS0作为片选
			}
			push.Cfg.TXDATA = out;				// 写入待发数据
			
			while (!ReadyToSend());
			PeripheralInfo::BaseAccess()->PUSHR_W = push.PUSH_Value;
			
			UINT8 in = 0;
			while (!DataAvailable());
			in = PeripheralInfo::BaseAccess()->POPR;
			PeripheralInfo::BaseAccess()->SR.RFDF = 1;

			return in;
		}
		virtual UINT16 SynchronouslyTranceive(const UINT16 out)
		{
			PUSHR_Type push = { 0 };
			if (GetMode() == MASTER)
			{
				push.Cfg.CONT = 0;				// 非连续传输
				push.Cfg.EOQ = 1;				// 最后一个数据
				push.Cfg.CTCNT = 1;				// 清空传输计数器
				push.Cfg.PCS = _BV(0);			// 默认PCS0作为片选
			}
			push.Cfg.TXDATA = out;				// 写入待发数据
			
			while (!ReadyToSend());
			PeripheralInfo::BaseAccess()->PUSHR_W = push.PUSH_Value;
			
			UINT16 in = 0;
			while (!DataAvailable());
			in = PeripheralInfo::BaseAccess()->POPR;
			PeripheralInfo::BaseAccess()->SR.RFDF = 1;

			return in;
		}
		virtual void* SynchronouslyTranceive(const void* _TxBuf, void* _RxBuf, size_type bufferSize)
		{
			// 此方法一般只在主机模式下使用
			PUSHR_Type push = { 0 };
			push.Cfg.CONT = 1;					// 连续传输
			push.Cfg.EOQ = 0;					// 非最后一个数据
			push.Cfg.CTCNT = 0;					// 不清空传输计数器
			push.Cfg.PCS = _BV(0);				// 默认PCS0作为片选

			if (GetDataBits() <= 8)
			{
				const UINT8* TxBuf = static_cast<const UINT8*>(_TxBuf);
				UINT8* RxBuf = static_cast<UINT8*>(_RxBuf);
				while (bufferSize > 1)
				{
					--bufferSize;

					// 装载数据
					push.Cfg.TXDATA = *TxBuf++;
					
					// 压入TX_FIFO缓冲区
					while (!ReadyToSend());
					PeripheralInfo::BaseAccess()->PUSHR_W = push.PUSH_Value;
					
					// 弹出RX_FIFO缓冲区
					Receive(*RxBuf++);
				}

				// 传输最后一个数据
				push.Cfg.CONT = 0;
				push.Cfg.EOQ = 1;

				// 并在传输完成后清空传输计数器，如果需要读取该值请在此之前作出处理
				push.Cfg.CTCNT = 1;

				// 压入最后一个数据到TX_FIFO并读取最后一个接收到的数据
				push.Cfg.TXDATA = *TxBuf++;
				while (!ReadyToSend());
				PeripheralInfo::BaseAccess()->PUSHR_W = push.PUSH_Value;
				Receive(*RxBuf++);
			}
			else
			{
				// 此处注释同上
				const UINT16* TxBuf = static_cast<const UINT16*>(_TxBuf);
				UINT16* RxBuf = static_cast<UINT16*>(_RxBuf);
				while (bufferSize > 1)
				{
					--bufferSize;
					push.Cfg.TXDATA = *TxBuf++;
					while (!ReadyToSend());
					PeripheralInfo::BaseAccess()->PUSHR_W = push.PUSH_Value;
					Receive(*RxBuf++);
				}
				push.Cfg.CONT = 0;
				push.Cfg.EOQ = 1;
				push.Cfg.CTCNT = 1;
				push.Cfg.TXDATA = *TxBuf++;
				while (!ReadyToSend());
				PeripheralInfo::BaseAccess()->PUSHR_W = push.PUSH_Value;
				Receive(*RxBuf++);
			}
			return _RxBuf;
		}




		/*
		 * @brief	通讯器发送接口方法
		 * @param	out 待发送数据的地址
		 *			发送帧支持字节、半字、字 类型数据，由通讯外设根据实际情况实现
		 *			发送前请务必配置好数据位
		 * @notice	在派生类中实现时如果不支持请返回false
		 * @ovride	派生类至少需要实现UINT8数据类型的发送方法，
		 *			其余类型默认分割成多个字节逐个发送，低字节在前（小端模式）
		 * @retval	发送成功返回true，否则返回false
		 */
		virtual bool Send(const UINT8 out)
		{
			PUSHR_Type push = { 0 };
			if (GetMode() == MASTER)
			{
				push.Cfg.CONT = 0;				// 非连续传输
				push.Cfg.EOQ = 1;				// 最后一个数据
				push.Cfg.CTCNT = 1;				// 清空传输计数器
				push.Cfg.PCS = _BV(0);			// 默认PCS0作为片选
			}
			push.Cfg.TXDATA = out;				// 写入待发数据
			
			while (!ReadyToSend());
			PeripheralInfo::BaseAccess()->PUSHR_W = push.PUSH_Value;
			
			// 一定要等待数据接收完成并写入RFDF后才可以算完成发送
			while (!DataAvailable());
			PeripheralInfo::BaseAccess()->SR.RFDF = 1;
			return 1;
		}
		virtual bool Send(const UINT16 out)
		{
			PUSHR_Type push = { 0 };
			if (GetMode() == MASTER)
			{
				push.Cfg.CONT = 0;				// 非连续传输
				push.Cfg.EOQ = 1;				// 最后一个数据
				push.Cfg.CTCNT = 1;				// 清空传输计数器
				push.Cfg.PCS = _BV(0);			// 默认PCS0作为片选
			}
			push.Cfg.TXDATA = out;				// 写入待发数据
			
			while (!ReadyToSend());
			PeripheralInfo::BaseAccess()->PUSHR_W = push.PUSH_Value;
			
			// 一定要等待数据接收完成并写入RFDF后才可以算完成发送
			while (!DataAvailable());
			PeripheralInfo::BaseAccess()->SR.RFDF = 1;
			return 1;
		}
		virtual bool Send(const void* _buffer, size_type bufferSize)
		{
			// 此方法一般只在主机模式下使用
			PUSHR_Type push = { 0 };
			push.Cfg.CONT = 1;					// 连续传输
			push.Cfg.EOQ = 0;					// 非最后一个数据
			push.Cfg.CTCNT = 0;					// 不清空传输计数器
			push.Cfg.PCS = _BV(0);				// 默认PCS0作为片选

			if (GetDataBits() <= 8)
			{
				const UINT8* buf = static_cast<const UINT8*>(_buffer);
				while (bufferSize > 1)
				{
					--bufferSize;
					while (!ReadyToSend());

					// 装载数据
					push.Cfg.TXDATA = *buf++;
					
					// 压入FIFO缓冲区
					PeripheralInfo::BaseAccess()->PUSHR_W = push.PUSH_Value;
				}

				// 传输最后一个数据
				push.Cfg.EOQ = 1;

				// 并在传输完成后清空传输计数器，如果需要读取该值请在此之前作出处理
				push.Cfg.CTCNT = 1;

				while (!ReadyToSend());
				push.Cfg.TXDATA = *buf++;
				PeripheralInfo::BaseAccess()->PUSHR_W = push.PUSH_Value;
				while (!DataAvailable());
				PeripheralInfo::BaseAccess()->SR.RFDF = 1;
			}
			else
			{
				const UINT16* buf = static_cast<const UINT16*>(_buffer);
				while (bufferSize > 1)
				{
					--bufferSize;
					while (!ReadyToSend());
					push.Cfg.TXDATA = *buf++;
					PeripheralInfo::BaseAccess()->PUSHR_W = push.PUSH_Value;
				}
				push.Cfg.EOQ = 1;
				push.Cfg.CTCNT = 1;
				while (!ReadyToSend());
				push.Cfg.TXDATA = *buf++;
				PeripheralInfo::BaseAccess()->PUSHR_W = push.PUSH_Value;
				while (!DataAvailable());
				PeripheralInfo::BaseAccess()->SR.RFDF = 1;
			}
			return 1;
		}




		/*
		 * @brief	SPI发送机就绪判断
		 * @retval	发送机就绪则返回true，否则返回false
		 * @ovride	派生类通讯器若无法判断是否就绪请根据实际情况返回true或false
		 *			若不支持此功能请返回true
		 */
		virtual bool ReadyToSend() const
		{
			return PeripheralInfo::BaseAccess()->SR.TFFF == 1;
		}



		/*
		 * @brief	SPI发送机发送完成判断
		 * @retval	发送机发送完成则返回true，否则返回false
		 */
		virtual bool SendComplete() const
		{
			return PeripheralInfo::BaseAccess()->SR.TCF == 1;
		}



		/*
		 * @brief	SPI发送机使能控制（开启/关闭接收机，检测使能状态）
		 * @retval	成功开启或关闭则返回1，否则返回0
		 * @notcie	K60 SPI收发机没有独立开关控制，模块使能时总是开启
		 */
		virtual int EnableTransmitter()
		{
			return 1;
		}
		virtual int DisableTransmitter()
		{
			return 0;
		}
		virtual bool IsTransmitterEnabled() const
		{
			return true;
		}




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
		virtual bool Receive(UINT8& in)
		{
			while (!DataAvailable());
			in = PeripheralInfo::BaseAccess()->POPR;
			PeripheralInfo::BaseAccess()->SR.RFDF = 1;
			return 1;
		}
		virtual bool Receive(UINT16& in)
		{
			while (!DataAvailable());
			in = PeripheralInfo::BaseAccess()->POPR;
			PeripheralInfo::BaseAccess()->SR.RFDF = 1;
			return 1;
		}
		virtual bool Receive(void* buffer, size_type bufferSize)
		{
			return SynchronousCommunicator::Receive(buffer, bufferSize);
		}



		/*
		 * @brief	SPI接收机接收到有效数据判断
		 * @retval	接收机接收到有效数据则返回true，否则返回false
		 */
		virtual bool DataAvailable() const
		{
			return PeripheralInfo::BaseAccess()->SR.RFDF == 1;
		}



		/*
		 * @brief	SPI接收机接收数据完成判断
		 * @retval	接收机接收数据完成则返回true，否则返回false
		 * @notice	当数据已经全部从外设的接收缓冲区中读入内存后则视为接收完成
		 */
		virtual bool ReceiveCompleted() const
		{
			return PeripheralInfo::BaseAccess()->SR.RFDF == 0;
		}



		/*
		 * @brief	SPI接收机使能控制（开启/关闭接收机，检测使能状态）
		 *			K60 SPI外设收发机不能独立开关，在模块开启是总是处于使能状态
		 */
		virtual int EnableReceiver()
		{
			return 1;
		}
		virtual int DisableReceiver()
		{
			return 0;
		}
		virtual bool IsReceiverEnabled() const
		{
			return true;
		}




		/*
		 * @brief	外设开启/关闭接口方法
		 * @retval	派生类无法实现与执行效果相符时请返回0，否则返回1
		 */
		virtual int EnablePeripheral()
		{
			PeripheralInfo::BaseAccess()->MCR.MDIS = 0;
			PeripheralInfo::BaseAccess()->MCR.HALT = 0;
			return 1;
		}
		virtual int DisablePeripheral()
		{
			PeripheralInfo::BaseAccess()->MCR.MDIS = 1;
			return 1;
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
			case DATE_AVAILABLE :
				PeripheralInfo::BaseAccess()->RSER.RFDF_RE = SET;
				PeripheralInfo::BaseAccess()->RSER.RFDF_DIRS = RESET;
				return 1;

			case READY_TO_SEND :
				PeripheralInfo::BaseAccess()->RSER.TFFF_RE = SET;
				PeripheralInfo::BaseAccess()->RSER.TFFF_DIRS = RESET;
				return 1;

			case SEND_COMPLETED :
				PeripheralInfo::BaseAccess()->RSER.TCF_RE = SET;
				return 1;

			default:
				return 0;
			}
		}
		virtual int DisableEventNotify(EventType evt)
		{
			switch (evt)
			{
			case DATE_AVAILABLE :
				PeripheralInfo::BaseAccess()->RSER.RFDF_RE = RESET;
				return 1;

			case READY_TO_SEND :
				PeripheralInfo::BaseAccess()->RSER.TFFF_RE = RESET;
				return 1;

			case SEND_COMPLETED :
				PeripheralInfo::BaseAccess()->RSER.TCF_RE = RESET;
				return 1;

			default:
				return 0;
			}
		}



		/*
		 * @brief	添加事件监听器，监听事件对象的某个事件
		 * @param	evt 对象的事件
		 *			FnPtr 触发该事件时的处理函数，由派生类在实现时转换到所需的类型
		 * @retval	成功注册某个事件监听器则返回1，否则返回0
		 */
		virtual int AddEventListener(EventType evt, void* FnPtr)
		{
			switch (evt)
			{
			case DATE_AVAILABLE :
				dataAvailableISR = FnPtr;
				PeripheralInfo::BaseAccess()->RSER.RFDF_RE = SET;
				PeripheralInfo::BaseAccess()->RSER.RFDF_DIRS = RESET;
				return 1;

			case READY_TO_SEND :
				readyToSendISR = FnPtr;
				PeripheralInfo::BaseAccess()->RSER.TFFF_RE = SET;
				PeripheralInfo::BaseAccess()->RSER.TFFF_DIRS = RESET;
				return 1;

			case SEND_COMPLETED :
				sendCompletedISR = FnPtr;
				PeripheralInfo::BaseAccess()->RSER.TCF_RE = SET;
				return 1;

			default:
				return 0;
			}
		}



		/*
		 * @brief	中断回调函数槽
		 */
		adxM3::Function< void(void*, UINT16) > dataAvailableISR;
		adxM3::Function< void(void*) > sendCompletedISR;
		adxM3::Function< void(void*) > readyToSendISR;



	protected:

		typedef typename PeripheralInfo::SCK SCKSignal;
		typedef typename PeripheralInfo::SOUT SOUTSignal;
		typedef typename PeripheralInfo::SIN SINSignal;
		typedef typename PeripheralInfo::PCS0_SS SelectSignal;
		
		// 必须生成信号引脚实例，必须有实例才能自动开启Port模块时钟
		SignalPin<SCKSignal> SCK;
		SignalPin<SOUTSignal> SOUT;
		SignalPin<SINSignal> SIN;
		SignalPin<SelectSignal> PCS0;

	};

}


extern "C"
{
	void SPI0_IRQHandler(void);
	void SPI1_IRQHandler(void);
	void SPI2_IRQHandler(void);
};

#endif  /*_ADX_K60_SPI_H_*/
