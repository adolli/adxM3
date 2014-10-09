
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
		 * @brief	�����¼���SPIȫ˫���������
		 * @notice	����ʱ��Ҫͬһ��MCU�ϵ�����SPI���裬�뽫��SPI�����MOSI��MISO��SCK
		 *			��Ӧ������һ��ʹ��������豸ѡ��NSS���ſɺ���
		 *			����һ��SPI��������ó���������һ�����óɴӻ�
		 *			��ʼʱ��������ӻ�����100�ֽڵ����ݣ��ӻ��ڴ˹�����Ҳ������100�ֽڵ�����
		 *			��ɴ�����������ݱȽϣ��ж��Ƿ����������
		 *			�������͵�������TXBUF1�У��ӻ����͵�������TXBUF2��
		 *			�������յ������ݴ���RXBUF1���ӻ����յ������ݴ���RXBUF2��
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

			// ��ʼ��������
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
			// ���������¼�����������
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
						// �������һ�����ݺ���У����
						This->Send(TXBUF1[TXIDX1++]);
						This->SendParityWord();
					}
					else
					{
						// Ȼ��ر��¼�֪ͨ������Ҫ�ٷ��Ͷ�����
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

			// �����ӻ��¼�����������
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
						// �������һ�����ݺ���У����
						This->Send(TXBUF2[TXIDX2++]);
						This->SendParityWord();
					}
					else
					{
						// Ȼ��ر��¼�֪ͨ������Ҫ�ٷ��Ͷ�����
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


			// ΪͨѶ��ע���¼���������ע��˳��
			scom_slave.AddEventListener(Communicator::DATE_AVAILABLE, Slave_EventListener::OnDataAvailable);
			scom_slave.AddEventListener(Communicator::READY_TO_SEND, Slave_EventListener::OnReadyToSend);
			scom_master.AddEventListener(Communicator::DATE_AVAILABLE, Master_EventListener::OnDataAvailable);
			scom_master.AddEventListener(Communicator::READY_TO_SEND, Master_EventListener::OnReadyToSend);
			

			// ��ʾ��Ϣ
			Tips<<"��ʼ�����ˣ�ͬ������Ϊ "<<scom_master.GetBaudRate()<<" bps"<<endl;

			// �ȴ��������
			while (RXIDX2 < BUFSIZE || RXIDX1 < BUFSIZE);


			// �Ƚϴ�����
			ErrorStatus status_master2slave = BufferCompare(TXBUF1, RXBUF2, 2 * BUFSIZE);
			ErrorStatus status_slave2master = BufferCompare(TXBUF2, RXBUF1, 2 * BUFSIZE);
			if (status_master2slave == SUCCESS && status_slave2master == SUCCESS)
			{
				if (!scom_master.GetErrors() && !scom_slave.GetErrors())
				{
					Tips<<"����"<<BUFSIZE<<"���������κδ���"<<endl;
					Tips<<"master CRC value is 0x"<<hex<<ParityWord1<<endl;
					Tips<<"slave CRC value is 0x"<<hex<<ParityWord2<<endl;
				}
			}


			// ���������
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


			// ���Խ���
		}



	protected:

		SynchronousCommunicator &scom_master, &scom_slave;
		TextStream<Communicator> Tips;
	};




	class SPI_SingleThreadFullDuplexTranceive : public adxM3_Example
	{

		/*
		 * @brief	�����¼���SPIȫ˫���������
		 * @notice	����ʱ��Ҫͬһ��MCU�ϵ�����SPI���裬�뽫��SPI�����MOSI��MISO��SCK
		 *			��Ӧ������һ��ʹ��������豸ѡ��NSS���ſɺ���
		 *			����һ��SPI��������ó���������һ�����óɴӻ�
		 *			��ʼʱ��������ӻ�����100�ֽڵ����ݣ��ӻ��ڴ˹�����Ҳ������100�ֽڵ�����
		 *			��ɴ�����������ݱȽϣ��ж��Ƿ����������
		 *			�������͵�������TXBUF1�У��ӻ����͵�������TXBUF2��
		 *			�������յ������ݴ���RXBUF1���ӻ����յ������ݴ���RXBUF2��
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

			// ��ʼ��������
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

			// �����ӻ��¼�����������
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
						// �������һ�����ݺ���У����
						This->Send(TXBUF2[TXIDX2++]);
						This->SendParityWord();
					}
					else
					{
						// Ȼ��ر��¼�֪ͨ������Ҫ�ٷ��Ͷ�����
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


			// ΪͨѶ��ע���¼���������ע��˳��
			scom_slave.AddEventListener(Communicator::DATE_AVAILABLE, Slave_EventListener::OnDataAvailable);
			scom_slave.AddEventListener(Communicator::READY_TO_SEND, Slave_EventListener::OnReadyToSend);
			

			// ������ʼ����
			scom_master.SynchronouslyTranceive(TXBUF1, RXBUF1, BUFSIZE);


			// ��ʾ��Ϣ
			Tips<<"��ʼ�����ˣ�ͬ������Ϊ "<<scom_master.GetBaudRate()<<" bps"<<endl;

			// �ȴ��������
			while (RXIDX2 < BUFSIZE);


			// �Ƚϴ�����
			ErrorStatus status_master2slave = BufferCompare(TXBUF1, RXBUF2, 2 * BUFSIZE);
			ErrorStatus status_slave2master = BufferCompare(TXBUF2, RXBUF1, 2 * BUFSIZE);
			if (status_master2slave == SUCCESS && status_slave2master == SUCCESS)
			{
				if (!scom_master.GetErrors() && !scom_slave.GetErrors())
				{
					Tips<<"����"<<BUFSIZE<<"���������κδ���"<<endl;
					Tips<<"master CRC value is 0x"<<hex<<ParityWord1<<endl;
					Tips<<"slave CRC value is 0x"<<hex<<ParityWord2<<endl;
				}
			}


			// ���������
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


			// ���Խ���
		}



	protected:

		SynchronousCommunicator &scom_master, &scom_slave;
		TextStream<Communicator> Tips;
	};





}

#endif  /*_EXAMPLESPI_H_*/
