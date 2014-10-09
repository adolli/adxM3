
#ifndef _INPUTSTREAM_H_
#define _INPUTSTREAM_H_

#include "StreamBase.h"

namespace adxM3
{

	/*
	 * @brief	输入流抽象类
	 *			实现该类需要实现以下方法，Read读入流，Skip跳过流，
	 *			Available判断流是否可读，Reset重置到流初始处，默
	 *			认流元素为字节，逐个字节读入，该接口已实现多字节
	 *			读入，可设定需要指定读入的字节数，以及InputStream& operator>>(Type&);
	 *			系列方法提供基本数据类型的读入（按字节序读入），
	 *			可根据实际情况，在派生类中重写特定函数以达到需要
	 *			的目的
	 */
	class InputStream : public StreamBase
	{
	public:



		/*
		 * @brief	从流中读取数据，并且阻塞等待读取完成
		 * @param	in 读取的数据存放区
		 *			buffer 读入数据的存放缓冲区
		 *			bufferSize 缓冲区大小（预期读取的字节数）
		 *			actuallyLength 实际读取的字节数
		 * @retval	成功读取则返回true，否则返回false
		 */
		virtual bool Read(UINT8& in) = 0;
		virtual bool Read(void* buffer, size_type bufferSize, size_type& actuallyLength)
		{
			UINT8* _buffer = static_cast<UINT8*>(buffer);
			while (bufferSize--)
			{
				Read(*_buffer++);
			}
			while (!ReadCompleted());
			actuallyLength = bufferSize;
			return true;
		}
	


		/*
		 * @brief	从流中读取数据，并且阻塞等待读取完成（析取运算符重载）
		 * @notice	功能同Read，适应Read方法支持的数据类型，派生类可根据具体情况重写该函数
		 */
		virtual InputStream& operator>>(INT8& in)
		{
			Read((UINT8&)in);
			return *this;
		}
		virtual InputStream& operator>>(INT16& in)
		{
			size_type dummy;
			Read(&in, sizeof(in), dummy);
			return *this;
		}
		virtual InputStream& operator>>(INT32& in)
		{
			size_type dummy;
			Read(&in, sizeof(in), dummy);
			return *this;
		}
		virtual InputStream& operator>>(UINT8& in)
		{
			Read(in);
			return *this;
		}
		virtual InputStream& operator>>(UINT16& in)
		{
			size_type dummy;
			Read(&in, sizeof(in), dummy);
			return *this;
		}
		virtual InputStream& operator>>(UINT32& in)
		{
			size_type dummy;
			Read(&in, sizeof(in), dummy);
			return *this;
		}



		/*
		 * @brief	向前跳过输入流n字节
		 * @param	size 要跳过的字节数
		 * @retval	成功跳过则返回true，否则返回false
		 */
		virtual bool Skip(size_type size) = 0;
		
		
		
		/*
		 * @brief	当前输入流是否有效并可读取
		 * @notice	实现此方法可以不必阻塞等待流有效
		 * @retval	数据有效返回true，否则返回false
		 */
		virtual bool Available() = 0;



		/*
		 * @brief	判断流是否读取完成
		 * @notice	如果不支持此功能请根据实际情况返回
		 *			如果不需要此功能，请返回true
		 * @retval	读取完成返回true，否则返回false
		 */
		virtual bool ReadCompleted() = 0;



		/*
		 * @brief	重置流到其起始处
		 * @notice	如果不支持此方法，请返回false
		 * @retval	成功回到流的起始处返回true，否则返回false
		 */
		virtual bool Reset() = 0;


	protected:

		virtual ~InputStream() {}

	};
}


#endif  /*_INPUTSTREAM_H_*/
