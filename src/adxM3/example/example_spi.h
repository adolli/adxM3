
#pragma once
#ifndef _EXAMPLESPI_H_
#define _EXAMPLESPI_H_

#include "example_base.h"
#include "adxM3.h"

using namespace adxM3;

namespace Example
{


	

	class SPI_EventBasedFullDuplexTranceive : public adxM3_Example
	{

		/*
		 * @brief	基于事件的SPI全双工传输测试
		 * @notice	测试时需要同一个MCU上的两个SPI外设，请将两SPI外设的MOSI，MISO，SCK
		 *			对应连接在一起，使用软件从设备选择，NSS引脚可忽略
		 *			其中一个SPI外设会配置成主机，另一个配置成从机
		 *			开始时，主机向从机发送100字节的数据，从机在此过程中也返回了100字节的数据
		 *			完成传输后会进行内容比较，判断是否发生传输错误
		 *			主机发送的内容在TXBUF1中，从机发送的内容在TXBUF2中
		 *			主机接收到的内容存在RXBUF1，从机接收到的内容存在RXBUF2中
		 */

	public:

		SPI_EventBasedFullDuplexTranceive(SynchronousCommunicator& _master, SynchronousCommunicator& _slave, Communicator& tips)
			: scom_master(_master), scom_slave(_slave), Tips(tips)
		{
			scom_master.SetBaudRate(2250000);	// 2.25Mbps
			scom_master.SetDataBits(16);
			scom_master.SetFirstBit(SynchronousCommunicator::MSB);
			scom_master.SetMode(SynchronousCommunicator::MASTER);
			scom_master.SetTransmissionMode(SynchronousCommunicator::FullDuplex);
			scom_master.SetParityMode(ConfigurableCommunicator::PARITY_CRC);
			
			scom_slave.SetDataBits(16);
			scom_slave.SetFirstBit(SynchronousCommunicator::MSB);
			scom_slave.SetMode(SynchronousCommunicator::SLAVE);
			scom_slave.SetTransmissionMode(SynchronousCommunicator::FullDuplex);
			scom_slave.SetParityMode(ConfigurableCommunicator::PARITY_CRC);

			// 初始化缓冲区
			for (int i = 0; i < BUFSIZE; ++i)
			{
				TXBUF1[i] = i + 1000;
				TXBUF2[i] = i;
				RXBUF1[i] = 0;
				RXBUF2[i] = 0;
 			}

			TXIDX1 = 0;
			TXIDX2 = 0;
			RXIDX1 = 0;
			RXIDX2 = 0;

			run();
		}



		void run()
		{
			// 建立主机事件监听器对象
			class Master_EventListener
			{
			public:

				static void CALLBACK OnReadyToSend(SynchronousCommunicator* This)
				{
					if (TXIDX1 < BUFSIZE - 1)
					{
						This->Send(TXBUF1[TXIDX1++]);
					}
					else if (TXIDX1 == BUFSIZE - 1)
					{
						// 发完最后一个数据后发送校验字
						This->Send(TXBUF1[TXIDX1++]);
						This->SendParityWord();
					}
					else
					{
						// 然后关闭事件通知，不需要再发送东西了
						This->DisableEventNotify(Communicator::READY_TO_SEND);
					}
				}

				static void CALLBACK OnDataAvailable(SynchronousCommunicator* This, UINT16 received)
				{
					if (RXIDX1 < BUFSIZE)
					{
						RXBUF1[RXIDX1++] = received;
					}
					else
					{
						ParityWord1 = received;
						This->DisableEventNotify(Communicator::DATE_AVAILABLE);
					}
				}

			};

			// 建立从机事件监听器对象
			class Slave_EventListener
			{
			public:

				static void CALLBACK OnReadyToSend(SynchronousCommunicator* This)
				{
					if (TXIDX2 < BUFSIZE - 1)
					{
						This->Send(TXBUF2[TXIDX2++]);
					}
					else if (TXIDX2 == BUFSIZE - 1)
					{
						// 发完最后一个数据后发送校验字
						This->Send(TXBUF2[TXIDX2++]);
						This->SendParityWord();
					}
					else
					{
						// 然后关闭事件通知，不需要再发送东西了
						This->DisableEventNotify(Communicator::READY_TO_SEND);
					}
				}

				static void CALLBACK OnDataAvailable(SynchronousCommunicator* This, UINT16 received)
				{
					if (RXIDX2 < BUFSIZE)
					{
						RXBUF2[RXIDX2++] = received;
					}
					else
					{
						ParityWord2 = received;
						This->DisableEventNotify(Communicator::DATE_AVAILABLE);
					}
				}

			};


			// 为通讯器注册事件监听器（注意顺序）
			scom_slave.AddEventListener(Communicator::DATE_AVAILABLE, Slave_EventListener::OnDataAvailable);
			scom_slave.AddEventListener(Communicator::READY_TO_SEND, Slave_EventListener::OnReadyToSend);
			scom_master.AddEventListener(Communicator::DATE_AVAILABLE, Master_EventListener::OnDataAvailable);
			scom_master.AddEventListener(Communicator::READY_TO_SEND, Master_EventListener::OnReadyToSend);
			

			// 提示信息
			Tips<<"开始传输了！同步速率为 "<<scom_master.GetBaudRate()<<" bps"<<endl;

			// 等待传输完成
			while (RXIDX2 < BUFSIZE || RXIDX1 < BUFSIZE);


			// 比较传输结果
			ErrorStatus status_master2slave = BufferCompare(TXBUF1, RXBUF2, 2 * BUFSIZE);
			ErrorStatus status_slave2master = BufferCompare(TXBUF2, RXBUF1, 2 * BUFSIZE);
			if (status_master2slave == SUCCESS && status_slave2master == SUCCESS)
			{
				if (!scom_master.GetErrors() && !scom_slave.GetErrors())
				{
					Tips<<"传输"<<BUFSIZE<<"个数据无任何错误！"<<endl;
					Tips<<"master CRC value is 0x"<<hex<<ParityWord1<<endl;
					Tips<<"slave CRC value is 0x"<<hex<<ParityWord2<<endl;
				}
			}


			// 输出传输结果
			Tips<<"RXBUF1 result"<<endl;
			for (int i = 0; i < BUFSIZE; ++i)
			{
				Tips<<RXBUF1[i]<<" ";
			}
			Tips<<endl;
			Tips<<"RXBUF2 result"<<endl;
			for (int i = 0; i < BUFSIZE; ++i)
			{
				Tips<<RXBUF2[i]<<" ";
			}
			Tips<<endl<<endl;


			// 测试结束
		}



	protected:

		SynchronousCommunicator &scom_master, &scom_slave;
		TextStream<Communicator> Tips;
	};




	class SPI_SingleThreadFullDuplexTranceive : public adxM3_Example
	{

		/*
		 * @brief	基于事件的SPI全双工传输测试
		 * @notice	测试时需要同一个MCU上的两个SPI外设，请将两SPI外设的MOSI，MISO，SCK
		 *			对应连接在一起，使用软件从设备选择，NSS引脚可忽略
		 *			其中一个SPI外设会配置成主机，另一个配置成从机
		 *			开始时，主机向从机发送100字节的数据，从机在此过程中也返回了100字节的数据
		 *			完成传输后会进行内容比较，判断是否发生传输错误
		 *			主机发送的内容在TXBUF1中，从机发送的内容在TXBUF2中
		 *			主机接收到的内容存在RXBUF1，从机接收到的内容存在RXBUF2中
		 */

	public:

		SPI_SingleThreadFullDuplexTranceive(SynchronousCommunicator& _master, SynchronousCommunicator& _slave, Communicator& tips)
			: scom_master(_master), scom_slave(_slave), Tips(tips)
		{
			scom_master.SetBaudRate(2250000);	// 2.25Mbps
			scom_master.SetDataBits(16);
			scom_master.SetFirstBit(SynchronousCommunicator::MSB);
			scom_master.SetMode(SynchronousCommunicator::MASTER);
			scom_master.SetTransmissionMode(SynchronousCommunicator::FullDuplex);
			scom_master.SetParityMode(ConfigurableCommunicator::PARITY_CRC);
			
			scom_slave.SetDataBits(16);
			scom_slave.SetFirstBit(SynchronousCommunicator::MSB);
			scom_slave.SetMode(SynchronousCommunicator::SLAVE);
			scom_slave.SetTransmissionMode(SynchronousCommunicator::FullDuplex);
			scom_slave.SetParityMode(ConfigurableCommunicator::PARITY_CRC);

			// 初始化缓冲区
			for (int i = 0; i < BUFSIZE; ++i)
			{
				TXBUF1[i] = i + 1000;
				TXBUF2[i] = i;
				RXBUF1[i] = 0;
				RXBUF2[i] = 0;
 			}

			TXIDX1 = 0;
			TXIDX2 = 0;
			RXIDX1 = 0;
			RXIDX2 = 0;

			run();
		}



		void run()
		{

			// 建立从机事件监听器对象
			class Slave_EventListener
			{
			public:

				static void CALLBACK OnReadyToSend(SynchronousCommunicator* This)
				{
					if (TXIDX2 < BUFSIZE - 1)
					{
						This->Send(TXBUF2[TXIDX2++]);
					}
					else if (TXIDX2 == BUFSIZE - 1)
					{
						// 发完最后一个数据后发送校验字
						This->Send(TXBUF2[TXIDX2++]);
						This->SendParityWord();
					}
					else
					{
						// 然后关闭事件通知，不需要再发送东西了
						This->DisableEventNotify(Communicator::READY_TO_SEND);
					}
				}

				static void CALLBACK OnDataAvailable(SynchronousCommunicator* This, UINT16 received)
				{
					if (RXIDX2 < BUFSIZE)
					{
						RXBUF2[RXIDX2++] = received;
					}
					else
					{
						ParityWord2 = received;
						This->DisableEventNotify(Communicator::DATE_AVAILABLE);
					}
				}

			};


			// 为通讯器注册事件监听器（注意顺序）
			scom_slave.AddEventListener(Communicator::DATE_AVAILABLE, Slave_EventListener::OnDataAvailable);
			scom_slave.AddEventListener(Communicator::READY_TO_SEND, Slave_EventListener::OnReadyToSend);
			

			// 主机开始发送
			scom_master.SynchronouslyTranceive(TXBUF1, RXBUF1, BUFSIZE);


			// 提示信息
			Tips<<"开始传输了！同步速率为 "<<scom_master.GetBaudRate()<<" bps"<<endl;

			// 等待传输完成
			while (RXIDX2 < BUFSIZE);


			// 比较传输结果
			ErrorStatus status_master2slave = BufferCompare(TXBUF1, RXBUF2, 2 * BUFSIZE);
			ErrorStatus status_slave2master = BufferCompare(TXBUF2, RXBUF1, 2 * BUFSIZE);
			if (status_master2slave == SUCCESS && status_slave2master == SUCCESS)
			{
				if (!scom_master.GetErrors() && !scom_slave.GetErrors())
				{
					Tips<<"传输"<<BUFSIZE<<"个数据无任何错误！"<<endl;
					Tips<<"master CRC value is 0x"<<hex<<ParityWord1<<endl;
					Tips<<"slave CRC value is 0x"<<hex<<ParityWord2<<endl;
				}
			}


			// 输出传输结果
			Tips<<"RXBUF1 result"<<endl;
			for (int i = 0; i < BUFSIZE; ++i)
			{
				Tips<<RXBUF1[i]<<" ";
			}
			Tips<<endl;
			Tips<<"RXBUF2 result"<<endl;
			for (int i = 0; i < BUFSIZE; ++i)
			{
				Tips<<RXBUF2[i]<<" ";
			}
			Tips<<endl<<endl;


			// 测试结束
		}



	protected:

		SynchronousCommunicator &scom_master, &scom_slave;
		TextStream<Communicator> Tips;
	};





}

#endif  /*_EXAMPLESPI_H_*/
