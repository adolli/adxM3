
#ifndef _COMMUNICATOR_H_
#define _COMMUNICATOR_H_

#include "../../common/defs.h"
#include "../Event/EventObject.h"
#include "../GPIO/GeneralPurposeInputOutputPin.h"
#include "../PeripheralBahaviour/PeripheralOnOff.h"

namespace adxM3
{








	/*
	 * @brief	ͨѶ��������ջ�����
	 *			ÿ�����ջ�����һ����������
	 *			�������˽��սӿڷ���
	 */
	class Receiver
	{
	public:

		typedef unsigned int size_type;



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
		 * @brief	ͨѶ�����ջ����յ���Ч�����ж�
		 * @retval	���ջ����յ���Ч�����򷵻�true�����򷵻�false
		 * @ovride	������ͨѶ�����ʵ���ǽӿڣ������޷���֤������ͨѶ����
		 */
		virtual bool DataAvailable() const = 0;



		/*
		 * @brief	ͨѶ�����ջ�������������ж�
		 * @retval	���ջ�������������򷵻�true�����򷵻�false
		 * @notice	�������Ѿ�ȫ��������Ľ��ջ������ж����ڴ������Ϊ�������
		 */
		virtual bool ReceiveCompleted() const = 0;



		/*
		 * @brief	ͨѶ�����ջ�ʹ�ܿ��ƣ�����/�رս��ջ������ʹ��״̬��
		 * @retval	�ɹ�������ر��򷵻�1�����򷵻�0
		 * @ovride	��������������д�÷��������޷�ʵ�ֽ��ջ�������ر��뷵��0
		 * @notcie	����ȫ˫����ģʽ��EnableReceiveʱ�벻Ҫʹ��EnableTransmitter����
		 *			DisableReceiverʱ�벻Ҫʹ��DisableTransmitter����
		 */
		virtual int EnableReceiver() = 0;
		virtual int DisableReceiver() = 0;
		virtual bool IsReceiverEnabled() const = 0;


	};




	/*
	 * @brief	ͨѶ�����跢�ͻ�����
	 *			ÿ�����ͻ�����һ����������
	 *			�������˷��ͽӿڷ���
	 */
	class Transmitter
	{
	public:

		typedef unsigned int size_type;



		/*
		 * @brief	ͨѶ�����ͽӿڷ���
		 * @param	out ���������ݵĵ�ַ
		 *			����֧֡���ֽڡ����֡��� �������ݣ���ͨѶ�������ʵ�����ʵ��
		 *			����ǰ��������ú�����λ
		 * @notice	����������ʵ��ʱ�����֧���뷵��false
		 * @ovride	������������Ҫʵ��UINT8�������͵ķ��ͷ�����
		 *			��������Ĭ�Ϸָ�ɶ���ֽ�������ͣ����ֽ���ǰ��С��ģʽ��
		 *			���������֧�ֱ�İ��ֻ��ֵķ��ͣ���override�÷���
		 * @retval	���ͳɹ�����true�����򷵻�false
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
		 * @brief	ͨѶ�����ͻ������ж�
		 * @retval	���ͻ������򷵻�true�����򷵻�false
		 * @ovride	������ͨѶ�����޷��ж��Ƿ���������ʵ���������true��false
		 *			����֧�ִ˹����뷵��true
		 */
		virtual bool ReadyToSend() const = 0;



		/*
		 * @brief	ͨѶ�����ͻ���������ж�
		 * @retval	���ͻ���������򷵻�true�����򷵻�false
		 * @ovride	������ͨѶ�����޷��ж��Ƿ�����������ʵ���������true��false
		 *			����֧�ִ˹����뷵��true
		 */
		virtual bool SendComplete() const = 0;



		/*
		 * @brief	ͨѶ�����ͻ�ʹ�ܿ��ƣ�����/�رս��ջ������ʹ��״̬��
		 * @retval	�ɹ�������ر��򷵻�1�����򷵻�0
		 * @ovride	��������������д�÷��������޷�ʵ�ַ��ͻ�������ر��뷵��0
		 * @notcie	��ȫ˫��ģʽ��EnableTransmitterʱ�벻Ҫʹ��EnableReceive����
		 *			DisableReceiverʱ�벻Ҫʹ��DisableTransmitter����
		 */
		virtual int EnableTransmitter() = 0;
		virtual int DisableTransmitter() = 0;
		virtual bool IsTransmitterEnabled() const = 0;



	};




	/*
	 * @brief	ͨѶ������Ļ��࣬ͨѶװ��Ҫʹ��ͨѶ����ʵ�ָýӿ�
	 *			�ýӿڰ��������շ��������շ�״̬�жϷ���
	 *			ͨѶͨѶ��һ��Ҳ���¼���������������Ҫʵ���¼����Ƶķ���
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
		 * @brief	�շ�ģʽѡ��
		 */
		enum IOM
		{
			Rx					= 0x01,
			Tx					= 0x02
		};




		/*
		 * @brief	ͨѶ���¼����Ͷ���
		 *			adxM3��֧�ֵ��첽ͨѶ�������¼����ڴ�
		 */
		enum EVT
		{
			READY_TO_SEND,									// ���ͻ�����
			DATE_AVAILABLE,									// ���յ�����Ч����
			SEND_COMPLETED,									// �������
			CLEAR_TO_SEND,									// CTS��־
			RECEIVE_BUFFER_OVERFLOW,						// ���ջ��������
			TRANSMIT_BUFFER_OVERFLOW,						// ���ͻ��������
			BUS_IDLE,										// ���߿���
			PARITY_ERROR,									// У�����
			FRAME_ERROR,									// ֡����
			EVT_COUNT
		};

		 

	protected:

		virtual ~Communicator() {}

	};




	
	/*
	 * @brief	������ͨѶ������
	 *			�ڸ����ж�������ͨѶ���������õĽӿڷ���
	 *			�����������úͲ�����ȡ
	 */
	class ConfigurableCommunicator
		: public Communicator
	{
	public:



		/*
		 * @brief	ͨѶ������������
		 * @param	baudrate ������ֵ������ʵ��������ò����ʣ���λbps
		 * @retval	�ɹ������򷵻�1�����򷵻�0
		 * @ovride	���������ʵ�ֲ����ʵ�����
		 *			��������ͨѶ��û�����������ʿ�����������������Ϊ����ƵĲ�����ֵ
		 */
		virtual int SetBaudRate(const unsigned int baudrate) = 0;
		


		/*
		 * @brief	ͨѶ����ȡ������
		 * @retval	���ز�����ֵ����λbps
		 * @ovride	�������಻֧�ָ÷����뷵��0
		 */
		virtual unsigned int GetBaudRate() const = 0;

		

		/*
		 * @brief	ͨѶ������λ����
		 * @param	datebits ����λ�����δ��������ݵ���Чλ��
		 * @retval	�ɹ������򷵻�1�����򷵻�0
		 * @notice	����λͨ��Ϊ7,8,16,32,64��
		 * @ovride	��������ͨѶ��û����������λ������������������Ϊ����Ƶ�����λֵ
		 */
		virtual int SetDataBits(const unsigned int databits) = 0;



		/*
		 * @brief	ͨѶ����ȡ����λ
		 * @retval	���ص���ͨѶ�д������Ч����λ��
		 * @ovride	�������಻֧�ָ÷����뷵��0
		 */
		virtual unsigned int GetDataBits() const = 0;



		/*
		 * @brief	У��ģʽѡ��
		 *			adxM3��֧�ֵĳ���У��ģʽ
		 */
		enum PARITY
		{
			PARITY_NONE,				// ��У��
			PARITY_EVEN,				// żУ��
			PARITY_ODD,					// ��У��
			PARITY_SPACE,				// �ո�У��
			PARITY_MARK,				// ��־У��
			PARITY_CRC					// ����ѭ��У��
		};



		/*
		 * @brief	ͨѶ��У��ģʽ����
		 * @param	parity У��ģʽö�٣�������PARITYö�ٵ��κ�һ��
		 *				@arg PARITY_NONE
		 *				@arg PARITY_EVEN
		 *				...
		 *				@arg PARITY_CRC
		 * @retval	�ɹ������򷵻�1�����򷵻�0
		 * @ovride	��������ͨѶ����֧��ĳЩģʽ�򷵻�0
		 */
		virtual int SetParityMode(const PARITY parity) = 0;



		/*
		 * @brief	ͨѶ����ȡУ��ģʽ
		 * @retval	��ǰ��У��ģʽ
		 * @ovride	��������ͨѶ����֧�ָ÷����򷵻�PARITY_NONE
		 */
		virtual PARITY GetParityMode() const = 0;



		/*
		 * @brief	ͨѶ������У���֣�һ������CRCУ��ģʽ�����һ��У���֣�
		 * @ovride	��������ͨѶ����֧�ָ÷����򷵻�0
		 */
		virtual int SendParityWord() = 0;



		enum ERT
		{
			ParityError		= 0x0001,
			Overflow		= 0x0002,
			Underflow		= 0x0004
		};



		/*
		 * @brief	ͨѶ����ȡ����״̬
		 * @ovride	�޴���ʱ����0��������ERT�еĴ���λ�����
		 */
		virtual int GetErrors() const = 0;




		/*
		 * @brief	�״���λѡ��
		 */
		enum FSB
		{
			MSB,						// Most Significant Bit
			LSB							// Least Significant Bit
		};



		/*
		 * @brief	ͨѶ�������״���λ
		 * @param	firstbit �״���λö��
		 *				@arg MSB ���λ�ȴ�
		 *				@arg LSB ���λ�ȴ�
		 * @retval	�ɹ������򷵻�1�����򷵻�0
		 * @ovride	��������ͨѶ����֧��ĳЩģʽ�򷵻�0
		 */
		virtual int SetFirstBit(const FSB firstbit) = 0;



		/*
		 * @brief	ͨѶ����ȡ�״���λģʽ
		 * @retval	��ǰ�״���λģʽ
		 * @ovride	��������һ���״���λģʽ��������ͨѶ���ܿ���ʵ�ִ˷���
		 */
		virtual FSB GetFirstBit() const = 0;




		/*
		 * @brief	ͨѶ��ģʽ����ѡ��
		 *			����������֮�ֵ�ͨѶ����ѡ��SUPERģʽ
		 */
		enum MSM
		{
			SLAVE,						// �ӻ�ģʽ
			MASTER,						// ����ģʽ
			SUPER,						// ����ģʽ���ȷ������ַǴӻ�
		};



		/*
		 * @brief	ͨѶ������ģʽ����
		 * @param	mode ģʽö��
		 *				@arg SUPER ��������ģʽ
		 *				@arg MASTER ����ͨѶ������ģʽ
		 *				@arg SLAVE ����ͨѶ���ӻ�ģʽ
		 * @retval	�ɹ������򷵻�1�����򷵻�0
		 */
		virtual int SetMode(const MSM mode) = 0;



		/*
		 * @brief	ͨѶ������ģʽ����
		 * @retval	����������ģʽ��ģʽ֮��ʱ����SUPER������ʵ���������
		 */
		virtual MSM GetMode() const = 0;



		/*
		 * @brief	���乤��ģʽѡ��
		 * @note	��˫���뵥������������
		 *			����ģʽ�·�����·��Զ�Ƿ�����·��������·��Զ�ǽ�����·
		 *			��˫��ģʽ��������շ�ʱ����ͬһ��·
		 */
		enum TRM
		{
			FullDuplex,					// ȫ˫��
			HalfDuplex_Tx,				// ��˫������
			HalfDuplex_Rx,				// ��˫������
			Simplex_Tx,					// ��������
			Simplex_Rx					// ��������
		};



		/*
		 * @brief	ͨѶ�����ô��乤��ģʽ
		 * @param	transmission_mode ���乤��ģʽö��
		 *				@arg FullDuplex ȫ˫��
		 *				@arg HalfDuplex_Tx/HalfDuplex_Rx ��˫����/��˫����
		 *				@arg Simplex_Tx/Simplex_Rx ������/������
		 * @retval	�ɹ������򷵻�1�����򷵻�0
		 * @ovride	��������ͨѶ����֧��ĳЩģʽ�򷵻�0
		 */
		virtual int SetTransmissionMode(const TRM transmission_mode) = 0;



		/*
		 * @brief	ͨѶ����ȡ���乤��ģʽ
		 * @retval	���乤��ģʽ
		 * @ovride	��������һ�����乤��ģʽ��������ͨѶ���ܿ���ʵ�ִ˷���
		 */
		virtual TRM GetTransmissionMode() const = 0;




	};


}

#endif  /*_COMMUNICATOR_H_*/
