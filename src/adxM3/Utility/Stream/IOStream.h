
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
		 * @brief	写入流
		 * @param	out 输出的字节、半字、字或者缓冲区首地址，有符号整数当成无符号整数处理
					bufferSize 缓冲区大小
		 * @notice	如果类不支持该方法，实现该方法时直接返回true，
		 * @retval	true表示成功，false表示失败
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
		 * @brief	串口异步输出流写入字符串，重载函数，重载插入运算符，
		 * @param	outstr 待发送字符串
		 * @notice	该方法会等待最后一个字节发送完成（直到遇到'\0'）
		 * @retval	返回该流自身引用，方便级联
		 */
		virtual IOStream& operator<<(const char* outstr)
		{
			// 使用带有缓冲的发送模型
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
			
				// 等待最后一个字节发送完成
				while (!device.SendComplete());
			}
			return *this;
		}



		/*
		 * @brief	读取流
		 * @param	in 读取的数据存放区
					size 预期读取的字节数
					actuallyLength 实际读取的字节数
		 * @retval	成功读取则返回true，否则返回false
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
		 * @brief	串口异步输入流读取字符串，重载函数，重载析取运算符，
		 * @param	instr 输入的字符串存放缓冲区指针
		 * @notice	遇到'\0'或'\r\n'则返回，instr以\0结尾 
		 * @retval	返回该流自身引用，方便级联
		 */
		virtual IOStream& operator>>(char* instr)
		{
			// 稍后实现这个功能
			return *this;
		}



		/*
		 * @brief	非缓冲通信输出流不支持关闭与冲刷方法，不使用
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
		 * @brief	写入流完成判断
		 * @retval	true表示流已完成写入，false表示未完成
		 */
		virtual bool WriteCompleted()
		{
			return device.SendComplete();
		}



		/*
		 * @brief	判断流是否读取完成
		 * @retval	接收并且读取完成返回true，否则返回false
		 */
		virtual bool ReadCompleted()
		{
			return device.ReceiveCompleted();
		}



		/*
		 * @brief	向前跳过输入流n字节
		 * @param	size 要跳过的字节数
		 * @retval	成功跳过则返回true，否则返回false
		 */
		virtual bool Skip(size_type size)
		{
			return false;
		}
		
		
		
		/*
		 * @brief	当前通讯输入流是否有效并可读取
		 * @retval	数据有效返回true，否则返回false
		 */
		virtual bool Available()
		{
			return device.DataAvailable();
		}



		/*
		 * @brief	重置流到其起始处
		 * @notice	通讯流为非缓冲流，不支持该方法，返回false
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
