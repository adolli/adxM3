 
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
	 * @breif	ͨ���첽ͨѶ������
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
		 * @brief	UARTֹͣλ����/��ȡ
		 * @param	stb ֹͣλö��
		 *				@arg STOPBITS_05
		 *				@arg STOPBITS_10
		 *				@arg STOPBITS_15
		 *				@arg STOPBITS_20
		 * @retval	���÷����У��ɹ������򷵻�1�����򷵻�0
		 *			��ȡ�����У���֧��ֹͣλ����STOPBITS_10��û�ж�Ӧ��ֹͣλ�򷵻�ƥ��ֵ�����
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
		 * @brief	UART���սӿڷ���
		 * @param	in ���յ����ݴ�ŵ�λ��
		 *			����ֻ֧���ֽڣ������ֽڵ��������ͽ��ָ�ɶ���ֽ���������
		 *			����ǰ��������ú�����λ
		 * @retval	���ճɹ�����true�����򷵻�false
		 */
		virtual bool Receive(UINT8& in)
		{
			while (!DataAvailable());
			in = PeripheralInfo::BaseAccess()->D;
			return true;
		}





		/*
		 * @brief	UART���ջ����յ���Ч�����ж�
		 * @retval	���ջ����յ���Ч�����򷵻�true�����򷵻�false
		 */
		virtual bool DataAvailable() const
		{
			return PeripheralInfo::BaseAccess()->S1 & _BV(UART_BitBandType::S1_RDRF);
		}



		/*
		 * @brief	UART���ջ�������������ж�
		 * @retval	���ջ�������������򷵻�true�����򷵻�false
		 * @notice	�������Ѿ�ȫ��������Ľ��ջ������ж����ڴ������Ϊ�������
		 */
		virtual bool ReceiveCompleted() const
		{
			// ����S1[RDRF]λ�Ƿ����0
			return (~PeripheralInfo::BaseAccess()->S1) & _BV(UART_BitBandType::S1_RDRF);
		}



		/*
		 * @brief	UART���ջ�ʹ�ܿ��ƣ�����/�رս��ջ������ʹ��״̬��
		 * @retval	�ɹ�������ر��򷵻�1�����򷵻�0
		 * @notcie	����ȫ˫����ģʽ��EnableReceiveʱ�벻Ҫʹ��EnableTransmitter����
		 *			DisableReceiverʱ�벻Ҫʹ��DisableTransmitter����
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
		 * @brief	UART���ͽӿڷ���
		 * @param	out ���������ݵĵ�ַ
		 *			UARTֻ֧�ְ��ֽڴ��䣬��SetDataBits�����趨����λ
		 * @retval	���ͳɹ�����true�����򷵻�false
		 */
		virtual bool Send(const UINT8 out)
		{
			while (!ReadyToSend() || !SendComplete());
			PeripheralInfo::BaseAccess()->D = out;
			return true;
		}



		/*
		 * @brief	UART���ͽӿڷ�����DMA��
		 * @param	buffer ���������ݵĵ�ַ
		 *			bufferSize �����ֽ���
		 * @retval	���ͳɹ�����true�����򷵻�false
		 */
		virtual bool Send(const void* buffer, size_type bufferSize)
		{	
			//typedef typename PeripheralInfo::DMAController DMAController;

			//// �������ţ��Ժ�Ҫ���Զ໺����д���
			//while (!ReadyToSend() || !SendComplete());
			//Peripheral<DMAController> DMAC;

			//DMAC.template TransferRequest<UINT8* const, UINT8*>(
			//	(void*)&PeripheralInfo::BaseAccess()->D, buffer, (UINT16)bufferSize, 
			//	PeripheralInfo::DMATransmitRequest);

			//// �ύ��������󱣴���βͨ����
			//LastSubmittedDMAChannel = DMAC.GetAllocatedChannelIndex();
			//
			//PeripheralInfo::BaseAccess()->C5 |= _BV(UART_BitBandType::C5_TDMAS);
			//PeripheralInfo::BaseAccess()->C2 |= _BV(UART_BitBandType::C2_TIE);
			// 

			// ��ʱʹ����������ģʽ���Ժ����޸�DMA����
			const UINT8* buf = static_cast<const UINT8*>(buffer);
			while(bufferSize--)
			{
				Send(*buf++);
			}

			return true;
		}
		



		/*
		 * @brief	UART���ͻ������ж�
		 * @retval	���ͻ������򷵻�true�����򷵻�false
		 */
		virtual bool ReadyToSend() const
		{
			return PeripheralInfo::BaseAccess()->S1 & _BV(UART_BitBandType::S1_TDRE);
		}



		/*
		 * @brief	UART���ͻ���������ж�
		 * @retval	���ͻ���������򷵻�true�����򷵻�false
		 */
		virtual bool SendComplete() const
		{
			return PeripheralInfo::BaseAccess()->S1 & _BV(UART_BitBandType::S1_TC);
		}



		/*
		 * @brief	UART���ͻ�ʹ�ܿ��ƣ�����/�رս��ջ������ʹ��״̬��
		 * @retval	�ɹ�������ر��򷵻�1�����򷵻�0
		 * @notcie	��ȫ˫��ģʽ��EnableTransmitterʱ�벻Ҫʹ��EnableReceive����
		 *			DisableReceiverʱ�벻Ҫʹ��DisableTransmitter����
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
		 * @brief	UART����������
		 * @param	baudrate ������ֵ������ʵ��������ò����ʣ���λbps
		 * @retval	�ɹ������򷵻�1�����򷵻�0
		 */
		virtual int SetBaudRate(const unsigned int baudrate)
		{
			UINT32 ModuleClockFreq = PeripheralInfo::ModuleClock::Frequency;
			
			// ����һ��������㷨��regval��5λ����BRFA��ֵ��ʣ�µľ���BD��ֵ
			UINT32 regval = (ModuleClockFreq << 1) / baudrate;

			// regval ����
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
		 * @brief	UART��ȡ������
		 * @retval	���ز�����ֵ����λbps
		 */
		virtual unsigned int GetBaudRate() const
		{
			UINT32 ModuleClockFreq = PeripheralInfo::ModuleClock::Frequency;
			UINT8 BRFA = PeripheralInfo::BaseAccess()->C4 & _BF(4, 0);
			UINT8 BDH_SBR = PeripheralInfo::BaseAccess()->BDH & _BF(4, 0);
			UINT8 BDL_SBR = PeripheralInfo::BaseAccess()->BDL;
			
			// �����ʼĴ����ϳ�ֵ
			UINT32 regval = (BDH_SBR << 13) | (BDL_SBR << 5) | BRFA;

			return (ModuleClockFreq << 1) / regval;
		}

		

		/*
		 * @brief	UART����λ����
		 * @param	datebits ����λ�����δ��������ݵ���Чλ��
		 * @retval	�ɹ������򷵻�1�����򷵻�0
		 * @notice	UARTֻ��8λ��9λ��������
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
		 * @brief	UART��ȡ����λ
		 * @retval	���ص���ͨѶ�д������Ч����λ��
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
		 * @brief	UARTУ��ģʽ����
		 * @param	parity У��ģʽö�٣�������PARITYö�ٵ��κ�һ��
		 *				@arg PARITY_NONE
		 *				@arg PARITY_EVEN
		 *				@arg PARITY_ODD
		 * @retval	�ɹ������򷵻�1�����򷵻�0
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
		 * @brief	UART��ȡУ��ģʽ
		 * @retval	��ǰ��У��ģʽ
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
		 * @brief	UART����У���֣�һ������CRCУ��ģʽ�����һ��У���֣�
		 * @ovride	UART��֧�ָ÷���
		 */
		virtual int SendParityWord()
		{
			return 0;
		}




		/*
		 * @brief	UART��ȡ����״̬
		 * @ovride	�޴���ʱ����0��������ERT�еĴ���λ�����
		 * @notice	��ǰ��δʵ�ִ˹���
		 */
		virtual int GetErrors() const
		{
			return 0;
		}





		/*
		 * @brief	UART�����״���λ
		 * @param	firstbit �״���λö��
		 *				@arg MSB ���λ�ȴ�
		 *				@arg LSB ���λ�ȴ�
		 * @retval	�ɹ������򷵻�1�����򷵻�0
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
		 * @brief	UART��ȡ�״���λģʽ
		 * @retval	��ǰ�״���λģʽ
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
		 * @brief	UART����ģʽ����
		 * @param	mode ģʽö�٣�ֻ����ģʽ��
		 *				@arg MASTER ����ͨѶ������ģʽ
		 * @retval	�ɹ������򷵻�1�����򷵻�0
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
		 * @brief	UART����ģʽ����
		 * @retval	UART��Զ������ģʽ
		 */
		virtual MSM GetMode() const
		{
			return MASTER;
		}





		/*
		 * @brief	ͨѶ�����ô��乤��ģʽ
		 * @param	transmission_mode ���乤��ģʽö��
		 *				@arg FullDuplex ȫ˫��
		 *				@arg HalfDuplex_Tx/HalfDuplex_Rx ��˫����/��˫����
		 *				@arg Simplex_Tx/Simplex_Rx ������/������
		 * @retval	�ɹ������򷵻�1�����򷵻�0
		 * @ovride	��������ͨѶ����֧��ĳЩģʽ�򷵻�0
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


				// ��˫��˵��
				// C1[LOOPS] �������ջ��Ƿ���RXD�ź��������ӣ�SETʱ�����ӣ�RESETʱ����
				// C1[RSRC]  �������ջ������ź�����TXD�����źŻ��Ƿ��ͻ�����źţ�
				//			 SETʱ���ӵ�TXD�����źţ�RESETʱ���ӵ����ͻ�����ź�
				// C3[TXDIR] ����TXD�ź����ŵķ���SETʱΪ�������RESETʱΪ���뷽��

			case HalfDuplex_Rx :
				// ��˫������򿪷��ͻ�����ջ�
				EnableReceiver();
				EnableTransmitter();

				// ����TXD����ΪRXD����״̬

				PeripheralInfo::BaseAccess()->C1 |= _BV(UART_BitBandType::C1_LOOPS);
				PeripheralInfo::BaseAccess()->C1 |= _BV(UART_BitBandType::C1_RSRC);
				PeripheralInfo::BaseAccess()->C3 &= ~_BV(UART_BitBandType::C3_TXDIR);
				return 1;

			case HalfDuplex_Tx :
				// ��˫������򿪷��ͻ�����ջ�
				EnableReceiver();
				EnableTransmitter();

				// ����TXD����ΪTXD����״̬

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
		 * @brief	UART��ȡ���乤��ģʽ
		 * @retval	���乤��ģʽ
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
				// ��˫����֧
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
		 * @brief	�¼�֪ͨ���ؿ���
		 * @param	evt ��Ҫ������ر�֪ͨ���¼�����
		 * @retval	�ɹ�ʹ��/��ֹĳ���¼�֪ͨ�򷵻�1�����򷵻�0
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
		 * @brief	����¼��������������¼������ĳ���¼�
		 *			���������¼�֪ͨ
		 * @param	evt ������¼�
		 *			FnPtr �������¼�ʱ�Ĵ�����������������ʵ��ʱת�������������
		 * @retval	�ɹ�ע��ĳ���¼��������򷵻�1�����򷵻�0
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
		 * @brief	���迪��/�رսӿڷ���
		 * @retval	�������޷�ʵ����ִ��Ч�����ʱ�뷵��0�����򷵻�1
		 *			e.g. ������EnablePeripheral������ĳ����ʼ�մ��ڿ���״̬�򷵻�1
		 *				 ������DisablePeripheral������ĳ����ʼ�մ��ڿ���״̬�޷��ر�ʱ�뷵��0
		 */
		virtual int EnablePeripheral()
		{
			// ��Ϊ��MCU����STOP mode �¡�����ֹͣģ��ʱ��
			PeripheralInfo::BaseAccess()->C1 &= ~_BV(UART_BitBandType::C1_UARTSWAI);
			return 1;
		}
		virtual int DisablePeripheral()
		{
			// ��Ϊ��MCU����STOP mode ��ֹͣģ��ʱ��
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
			// ע��ȫ��ʵ��ָ��
			PeripheralInfo::InstancePtr = this;
			
			// ����ģ��ʱ��
			ClockSource::ClockOn();


			// ֹͣλ
			SetStopBits(stopBits);

			// ����λ
			SetDataBits(dataBits);

			// У��λ
			SetParityMode(parity);

			// ���������ã��ο��̼����ṩ���㷨��
			SetBaudRate(baudRate);


			// ���乤������
			SetTransmissionMode(iom);


			// NVIC
			CM4::$NVIC::SetPriority(
				PeripheralInfo::TranceiveIQR, 
				PeripheralInfo::TranceiveInteruptPriority);
			CM4::$NVIC::EnableIRQ(PeripheralInfo::TranceiveIQR);
			

			// ���ʹ������
			EnablePeripheral();

		}



		virtual ~Peripheral()
		{
			ClockSource::ClockOff();
		}



		/*
		 * @brief	�жϻص�������
		 */
		adxM3::Function< void(void*) > sendCompletedISR;
		adxM3::Function< void(void*) > readyToSendISR;
		
		// �û�������
		EventListenerWrapper< ReceiveListener<UINT8> > dataAvailableListenerWrapper;

	protected:
	

		typedef typename PeripheralInfo::TxSignal TxSignal;
		typedef typename PeripheralInfo::RxSignal RxSignal;
		
		// ���������ź�����ʵ����������ʵ�������Զ�����Portģ��ʱ��
		// ����ʱ���������л�MUX���ȵ��շ�ͨ��ʱ������Ҫ�Զ��л�
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
