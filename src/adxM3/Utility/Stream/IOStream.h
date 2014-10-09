
#ifndef _COMIOSTREAM_H_
#define _COMIOSTREAM_H_

#include "OutputStream.h"
#include "InputStream.h"



namespace adxM3
{



	template< typename _io_DeviceT >
	class IOStream
		: public OutputStream
		, public InputStream
	{
	public:

		using OutputStream::operator <<;
		using InputStream::operator >>;


		IOStream(_io_DeviceT& _device)
			: device(_device)
		{
		}

		virtual ~IOStream() {}



		/*
		 * @brief	д����
		 * @param	out ������ֽڡ����֡��ֻ��߻������׵�ַ���з������������޷�����������
					bufferSize ��������С
		 * @notice	����಻֧�ָ÷�����ʵ�ָ÷���ʱֱ�ӷ���true��
		 * @retval	true��ʾ�ɹ���false��ʾʧ��
		 */
		virtual bool Write(const UINT8 out)
		{
			return device.Send(out);
		}
		virtual bool Write(const void* buffer, size_type bufferSize)
		{
			return device.Send(buffer, bufferSize);
		}



		/*
		 * @brief	�����첽�����д���ַ��������غ��������ز����������
		 * @param	outstr �������ַ���
		 * @notice	�÷�����ȴ����һ���ֽڷ�����ɣ�ֱ������'\0'��
		 * @retval	���ظ����������ã����㼶��
		 */
		virtual IOStream& operator<<(const char* outstr)
		{
			// ʹ�ô��л���ķ���ģ��
			if (_io_DeviceT::UseBufferedTransfer)
			{
				unsigned int len = BasicString::strlen(outstr);
				Write(outstr, len);
			}
			else
			{
				while (*outstr)
				{
					Write(*(const UINT8*)outstr);
					++outstr;
				}
			
				// �ȴ����һ���ֽڷ������
				while (!device.SendComplete());
			}
			return *this;
		}



		/*
		 * @brief	��ȡ��
		 * @param	in ��ȡ�����ݴ����
					size Ԥ�ڶ�ȡ���ֽ���
					actuallyLength ʵ�ʶ�ȡ���ֽ���
		 * @retval	�ɹ���ȡ�򷵻�true�����򷵻�false
		 */
		virtual bool Read(UINT8& in)
		{
			return device.Receive(in);
		}
		virtual bool Read(void* in, size_type size, size_type& actuallyLength)
		{
			actuallyLength = size;
			return device.Receive(in, size);
		}



		/*
		 * @brief	�����첽��������ȡ�ַ��������غ�����������ȡ�������
		 * @param	instr ������ַ�����Ż�����ָ��
		 * @notice	����'\0'��'\r\n'�򷵻أ�instr��\0��β 
		 * @retval	���ظ����������ã����㼶��
		 */
		virtual IOStream& operator>>(char* instr)
		{
			// �Ժ�ʵ���������
			return *this;
		}



		/*
		 * @brief	�ǻ���ͨ���������֧�ֹر����ˢ��������ʹ��
		 * @retval	true
		 */
		virtual bool Flush()
		{
			return true;
		}
		virtual bool Close()
		{
			return true;
		}



		/*
		 * @brief	д��������ж�
		 * @retval	true��ʾ�������д�룬false��ʾδ���
		 */
		virtual bool WriteCompleted()
		{
			return device.SendComplete();
		}



		/*
		 * @brief	�ж����Ƿ��ȡ���
		 * @retval	���ղ��Ҷ�ȡ��ɷ���true�����򷵻�false
		 */
		virtual bool ReadCompleted()
		{
			return device.ReceiveCompleted();
		}



		/*
		 * @brief	��ǰ����������n�ֽ�
		 * @param	size Ҫ�������ֽ���
		 * @retval	�ɹ������򷵻�true�����򷵻�false
		 */
		virtual bool Skip(size_type size)
		{
			return false;
		}
		
		
		
		/*
		 * @brief	��ǰͨѶ�������Ƿ���Ч���ɶ�ȡ
		 * @retval	������Ч����true�����򷵻�false
		 */
		virtual bool Available()
		{
			return device.DataAvailable();
		}



		/*
		 * @brief	������������ʼ��
		 * @notice	ͨѶ��Ϊ�ǻ���������֧�ָ÷���������false
		 * @retval	false
		 */
		virtual bool Reset()
		{
			return false;
		}



	protected:

		_io_DeviceT& device;

	};
}
#endif  /*_COMIOSTREAM_H_*/
