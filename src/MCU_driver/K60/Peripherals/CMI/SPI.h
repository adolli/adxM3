
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
	 * @breif	��������ͨѶ�ӿ�����
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
			// ע��ȫ��ʵ��ָ��
			PeripheralInfo::InstancePtr = this;
			
			// ����ģ��ʱ��
			ClockSource::ClockOn();

			EnablePeripheral();

			// ͣ����������
			PeripheralInfo::BaseAccess()->MCR.HALT = 1;

			SetMode(mode);
			SetBaudRate(baudrate);
			SetDataBits(databits);
			SetFirstBit(firstbit);
			SetClockPolarity(polarity);
			SetClockPhase(phase);

			// �����������֮���PCS inactive��ƽ��ͳ���ʱ����ʱ��Ϊ177ns
			// DT value == 8
			// tDT == 177ns
			//PeripheralInfo::BaseAccess()->CTAR[0].DT = 2;
			
			// ��������֮���ÿ����֮��ļ����Ϊ177ns
			// ASC value == 8
			// tASC == 177ns
			//PeripheralInfo::BaseAccess()->CTAR[0].ASC = 2;

			//PeripheralInfo::BaseAccess()->CTAR[0].CSSCK = 2;


			// ����PCS�źŷǻʱΪ��
			PeripheralInfo::BaseAccess()->MCR.PCSIS = 0x3F;

			// ʹ��Ӳ������ģʽ
			PeripheralInfo::BaseAccess()->MCR.DIS_TXF = 0;
			PeripheralInfo::BaseAccess()->MCR.DIS_RXF = 0;

			// ���FIFO
			PeripheralInfo::BaseAccess()->MCR.CLR_TXF = 1;
			PeripheralInfo::BaseAccess()->MCR.CLR_RXF = 1;
			

			// ���SR�Ĵ�����־λ
			PeripheralInfo::BaseAccess()->SR.EOQF = 1;
			PeripheralInfo::BaseAccess()->SR.TFUF = 1;
			PeripheralInfo::BaseAccess()->SR.TFFF = 1;
			PeripheralInfo::BaseAccess()->SR.RFOF = 1;
			PeripheralInfo::BaseAccess()->SR.RFDF = 1;


			// ����NVIC
			CM4::$NVIC::SetPriority(PeripheralInfo::PeripheralIRQ,
				PeripheralInfo::InteruptPriority);
			CM4::$NVIC::EnableIRQ(PeripheralInfo::PeripheralIRQ);

			// ȡ��ͣ��ģʽ
			PeripheralInfo::BaseAccess()->MCR.HALT = 0;
		}

		virtual ~Peripheral()
		{
			ClockSource::ClockOff();
		}




		/*
		 * @brief	SPI����ʱ�Ӽ���
		 * @param	polarity ʱ�Ӽ���ö��
		 *				@arg CPOL_LOW
		 *				@arg CPOL_HIGH
		 * @retval	���óɹ��򷵻�1�����򷵻�0
		 */
		virtual int SetClockPolarity(const CPOL polarity)
		{
			STATIC_ASSERT(CPOL_HIGH == 1 && CPOL_LOW == 0,
				enumerate_value_is_not_campacted_with_this_configuration);
			PeripheralInfo::BaseAccess()->CTAR[0].CPOL = polarity;
			return 1;
		}



		/*
		 * @brief	SPI��ȡʱ�Ӽ���
		 * @retval	polarity ʱ�Ӽ���ö��
		 */
		virtual CPOL GetClockPolarity() const
		{
			STATIC_ASSERT(CPOL_HIGH == 1 && CPOL_LOW == 0,
				enumerate_value_is_not_campacted_with_this_configuration);
			return CPOL(PeripheralInfo::BaseAccess()->CTAR[0].CPOL);
		}




		/*
		 * @brief	SPI����ʱ����λ
		 * @param	phase ʱ����λö��
		 *				@arg CPHA_1st_EDGE
		 *				@arg CPHA_2nd_EDGE
		 * @retval	���óɹ��򷵻�1�����򷵻�0
		 */
		virtual int SetClockPhase(const CPHA phase)
		{
			STATIC_ASSERT(CPHA_1st_EDGE == 0 && CPHA_2nd_EDGE == 1,
				enumerate_value_is_not_campacted_with_this_configuration);
			PeripheralInfo::BaseAccess()->CTAR[0].CPHA = phase;
			return 1;
		}



		/*
		 * @brief	SPI��ȡʱ����λ
		 * @retval	phase ʱ����λö��
		 */
		virtual CPHA GetClockPhase() const
		{
			return CPHA(PeripheralInfo::BaseAccess()->CTAR[0].CPHA);
		}




		enum { FSYS = PeripheralInfo::ModuleClock::Frequency };



		/*
		 * @brief	SPI����������
		 * @param	baudrate ������ֵ��K60 SPI��������������²�����ֵ @BusCloclk == 90MHz
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
		 *				@arg 5,625,000 ~ 2747 ���п�ѡ����ֵ���������ж������õ������Զ����õ����Ƶ�
		 * @retval	�ɹ������򷵻�1�����򷵻�0
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
						// ��¼���ӽ������ʵ�����
						min_diff = difference;
						br = scalerIdx;
						pbr = prescalerIdx;

						// ����ƥ����
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
		 * @brief	SPI��ȡ������
		 * @retval	���ز�����ֵ����λbps
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
		 * @brief	SPI����λ����
		 * @param	datebits ����λ�����δ��������ݵ���Чλ��
		 * @retval	�ɹ������򷵻�1�����򷵻�0
		 * @notice	����λͨ��Ϊ7,8,16,32,64��
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
		 * @brief	K60 SPI��������������²�����ֵ��ȡ����λ
		 * @retval	���ص���ͨѶ�д������Ч����λ��
		 */
		virtual unsigned int GetDataBits() const
		{
			return PeripheralInfo::BaseAccess()->CTAR[0].FMSZ + 1;
		}




		/*
		 * @brief	SPIУ��ģʽ����
		 * @param	parity У��ģʽö�٣�����������ֵ֮һ
		 *				@arg PARITY_NONE
		 *				@arg PARITY_CRC
		 * @retval	�ɹ������򷵻�1�����򷵻�0
		 */
		virtual int SetParityMode(const PARITY parity)
		{
			// ��ǰ��δ֧��
			return 0;
		}



		/*
		 * @brief	SPI��ȡУ��ģʽ
		 * @retval	��ǰ��У��ģʽ
		 */
		virtual PARITY GetParityMode() const
		{
			return PARITY_NONE;
		}



		/*
		 * @brief	SPI����У���֣�һ������CRCУ��ģʽ�����һ��У���֣�
		 */
		virtual int SendParityWord()
		{
			return 0;
		}



		/*
		 * @brief	SPI��ȡ����״̬
		 * @ovride	�޴���ʱ����0��������ERT�еĴ���λ�����
		 */
		virtual int GetErrors() const
		{
			return 0;
		}




		/*
		 * @brief	SPI�����״���λ
		 * @param	firstbit �״���λö��
		 *				@arg MSB ���λ�ȴ�
		 *				@arg LSB ���λ�ȴ�
		 * @retval	�ɹ������򷵻�1�����򷵻�0
		 * @ovride	��������ͨѶ����֧��ĳЩģʽ�򷵻�0
		 */
		virtual int SetFirstBit(const FSB firstbit)
		{
			STATIC_ASSERT(MSB == 0 && LSB == 1,
				enumerate_value_is_not_campacted_with_this_configuration);
			PeripheralInfo::BaseAccess()->CTAR[0].LSBFE = firstbit;
			return 1;
		}



		/*
		 * @brief	SPI��ȡ�״���λģʽ
		 * @retval	��ǰ�״���λģʽ
		 * @ovride	��������һ���״���λģʽ��������ͨѶ���ܿ���ʵ�ִ˷���
		 */
		virtual FSB GetFirstBit() const
		{
			STATIC_ASSERT(MSB == 0 && LSB == 1,
				enumerate_value_is_not_campacted_with_this_configuration);
			return FSB(PeripheralInfo::BaseAccess()->CTAR[0].LSBFE);
		}



		/*
		 * @brief	SPI����ģʽ����
		 * @param	mode ģʽö��
		 *				@arg MASTER ����ͨѶ������ģʽ
		 *				@arg SLAVE ����ͨѶ���ӻ�ģʽ
		 * @retval	�ɹ������򷵻�1�����򷵻�0
		 */
		virtual int SetMode(const MSM mode)
		{
			STATIC_ASSERT(MASTER == 1 && SLAVE == 0,
				enumerate_value_is_not_campacted_with_this_configuration);
			PeripheralInfo::BaseAccess()->MCR.MSTR = mode;
			return 1;
		}



		/*
		 * @brief	SPI����ģʽ����
		 * @retval	��ģʽ���ģʽö��ֵ
		 */
		virtual MSM GetMode() const
		{
			STATIC_ASSERT(MASTER == 1 && SLAVE == 0,
				enumerate_value_is_not_campacted_with_this_configuration);
			return MSM(PeripheralInfo::BaseAccess()->MCR.MSTR);
		}





		/*
		 * @brief	SPI���ô��乤��ģʽ
		 * @param	transmission_mode ���乤��ģʽö��
		 *				@arg FullDuplex ȫ˫��
		 *				@arg HalfDuplex_Tx/HalfDuplex_Rx ��˫����/��˫����
		 *				@arg Simplex_Tx/Simplex_Rx ������/������
		 * @retval	�ɹ������򷵻�1�����򷵻�0
		 */
		virtual int SetTransmissionMode(const TRM transmission_mode)
		{
			return 0;
		}



		/*
		 * @brief	SPI��ȡ���乤��ģʽ
		 * @retval	ȫ˫��
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
		 * @brief	SPI�շ��ӿڷ���
		 * @param	out ���������ݵĵ�ַ
		 *			in ���յ����ݴ�����ĵ�ַ
		 *			_TxBuf ���������ݻ�����
		 *			_RxBuf ���ջ���������ʹ�������з��䣩
		 *			bufferSize ���ֽ��շ��ķ��ͻ�������С
		 *			����֧֡���ֽڡ����֡��� �������ݣ���ͨѶ�������ʵ�����ʵ��
		 *			�շ�ǰ��������ú�����λ
		 * @notice	��������ͬ��ͨѶ�������û�յ����ݿɷ���0
		 *			���ֽ��շ������е��η���һ���ֽڻ���һ��������������������ݵ�ǰ���þ���
		 * @retval	���ؽ��յ�������/������
		 */
		virtual UINT8 SynchronouslyTranceive(const UINT8 out)
		{
			PUSHR_Type push = { 0 };
			if (GetMode() == MASTER)
			{
				push.Cfg.CONT = 0;				// ����������
				push.Cfg.EOQ = 1;				// ���һ������
				push.Cfg.CTCNT = 1;				// ��մ��������
				push.Cfg.PCS = _BV(0);			// Ĭ��PCS0��ΪƬѡ
			}
			push.Cfg.TXDATA = out;				// д���������
			
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
				push.Cfg.CONT = 0;				// ����������
				push.Cfg.EOQ = 1;				// ���һ������
				push.Cfg.CTCNT = 1;				// ��մ��������
				push.Cfg.PCS = _BV(0);			// Ĭ��PCS0��ΪƬѡ
			}
			push.Cfg.TXDATA = out;				// д���������
			
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
			// �˷���һ��ֻ������ģʽ��ʹ��
			PUSHR_Type push = { 0 };
			push.Cfg.CONT = 1;					// ��������
			push.Cfg.EOQ = 0;					// �����һ������
			push.Cfg.CTCNT = 0;					// ����մ��������
			push.Cfg.PCS = _BV(0);				// Ĭ��PCS0��ΪƬѡ

			if (GetDataBits() <= 8)
			{
				const UINT8* TxBuf = static_cast<const UINT8*>(_TxBuf);
				UINT8* RxBuf = static_cast<UINT8*>(_RxBuf);
				while (bufferSize > 1)
				{
					--bufferSize;

					// װ������
					push.Cfg.TXDATA = *TxBuf++;
					
					// ѹ��TX_FIFO������
					while (!ReadyToSend());
					PeripheralInfo::BaseAccess()->PUSHR_W = push.PUSH_Value;
					
					// ����RX_FIFO������
					Receive(*RxBuf++);
				}

				// �������һ������
				push.Cfg.CONT = 0;
				push.Cfg.EOQ = 1;

				// ���ڴ�����ɺ���մ���������������Ҫ��ȡ��ֵ���ڴ�֮ǰ��������
				push.Cfg.CTCNT = 1;

				// ѹ�����һ�����ݵ�TX_FIFO����ȡ���һ�����յ�������
				push.Cfg.TXDATA = *TxBuf++;
				while (!ReadyToSend());
				PeripheralInfo::BaseAccess()->PUSHR_W = push.PUSH_Value;
				Receive(*RxBuf++);
			}
			else
			{
				// �˴�ע��ͬ��
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
		 * @brief	ͨѶ�����ͽӿڷ���
		 * @param	out ���������ݵĵ�ַ
		 *			����֧֡���ֽڡ����֡��� �������ݣ���ͨѶ�������ʵ�����ʵ��
		 *			����ǰ��������ú�����λ
		 * @notice	����������ʵ��ʱ�����֧���뷵��false
		 * @ovride	������������Ҫʵ��UINT8�������͵ķ��ͷ�����
		 *			��������Ĭ�Ϸָ�ɶ���ֽ�������ͣ����ֽ���ǰ��С��ģʽ��
		 * @retval	���ͳɹ�����true�����򷵻�false
		 */
		virtual bool Send(const UINT8 out)
		{
			PUSHR_Type push = { 0 };
			if (GetMode() == MASTER)
			{
				push.Cfg.CONT = 0;				// ����������
				push.Cfg.EOQ = 1;				// ���һ������
				push.Cfg.CTCNT = 1;				// ��մ��������
				push.Cfg.PCS = _BV(0);			// Ĭ��PCS0��ΪƬѡ
			}
			push.Cfg.TXDATA = out;				// д���������
			
			while (!ReadyToSend());
			PeripheralInfo::BaseAccess()->PUSHR_W = push.PUSH_Value;
			
			// һ��Ҫ�ȴ����ݽ�����ɲ�д��RFDF��ſ�������ɷ���
			while (!DataAvailable());
			PeripheralInfo::BaseAccess()->SR.RFDF = 1;
			return 1;
		}
		virtual bool Send(const UINT16 out)
		{
			PUSHR_Type push = { 0 };
			if (GetMode() == MASTER)
			{
				push.Cfg.CONT = 0;				// ����������
				push.Cfg.EOQ = 1;				// ���һ������
				push.Cfg.CTCNT = 1;				// ��մ��������
				push.Cfg.PCS = _BV(0);			// Ĭ��PCS0��ΪƬѡ
			}
			push.Cfg.TXDATA = out;				// д���������
			
			while (!ReadyToSend());
			PeripheralInfo::BaseAccess()->PUSHR_W = push.PUSH_Value;
			
			// һ��Ҫ�ȴ����ݽ�����ɲ�д��RFDF��ſ�������ɷ���
			while (!DataAvailable());
			PeripheralInfo::BaseAccess()->SR.RFDF = 1;
			return 1;
		}
		virtual bool Send(const void* _buffer, size_type bufferSize)
		{
			// �˷���һ��ֻ������ģʽ��ʹ��
			PUSHR_Type push = { 0 };
			push.Cfg.CONT = 1;					// ��������
			push.Cfg.EOQ = 0;					// �����һ������
			push.Cfg.CTCNT = 0;					// ����մ��������
			push.Cfg.PCS = _BV(0);				// Ĭ��PCS0��ΪƬѡ

			if (GetDataBits() <= 8)
			{
				const UINT8* buf = static_cast<const UINT8*>(_buffer);
				while (bufferSize > 1)
				{
					--bufferSize;
					while (!ReadyToSend());

					// װ������
					push.Cfg.TXDATA = *buf++;
					
					// ѹ��FIFO������
					PeripheralInfo::BaseAccess()->PUSHR_W = push.PUSH_Value;
				}

				// �������һ������
				push.Cfg.EOQ = 1;

				// ���ڴ�����ɺ���մ���������������Ҫ��ȡ��ֵ���ڴ�֮ǰ��������
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
		 * @brief	SPI���ͻ������ж�
		 * @retval	���ͻ������򷵻�true�����򷵻�false
		 * @ovride	������ͨѶ�����޷��ж��Ƿ���������ʵ���������true��false
		 *			����֧�ִ˹����뷵��true
		 */
		virtual bool ReadyToSend() const
		{
			return PeripheralInfo::BaseAccess()->SR.TFFF == 1;
		}



		/*
		 * @brief	SPI���ͻ���������ж�
		 * @retval	���ͻ���������򷵻�true�����򷵻�false
		 */
		virtual bool SendComplete() const
		{
			return PeripheralInfo::BaseAccess()->SR.TCF == 1;
		}



		/*
		 * @brief	SPI���ͻ�ʹ�ܿ��ƣ�����/�رս��ջ������ʹ��״̬��
		 * @retval	�ɹ�������ر��򷵻�1�����򷵻�0
		 * @notcie	K60 SPI�շ���û�ж������ؿ��ƣ�ģ��ʹ��ʱ���ǿ���
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
		 * @brief	ͨѶ�����սӿڷ���
		 * @param	in ���յ����ݴ�����ĵ�ַ
		 *			����֧֡���ֽڡ����֡��� �������ݣ���ͨѶ�������ʵ�����ʵ��
		 *			����ǰ��������ú�����λ
		 * @notice	����������ʵ��ʱ�����֧���뷵��false
		 * @ovride	������������Ҫʵ��UINT8�������͵Ľ��շ�����
		 *			��������Ĭ�Ϸָ�ɶ���ֽ�������գ����ֽ���ǰ��С��ģʽ��
		 *			���������֧�ֱ�İ��ֻ��ֵĽ��գ���override�÷���
		 * @retval	���ճɹ�����true�����򷵻�false
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
		 * @brief	SPI���ջ����յ���Ч�����ж�
		 * @retval	���ջ����յ���Ч�����򷵻�true�����򷵻�false
		 */
		virtual bool DataAvailable() const
		{
			return PeripheralInfo::BaseAccess()->SR.RFDF == 1;
		}



		/*
		 * @brief	SPI���ջ�������������ж�
		 * @retval	���ջ�������������򷵻�true�����򷵻�false
		 * @notice	�������Ѿ�ȫ��������Ľ��ջ������ж����ڴ������Ϊ�������
		 */
		virtual bool ReceiveCompleted() const
		{
			return PeripheralInfo::BaseAccess()->SR.RFDF == 0;
		}



		/*
		 * @brief	SPI���ջ�ʹ�ܿ��ƣ�����/�رս��ջ������ʹ��״̬��
		 *			K60 SPI�����շ������ܶ������أ���ģ�鿪�������Ǵ���ʹ��״̬
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
		 * @brief	���迪��/�رսӿڷ���
		 * @retval	�������޷�ʵ����ִ��Ч�����ʱ�뷵��0�����򷵻�1
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
		 * @brief	�¼�֪ͨ���ؿ���
		 * @param	evt ��Ҫ������ر�֪ͨ���¼�����
		 * @retval	�ɹ�ʹ��/��ֹĳ���¼�֪ͨ�򷵻�1�����򷵻�0
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
		 * @brief	����¼��������������¼������ĳ���¼�
		 * @param	evt ������¼�
		 *			FnPtr �������¼�ʱ�Ĵ�����������������ʵ��ʱת�������������
		 * @retval	�ɹ�ע��ĳ���¼��������򷵻�1�����򷵻�0
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
		 * @brief	�жϻص�������
		 */
		adxM3::Function< void(void*, UINT16) > dataAvailableISR;
		adxM3::Function< void(void*) > sendCompletedISR;
		adxM3::Function< void(void*) > readyToSendISR;



	protected:

		typedef typename PeripheralInfo::SCK SCKSignal;
		typedef typename PeripheralInfo::SOUT SOUTSignal;
		typedef typename PeripheralInfo::SIN SINSignal;
		typedef typename PeripheralInfo::PCS0_SS SelectSignal;
		
		// ���������ź�����ʵ����������ʵ�������Զ�����Portģ��ʱ��
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
