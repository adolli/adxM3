
#ifndef _COMTEXTSTREAM_H_
#define	_COMTEXTSTREAM_H_


#include "IOStream.h"
#include "../../common/auxiliary.h"
#include "StreamManipulator.h"

namespace adxM3
{
	



	/*
	 * @brief	通讯器字符输入输出流
	 *			该流主要为了实现基本数据类型的字符型输入输出方式
	 *			并可按照任意进制的方式输出，同时也提供c标准字符串
	 *			的输入输出支持
	 * @notice	该流类的插入和吸取运算符均按照数值型对待参数写入或从流中读取
	 *			如果需要按字符输入输出，请使用IOStream<_io_DeviceT>::Read(UINT8&)、
	 *			IOStream<_io_DeviceT>::Write(const UINT8)方法
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
		 * @brief	字符流操纵子类型定义
		 *			此操纵子为字符流专用操纵子
		 */
		typedef void (*TextStreamManipulator)(TextStream& stream);


		TextStream(_io_DeviceT& _device)
			: IOStream<_io_DeviceT>(_device), radix(10), byChar(true)
		{
		}

		virtual ~TextStream() {}



		/*
		 * @brief	字符流读入，遇到换行符\r\n或者字符串结束符\0则终止
		 * @param	buffer 读入内容存放缓冲区地址，按字节依次存放
		 *			bufferSize 希望读入的最大字符数-1，因为至少预留一个字节存放结束符\0
		 *			actuallyLength 实际读入的字符数，不包括结束符\0
		 */
		virtual bool Read(void* buffer, size_type bufferSize, size_type& actuallyLength)
		{
			//device.DisableEventNotify(Communicator::DATE_AVAILABLE);

			UINT8* _buffer = static_cast<UINT8*>(buffer);

			actuallyLength = 0;
			while (actuallyLength < bufferSize - 1)
			{
				IOStream<_io_DeviceT>::Read(*_buffer);	// 按字节读入，不作任何转换
				
				// 这个字符是\n，且不是第一个字符，且这个字符的前一个字符是\r
				if (*_buffer == '\n' && actuallyLength > 0 && *(_buffer - 1) == '\r')
				{
					// 连续读入\r\n时，在\r处写入结束符\0（跳出循环后写，如果还需读取的话，不需要急着写，反正会给覆盖）
					// 如果_buffer和buffer相同表示尚未收到有效字符，
					// 仍应继续读取
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
					// 如果接收到的就是\0，则可以直接返回
					return true;
				}
				else
				{
					// 什么也不做
				}
				++actuallyLength;
				++_buffer;
			}

			// 输到最后两个字节时，由于最后一个字节只能放结束符，所以当倒数第二个字节是\r时，这个字节无效
			// 这个本不是用户希望的字符，只是换行作为输入结束标记
			if (*(_buffer - 1) == '\r')
			{
				--actuallyLength;
				--_buffer;
			}

			// 无论从上面循环哪个分支跳出时，最后一位都应该写\0
			// 使得缓冲区读满时，_buffer当前指向缓冲区最后一字节应写上\0
			*_buffer = '\0';

			//device.EnableEventNotify(Communicator::DATE_AVAILABLE);
			return true;
		}
		


		/*
		 * @brief	写入流（插入运算符重载）
		 * @param	out 输出的字节、半字、字、双字，不管是什么类型，默认都按照数值类型输出
		 * @notice	该流类中的插入和析取运算符均按照标准数值类型转换成字符流输出
		 *			e.g. 整数100 写入到流中为"100"（默认10进制，可用操纵子修改）
		 *			INT8或UINT8可使用by_char和by_ascii操纵子控制输出字符还是ascii值
		 *			char类型额外的增加一个函数只输出字符型，非ascii值
		 * @retval	返回自身引用方便级联
		 */
		virtual TextStream& operator<<(const char out)
		{
			// 对于char这种类型要额外加一个函数处理
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
		 * @brief	浮点数写入流（按照标准小数点表示，非科学计数法）
		 * @param	out 浮点数，按照字符写入到流中
		 * @notice	浮点数整数部分不宜过大，超过32位整数范围有可能表示不了
		 * @retval	返回自身引用方便级联
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
		 * @brief	操纵子插入到流中
		 * @param	manip 操纵子函子
		 * @retval	返回自身引用方便级联
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
		 * @brief	从流中读取数据，并且阻塞等待读取完成（析取运算符重载）
		 * @notice	该流类中的插入和析取运算符均按照标准数值类型转换成字符流输出
		 *			e.g. 从流中读入"100" 即转换为整数100（默认10进制，可用操纵子修改）
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
		 * @brief	字符串输入流方法，遇到换行符\r\n或者结束符\0终止读取并返回
		 * @notice	请用户分配足够的缓冲区，该方法无法获取用户分配给instr的缓冲区大小
		 *			如果需要使用安全的输入模式，请使用Read(void*, size_type, size_type&)方法
		 */
		virtual TextStream& operator>>(char* instr)
		{
			//device.DisableEventNotify(Communicator::DATE_AVAILABLE);

			char* p = instr;
			BYTE temp = 0;
	
			// 关于status，
			// status = 0 表示正常接收，status = 1表示接收到了\r，status=2 表示接收到了\r\n,
			// 那么就结束接收了
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
				else if (temp == '\0')				// 遇到了结束符就可以不用继续接收下去了
				{
					*p = '\0';
					break;
				}
				else 
				{
					status = 0;
					*p++ = temp;
				}
			} while (status != 2 || p == instr);	//只接收到回车（没有接收到任何数据）时，继续等待接收

			//device.EnableEventNotify(Communicator::DATE_AVAILABLE);
			return *this;
		}



		/*
		 * @brief	浮点数输入流方法，遇到换行符\r\n或者结束符\0终止读取并返回
		 *			从流中读取并按转换到IEEE标准浮点数
		 * @notice	缓冲区默认设为100字节，输入的浮点数不要超过此长度
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
		UINT8 radix;			// 数值的基
		bool byChar;			// char类型下按字符还是ascii输出的控制，true时按字符输出

	};



	/*
	 * @brief	字符流操纵子 插入行结束符，即换行
	 * @notice	同时会恢复默认字符流的基数
	 */
	template< typename _io_DeviceT >
	inline void endl(TextStream<_io_DeviceT>& stream)
	{
		stream.Write('\r');
		stream.Write('\n');
		stream.radix = 10;
	}



	/*
	 * @brief	字符流操纵子 转换到N进制输入输出流
	 * @notice	输入输出都有效
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
	 * @brief	字符流操纵子 冲刷输出缓冲区并恢复默认字符流基数
	 */
	template< typename _io_DeviceT >
	inline void flush(TextStream<_io_DeviceT>& stream)
	{
		stream.Flush();
		stream.radix = 10;
	}



	/*
	 * @brief	控制写入流类型为char（INT8）时按字符还是按ascii值输出
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
