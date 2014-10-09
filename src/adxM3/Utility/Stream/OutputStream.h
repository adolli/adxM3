
#ifndef _OUTPUTSTREAM_H_
#define _OUTPUTSTREAM_H_

#include "StreamBase.h"


namespace adxM3
{


	/*
	 * @brief	输出流抽象类，实现该接口需要实现以下方法，
	 *			Write写入流，Flush冲刷流，
	 *			WriteCompleted写入完成测试，输出流默认按照
	 *			字节为元素，逐个字节输出，该类已实现多字节
	 *			缓冲区的输出方法，以及 OutputStream& operator<<(Type); 
	 *			系列方法按字节输出基本数据类型，可根据实际
	 *			情况在派生类中重写OutputStream& operator<<(Type);
	 *			方法以实现不同数据类型的输出
	 */
	class OutputStream : public StreamBase
	{
	public:


		/*
		 * @brief	写入流，默认只需实现单字节为流元素
		 * @param	out 输出的字节
		 *			buffer 缓冲区首地址
		 *			bufferSize 缓冲区大小
		 * @notice	如果类不支持该方法，实现该方法时直接返回true，
		 * @retval	true表示成功，false表示失败
		 */
		virtual bool Write(const UINT8 out) = 0;
		virtual bool Write(const void* buffer, size_type bufferSize)
		{
			const UINT8* _buffer = static_cast<const UINT8*>(buffer);
			while (bufferSize--)
			{
				Write(*_buffer++);
			}
			while (!WriteCompleted());
			return true;
		}



		/*
		 * @brief	写入流（插入运算符重载）
		 * @param	out 输出的字节、半字、字、双字，不管是什么类型，默认都按照一个字节一个字节传输
		 * @notice	该方法不考虑字节序和有无符号类型，派生类中可以重写该函数以实现派生类中所需功能
		 * @retval	返回自身引用方便级联
		 */
		virtual OutputStream& operator<<(const INT8 out)
		{
			Write((UINT8)out);
			return *this;
		}
		virtual OutputStream& operator<<(const INT16 out)
		{
			Write(&out, sizeof(out));
			return *this;
		}
		virtual OutputStream& operator<<(const INT32 out)
		{
			Write(&out, sizeof(out));
			return *this;
		}
		virtual OutputStream& operator<<(const UINT8 out)
		{
			Write(out);
			return *this;
		}
		virtual OutputStream& operator<<(const UINT16 out)
		{
			Write(&out, sizeof(out));
			return *this;
		}
		virtual OutputStream& operator<<(const UINT32 out)
		{
			Write(&out, sizeof(out));
			return *this;
		}



		/*
		 * @brief	冲刷缓冲区
		 * @notice	如果类不支持该方法，实现该方法时直接返回true，
		 * @retval	true表示成功，false表示失败
		 */
		virtual bool Flush() = 0;



		/*
		 * @brief	写入流完成判断
		 * @notice	如果类不支持该方法，实现该方法时直接返回true，
		 * @retval	true表示流已完成写入，false表示未完成
		 */
		virtual bool WriteCompleted() = 0;


	protected:
		virtual ~OutputStream() {}
	};
}


#endif  /*_OUTPUTSTREAM_H_*/
