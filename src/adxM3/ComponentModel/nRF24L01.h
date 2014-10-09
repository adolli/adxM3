  
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
		typedef UINT8 STAWORD;										// nRF״̬��

	private:



		/*
		 * nRF24L01����ö��
		 */
		enum CMD
		{
			READ_REG				= 0x00,							// ���Ĵ���
			WRITE_REG				= 0x20,							// д�Ĵ���
			RD_RX_PLOAD				= 0x61,							// ��ȡ��������ָ��
			WR_TX_PLOAD				= 0xA0,							// д��������ָ��
			FLUSH_TX				= 0xE1,							// ��ϴ���� FIFOָ��
			FLUSH_RX				= 0xE2,							// ��ϴ���� FIFOָ��
			REUSE_TX_PL				= 0xE3,							// �����ظ�װ������ָ��
			NOP						= 0xFF							// ����
		};




		/*
		 * nRF24L01�Ĵ�����ַö��
		 */
		enum REG 
		{
			CONFIG					= 0x00,							// �����շ�״̬��CRCУ��ģʽ�Լ��շ�״̬��Ӧ��ʽ
			EN_AA					= 0x01,							// �Զ�Ӧ��������
			EN_RXADDR				= 0x02,							// �����ŵ�����
			SETUP_AW				= 0x03,							// �շ���ַ�������
			SETUP_RETR				= 0x04,							// �Զ��ط���������
			RF_CH					= 0x05,							// ����Ƶ��(ͨ��)����
			RF_SETUP				= 0x06,							// �������ʡ����Ĺ�������
			STATUS					= 0x07,							// ״̬�Ĵ���
			OBSERVE_TX				= 0x08,							// ���ͼ�⹦��
			CD						= 0x09,							// ��ַ���           
			RX_ADDR_P0				= 0x0A,							// Ƶ��0�������ݵ�ַ
			RX_ADDR_P1				= 0x0B,							// Ƶ��1�������ݵ�ַ
			RX_ADDR_P2				= 0x0C,							// Ƶ��2�������ݵ�ַ
			RX_ADDR_P3				= 0x0D,							// Ƶ��3�������ݵ�ַ
			RX_ADDR_P4				= 0x0E,							// Ƶ��4�������ݵ�ַ
			RX_ADDR_P5				= 0x0F,							// Ƶ��5�������ݵ�ַ
			TX_ADDR					= 0x10,							// ���͵�ַ�Ĵ���
			RX_PW_P0				= 0x11,							// ����Ƶ��0�������ݳ���
			RX_PW_P1				= 0x12,							// ����Ƶ��0�������ݳ���
			RX_PW_P2				= 0x13,							// ����Ƶ��0�������ݳ���
			RX_PW_P3				= 0x14,							// ����Ƶ��0�������ݳ���
			RX_PW_P4				= 0x15,							// ����Ƶ��0�������ݳ���
			RX_PW_P5				= 0x16,							// ����Ƶ��0�������ݳ���
			FIFO_STATUS				= 0x17							// FIFO״̬�Ĵ�������
		};

	
	public:

		/*
		 * CONFIG �Ĵ���λ����
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
		 * STATUS �Ĵ���λ����
		 */
		enum STA
		{
			FLAG_DataReceived		= 0x40,
			FLAG_DataSent			= 0x20,
			FLAG_MaxRetry			= 0x10,
			FLAG_TxFull			= 0x01
		};



		/*
		 * FIFO_STATUS �Ĵ���λ����
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
		 * RF_SETUP �Ĵ���λ����
		 */
		enum RF
		{
			LNA_HCURR				= 0x01,							// ����������ʹ��λ
			PWR_0dBm				= 0x06,							// 0dBm����
			PWR_n6dBm				= 0x04,							// -6dBm����
			PWR_n12dBm				= 0x02,							// -12dBm����
			PWR_n18dBm				= 0x00,							// -18dBm����
			DataRate_2Mbps			= 0x08,							// 2Mbps��������
			DataRate_1Mbps			= 0x00							// 1Mbps��������
		};


		

		/*
		 * nRF24L01����ͨ������
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


	// ת���� RxPipe ����ͨ�����ճ��ȼĴ�����ַ
	#define REG_PIPE_PayLoadWidth(P)	((P)+0x11)

	// ת���� RxPipe ����ͨ�����յ�ַ�Ĵ�����ַ
	#define REG_PIPE_RxAddress(P)		((P)+0x0A)

	// ת���� RxPipe ����ͨ���Զ�Ӧ��ʹ�ܼĴ���λ����
	#define AutoAck_PIPE(P)				(_BV(P))

	// ת���� RxPipe ����ͨ������ʹ�ܼĴ���λ����
	#define EN_RxPIPE(P)				(_BV(P))


		enum EVT
		{
			TRANSMISSION_FAIL			= 0x10,
			TRANSMISSION_COMPLETE		= 0x20,
			RECEIVED					= 0x40
		};



// �ر�������union�ж�������ľ���
#pragma diag_suppress 1165


		/*
		 * @brief	��������Ϣ��
		 *			�󶨽����ߺ��ֱ�����䷢�����ݰ�
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
		 * @brief	��������Ϣ��
		 *			�󶨷����ߺ�ɽ��յ��䷢�������ݰ�
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
		 * @brief	�������ݰ��ṹ
		 * @tparam	T �û����ݽṹ
		 *			PacketSize �û����ݰ���С���ֽ��������벻Ҫ����32
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


			// ���ߵ�����趨
			RFConfig(40, LNA_HCURR|PWR_0dBm|DataRate_2Mbps);

			CE = 0;
			FlushTx();
			FlushRx();
		}


		~nRF24L01()
		{
			FlushTx();
			FlushRx();

			// ģ��ͣ��ʡ��
			WriteReg(WRITE_REG|CONFIG, 0);
		}




		/*
		 * @brief	�򻺳���д����߶���һ�����ݰ�
		 * @param	pBuf ��д�����������ݻ�����ָ��
		 * @retval	����״̬�Ĵ�����ֵ
		 */
		STAWORD	TransmitPacket(const BYTE* pBuf)
		{
			CE = 0;
			while (!ReadyToSend());
			STAWORD status = WriteBuf(WR_TX_PLOAD, pBuf, PayLoadWidth);	// װ������ 
			CE = 1;																// ��������		
			MCU.Delay_us(10);													// 10us��ʼ����
			return status;
		}
		STAWORD ReceivePacket(BYTE* pBuf)
		{
			CE = 0;
			while (!DataAvailable());
			STAWORD status = ReadBuf(RD_RX_PLOAD, pBuf, PayLoadWidth);		// �ӽ��ջ�������������
			return status;
		}




		/*
		 * @brief	�������ߵ����
		 * @param	channel RFƵ������Χ��0~125
					setup RF����������д��RF_SETUP�Ĵ�����ֵ��
		 */
		void RFConfig(BYTE channel, BYTE setup)
		{
			WriteReg(WRITE_REG|RF_CH, channel);								// ���� RF Ƶ��
			WriteReg(WRITE_REG|RF_SETUP, setup);							// TX_PWR,Datarate,LNA
		}



		/*
		 * @brief	���ý���ͨ�����ݰ�����
		 * @param	pipex ͨ��x��ȡֵRxPipe��xȡ0~5
					pl_width ���ճ��ȣ���Χ1~32
		 */
		void SetPayLoadWidth(PIPE pipex, BYTE pl_width)
		{
			WriteReg(WRITE_REG|REG_PIPE_PayLoadWidth(pipex), pl_width);			// ���ý������ݳ���
			PayLoadWidth = pl_width;
		}


		
		/*
		 * @brief	��ȡ����ͨ�����ݰ����ճ���
		 * @param	pipex ͨ��x��ȡֵRxPipe��xȡ0~5
		 * @retval 	���ճ��ȣ���Χ1~32
		 */
		BYTE GetPayLoadWidth(PIPE pipex)
		{
			PayLoadWidth = ReadReg(READ_REG|pipex);
			return PayLoadWidth;
		}




		/*
		 * @brief	��ȡ��ǰnRFģ��״̬
		 * @retval 	״̬�Ĵ���ֵ
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
		 * @brief	��ϴ�շ�������
		 * @retval 	״̬�Ĵ���ֵ
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
		 * @brief	�����͹ر�nRF�¼�֪ͨ
		 * @param 	evt nRF�¼����ƣ������������¼��İ�λ�����
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
		 * @brief	����жϱ��
		 * @param	flag STAö���е�ֵ�����ⰴλ�����
		 */
		void ClearFlag(UINT8 flag)
		{
			WriteReg(WRITE_REG|STATUS, flag);
		}




		/*
		 * @brief	�趨��������ȥ����Ŀ��
		 * @param	receiver ���ջ���Ϣ
		 */
		nRF24L01& operator[](ReceiverInfo& receiver)
		{
			CE = 0;
			WriteBuf(WRITE_REG|TX_ADDR, receiver.AddressBytes, nRF_ADDR_WIDTH);	// д���ջ���ַ
			WriteBuf(WRITE_REG|RX_ADDR_P0, receiver.AddressBytes, nRF_ADDR_WIDTH);	// �������յ�ַPipe0Ϊ���ջ���ַ�����Զ�Ӧ����ջ������ź�
			//WriteReg(WRITE_REG|EN_RXADDR, EN_RxPIPE(PIPE0));					// ���ջ����� Pipe0 ʹ��

			// ���ͻ�ֻ��Ҫ���ú÷��͵ĳ��ȣ����ջ�����Ҫ���ý��ճ��ȼĴ���
			PayLoadWidth = receiver.PayLoadWidth;
	
			//WriteReg(WRITE_REG|EN_AA, AutoAck_PIPE(PIPE0));					// Pipe0 �����Զ�Ӧ��
	
			//WriteReg(WRITE_REG|SETUP_RETR, 0x1A);								// �����Զ��ط����ʱ��:500us + 86us;����Զ��ط�����:10��
			BYTE regval = ReadReg(READ_REG|CONFIG);
			regval &= (UINT8)~0x0F;
			regval |= Tx|PWR_UP|CRCOCT|EN_CRC;
			WriteReg(WRITE_REG|CONFIG, regval);
			return *this;
		}



		/*
		 * @brief	�趨�����������Ե�Ŀ��
		 * @param	transmitter ���ͻ���Ϣ
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
			MCU.Delay_us(130);	// 130us��������ģʽ
			return *this;
		}
		


			
		/*
		 * @brief	�����ȴ����ͻ��߽������
		 * @param	pBuf ��д�����������ݻ�����ָ��
		 * @retval	�������Լ���
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
			WriteReg(WRITE_REG|STATUS, status);						// ������
			return *this;
		}
		nRF24L01& operator>>(BYTE* pBuf)
		{
			STAWORD status = ReceivePacket(pBuf);
			WriteReg(WRITE_REG|STATUS, status);						// ������
			return *this;
		}




		//CALLBACK		EXTIISR	 &	OnEvent(void* capture = NULL);




		/*
		 * @brief	״̬��⺯����ͨ����ȡSTATUS�Ĵ�������FIFO_STATUS�Ĵ����ж��Ƿ���������
		 * @retval	true������㣬false���������
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
		 * @brief	д�Ĵ���ֵ
		 * @param	reg �Ĵ�����ַ
					val	д���ֵ
		 * @retval	����nRF24L01״̬�Ĵ�����ֵ
		 */
		STAWORD	WriteReg(const BYTE reg, const BYTE val)
		{
			// �ȴ���reg���ٴ���val�����صĸ��ֽ�Ϊstatus word�����������ƶ�8λ
			BYTE tx[2] = { reg, val };
			BYTE rx[2] = { 0 };
			
			SPI.SynchronouslyTranceive(tx, rx, 2);
			return rx[0];
		}



		/*
		 * @brief	���Ĵ���ֵ
		 * @param	reg �Ĵ�����ַ
		 * @retval	�Ĵ�����ֵ
		 */
		BYTE ReadReg(const BYTE reg)
		{
			// �ȴ���reg���ٴ����հ׵��ֽڣ����صĸ��ֽ�Ϊstatus word�����õ��ֽ���regval
			BYTE tx[2] = { reg, 0xFF };
			BYTE rx[2] = { 0 };
			
			SPI.SynchronouslyTranceive(tx, rx, 2);
			return rx[1];
		}



		/*
		 * @brief	��дnRF24L01������
		 * @param	reg �Ĵ�����ַ
					pBuf ��д�����������ݻ�����ָ��
					len ��д����
		 * @retval	״̬�Ĵ�����ֵ
		 */
		STAWORD	WriteBuf(const BYTE reg, const BYTE* pBuf, const BYTE len)
		{
			// ����һ��������������nrf���η��Ͳ�����32�ֽڣ�
			// �����ڶ�ջ��һ��33�ֽڵĽ��ջ������ͷ��ͻ�����
			// ����Ŀ��Ϊ����ӦSynchronousCommunicator�Ľӿڷ���
			// ���ͻ�������һ�ֽ�Ϊreg��ַ��������len���ֽ�Ϊ�����͵�����
			// �ὫpBuf�е�����ȫ������ȥ
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
			
			// �����൱��ʹ��rx_buf������Ϣ
			SPI.SynchronouslyTranceive(rx_buf, rx_buf, len + 1);
			for (int i = 1; i <= len; ++i)
			{
				// �ѽ��յ������ݿ�����pBuf
				pBuf[i - 1] = rx_buf[i];
			}
			return rx_buf[0];
		}
	




	protected:

		SynchronousCommunicator& SPI;						// SPIͨѶ�ӿ�
		GeneralPurposeInputOutputPin& CE;					// ģ��ѡ��active low
		GeneralPurposeInputOutputPin& IRQ;					// �ж��ź���at rising edge
		BlockingDelayController& MCU;						// ���Ŀ������ṩ��ʱ����
		
		// ����ͨ���������ݳ���
		BYTE PayLoadWidth;											
	};

}

#endif  /*_ADX_CM_NRF24L01_H_*/
