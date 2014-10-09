
#ifndef _COMTEXTSTREAM_H_
#define	_COMTEXTSTREAM_H_


#include "IOStream.h"
#include "../../common/auxiliary.h"
#include "StreamManipulator.h"

namespace adxM3
{
	



	/*
	 * @brief	ͨѶ���ַ����������
	 *			������ҪΪ��ʵ�ֻ����������͵��ַ������������ʽ
	 *			���ɰ���������Ƶķ�ʽ�����ͬʱҲ�ṩc��׼�ַ���
	 *			���������֧��
	 * @notice	������Ĳ������ȡ�������������ֵ�ͶԴ�����д�������ж�ȡ
	 *			�����Ҫ���ַ������������ʹ��IOStream<_io_DeviceT>::Read(UINT8&)��
	 *			IOStream<_io_DeviceT>::Write(const UINT8)����
	 */

	template< typename _io_DeviceT >
	class TextStream : public IOStream<_io_DeviceT>
	{
	public:

		using IOStream<_io_DeviceT>::Write;
		using IOStream<_io_DeviceT>::Read;

		using IOStream<_io_DeviceT>::device;


		typedef typename IOStream<_io_DeviceT>::size_type size_type;



		/*
		 * @brief	�ַ������������Ͷ���
		 *			�˲�����Ϊ�ַ���ר�ò�����
		 */
		typedef void (*TextStreamManipulator)(TextStream& stream);


		TextStream(_io_DeviceT& _device)
			: IOStream<_io_DeviceT>(_device), radix(10), byChar(true)
		{
		}

		virtual ~TextStream() {}



		/*
		 * @brief	�ַ������룬�������з�\r\n�����ַ���������\0����ֹ
		 * @param	buffer �������ݴ�Ż�������ַ�����ֽ����δ��
		 *			bufferSize ϣ�����������ַ���-1����Ϊ����Ԥ��һ���ֽڴ�Ž�����\0
		 *			actuallyLength ʵ�ʶ�����ַ�����������������\0
		 */
		virtual bool Read(void* buffer, size_type bufferSize, size_type& actuallyLength)
		{
			//device.DisableEventNotify(Communicator::DATE_AVAILABLE);

			UINT8* _buffer = static_cast<UINT8*>(buffer);

			actuallyLength = 0;
			while (actuallyLength < bufferSize - 1)
			{
				IOStream<_io_DeviceT>::Read(*_buffer);	// ���ֽڶ��룬�����κ�ת��
				
				// ����ַ���\n���Ҳ��ǵ�һ���ַ���������ַ���ǰһ���ַ���\r
				if (*_buffer == '\n' && actuallyLength > 0 && *(_buffer - 1) == '\r')
				{
					// ��������\r\nʱ����\r��д�������\0������ѭ����д����������ȡ�Ļ�������Ҫ����д������������ǣ�
					// ���_buffer��buffer��ͬ��ʾ��δ�յ���Ч�ַ���
					// ��Ӧ������ȡ
					--actuallyLength;
					--_buffer;
					if (actuallyLength == 0)
					{
						continue;
					}
					else
					{
						break;
					}
				}
				else if (*_buffer == '\0')
				{
					// ������յ��ľ���\0�������ֱ�ӷ���
					return true;
				}
				else
				{
					// ʲôҲ����
				}
				++actuallyLength;
				++_buffer;
			}

			// �䵽��������ֽ�ʱ���������һ���ֽ�ֻ�ܷŽ����������Ե������ڶ����ֽ���\rʱ������ֽ���Ч
			// ����������û�ϣ�����ַ���ֻ�ǻ�����Ϊ����������
			if (*(_buffer - 1) == '\r')
			{
				--actuallyLength;
				--_buffer;
			}

			// ���۴�����ѭ���ĸ���֧����ʱ�����һλ��Ӧ��д\0
			// ʹ�û���������ʱ��_buffer��ǰָ�򻺳������һ�ֽ�Ӧд��\0
			*_buffer = '\0';

			//device.EnableEventNotify(Communicator::DATE_AVAILABLE);
			return true;
		}
		


		/*
		 * @brief	д������������������أ�
		 * @param	out ������ֽڡ����֡��֡�˫�֣�������ʲô���ͣ�Ĭ�϶�������ֵ�������
		 * @notice	�������еĲ������ȡ����������ձ�׼��ֵ����ת�����ַ������
		 *			e.g. ����100 д�뵽����Ϊ"100"��Ĭ��10���ƣ����ò������޸ģ�
		 *			INT8��UINT8��ʹ��by_char��by_ascii�����ӿ�������ַ�����asciiֵ
		 *			char���Ͷ��������һ������ֻ����ַ��ͣ���asciiֵ
		 * @retval	�����������÷��㼶��
		 */
		virtual TextStream& operator<<(const char out)
		{
			// ����char��������Ҫ�����һ����������
			IOStream<_io_DeviceT>::operator<<(out);
			return *this;
		}
		virtual TextStream& operator<<(const INT8 out)
		{
			if (byChar)
			{
				IOStream<_io_DeviceT>::operator<<(out);
			}
			else
			{
				char buf[10];
				operator<<(Convertor.itoa(out, buf, radix));
			}
			return *this;
		}
		virtual TextStream& operator<<(const INT16 out)
		{
			char buf[18];
			return operator<<(Convertor.itoa(out, buf, radix));
		}
		virtual TextStream& operator<<(const INT32 out)
		{
			char buf[34];
			return operator<<(Convertor.itoa(out, buf, radix));
		}
		virtual TextStream& operator<<(const UINT8 out)
		{
			if (byChar)
			{
				IOStream<_io_DeviceT>::operator<<(out);
			}
			else
			{
				char buf[10];
				operator<<(Convertor.itoa(out, buf, radix));
			}
			return *this;		
		}
		virtual TextStream& operator<<(const UINT16 out)
		{
			char buf[18];
			return operator<<(Convertor.itoa(out, buf, radix));
		}
		virtual TextStream& operator<<(const UINT32 out)
		{
			char buf[34];
			return operator<<(Convertor.itoa(out, buf, radix));
		}
		virtual TextStream& operator<<(const char* outstr)
		{
			IOStream<_io_DeviceT>::operator<<(outstr);
			return *this;
		}



		/*
		 * @brief	������д���������ձ�׼С�����ʾ���ǿ�ѧ��������
		 * @param	out �������������ַ�д�뵽����
		 * @notice	�������������ֲ��˹��󣬳���32λ������Χ�п��ܱ�ʾ����
		 * @retval	�����������÷��㼶��
		 */
		virtual TextStream& operator<<(const float out)
		{
			char buf[50];
			return operator<<(Convertor.ftoa(out, buf, radix));
		}
		virtual TextStream& operator<<(const double out)
		{
			char buf[60];
			return operator<<(Convertor.dtoa(out, buf, radix));
		}
		


		/*
		 * @brief	�����Ӳ��뵽����
		 * @param	manip �����Ӻ���
		 * @retval	�����������÷��㼶��
		 */
		virtual TextStream& operator<<(TextStreamManipulator manip)
		{
			manip(*this);
			return *this;
		}
		virtual TextStream& operator>>(TextStreamManipulator manip)
		{
			manip(*this);
			return *this;
		}



		/*
		 * @brief	�����ж�ȡ���ݣ����������ȴ���ȡ��ɣ���ȡ��������أ�
		 * @notice	�������еĲ������ȡ����������ձ�׼��ֵ����ת�����ַ������
		 *			e.g. �����ж���"100" ��ת��Ϊ����100��Ĭ��10���ƣ����ò������޸ģ�
		 */
		virtual TextStream& operator>>(INT8& in)
		{
			size_type actuallyLength;
			char buf[10];
			Read(buf, 10, actuallyLength);
			in = Convertor.atoi(buf, radix);
			return *this;
		}
		virtual TextStream& operator>>(INT16& in)
		{
			size_type actuallyLength;
			char buf[18];
			Read(buf, 18, actuallyLength);
			in = Convertor.atoi(buf, radix);
			return *this;
		}
		virtual TextStream& operator>>(INT32& in)
		{
			size_type actuallyLength;
			char buf[34];
			Read(buf, 34, actuallyLength);
			in = Convertor.atoi(buf, radix);
			return *this;
		}
		virtual TextStream& operator>>(UINT8& in)
		{
			size_type actuallyLength;
			char buf[10];
			Read(buf, 10, actuallyLength);
			in = Convertor.atoi(buf, radix);
			return *this;
		}
		virtual TextStream& operator>>(UINT16& in)
		{
			size_type actuallyLength;
			char buf[18];
			Read(buf, 18, actuallyLength);
			in = Convertor.atoi(buf, radix);
			return *this;
		}
		virtual TextStream& operator>>(UINT32& in)
		{
			size_type actuallyLength;
			char buf[34];
			Read(buf, 34, actuallyLength);
			in = Convertor.atoi(buf, radix);
			return *this;
		}
		
		

		/*
		 * @brief	�ַ����������������������з�\r\n���߽�����\0��ֹ��ȡ������
		 * @notice	���û������㹻�Ļ��������÷����޷���ȡ�û������instr�Ļ�������С
		 *			�����Ҫʹ�ð�ȫ������ģʽ����ʹ��Read(void*, size_type, size_type&)����
		 */
		virtual TextStream& operator>>(char* instr)
		{
			//device.DisableEventNotify(Communicator::DATE_AVAILABLE);

			char* p = instr;
			BYTE temp = 0;
	
			// ����status��
			// status = 0 ��ʾ�������գ�status = 1��ʾ���յ���\r��status=2 ��ʾ���յ���\r\n,
			// ��ô�ͽ���������
			BYTE status = 0;
	
			do 
			{
				IOStream<_io_DeviceT>::Read(temp);
				if (temp == '\r') 
				{
					status = 1;
					*p++ = temp;
				} 
				else if (temp == '\n' && status == 1) 
				{
					status = 2;
					*(--p) = '\0';
				} 
				else if (temp == '\0')				// �����˽������Ϳ��Բ��ü���������ȥ��
				{
					*p = '\0';
					break;
				}
				else 
				{
					status = 0;
					*p++ = temp;
				}
			} while (status != 2 || p == instr);	//ֻ���յ��س���û�н��յ��κ����ݣ�ʱ�������ȴ�����

			//device.EnableEventNotify(Communicator::DATE_AVAILABLE);
			return *this;
		}



		/*
		 * @brief	�������������������������з�\r\n���߽�����\0��ֹ��ȡ������
		 *			�����ж�ȡ����ת����IEEE��׼������
		 * @notice	������Ĭ����Ϊ100�ֽڣ�����ĸ�������Ҫ�����˳���
		 */
		virtual TextStream& operator>>(float& in)
		{
			size_type actuallyLength;
			char buf[100];
			Read(buf, 97, actuallyLength);
			in = Convertor.atof(buf, radix);
			return *this;
		}
		virtual TextStream& operator>>(double& in)
		{
			size_type actuallyLength;
			char buf[100];
			Read(buf, 97, actuallyLength);
			in = Convertor.atod(buf, radix);
			return *this;
		}


	public:
		UINT8 radix;			// ��ֵ�Ļ�
		bool byChar;			// char�����°��ַ�����ascii����Ŀ��ƣ�trueʱ���ַ����

	};



	/*
	 * @brief	�ַ��������� �����н�������������
	 * @notice	ͬʱ��ָ�Ĭ���ַ����Ļ���
	 */
	template< typename _io_DeviceT >
	inline void endl(TextStream<_io_DeviceT>& stream)
	{
		stream.Write('\r');
		stream.Write('\n');
		stream.radix = 10;
	}



	/*
	 * @brief	�ַ��������� ת����N�������������
	 * @notice	�����������Ч
	 */
	template< typename _io_DeviceT >
	inline void hex(TextStream<_io_DeviceT>& stream)
	{
		stream.radix = 16;
	}
	template< typename _io_DeviceT >
	inline void dec(TextStream<_io_DeviceT>& stream)
	{
		stream.radix = 10;
	}
	template< typename _io_DeviceT >
	inline void oct(TextStream<_io_DeviceT>& stream)
	{
		stream.radix = 8;
	}
	template< typename _io_DeviceT >
	inline void bin(TextStream<_io_DeviceT>& stream)
	{
		stream.radix = 2;
	}



	/*
	 * @brief	�ַ��������� ��ˢ������������ָ�Ĭ���ַ�������
	 */
	template< typename _io_DeviceT >
	inline void flush(TextStream<_io_DeviceT>& stream)
	{
		stream.Flush();
		stream.radix = 10;
	}



	/*
	 * @brief	����д��������Ϊchar��INT8��ʱ���ַ����ǰ�asciiֵ���
	 */
	template< typename _io_DeviceT >
	inline void by_char(TextStream<_io_DeviceT>& stream)
	{
		stream.byChar = true;
	}
	template< typename _io_DeviceT >
	inline void by_ascii(TextStream<_io_DeviceT>& stream)
	{
		stream.byChar = false;
	}



}
#endif  /*_COMTEXTSTREAM_H_*/
