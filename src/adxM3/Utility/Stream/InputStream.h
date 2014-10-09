
#ifndef _INPUTSTREAM_H_
#define _INPUTSTREAM_H_

#include "StreamBase.h"

namespace adxM3
{

	/*
	 * @brief	������������
	 *			ʵ�ָ�����Ҫʵ�����·�����Read��������Skip��������
	 *			Available�ж����Ƿ�ɶ���Reset���õ�����ʼ����Ĭ
	 *			����Ԫ��Ϊ�ֽڣ�����ֽڶ��룬�ýӿ���ʵ�ֶ��ֽ�
	 *			���룬���趨��Ҫָ��������ֽ������Լ�InputStream& operator>>(Type&);
	 *			ϵ�з����ṩ�����������͵Ķ��루���ֽ�����룩��
	 *			�ɸ���ʵ�������������������д�ض������Դﵽ��Ҫ
	 *			��Ŀ��
	 */
	class InputStream : public StreamBase
	{
	public:



		/*
		 * @brief	�����ж�ȡ���ݣ����������ȴ���ȡ���
		 * @param	in ��ȡ�����ݴ����
		 *			buffer �������ݵĴ�Ż�����
		 *			bufferSize ��������С��Ԥ�ڶ�ȡ���ֽ�����
		 *			actuallyLength ʵ�ʶ�ȡ���ֽ���
		 * @retval	�ɹ���ȡ�򷵻�true�����򷵻�false
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
		 * @brief	�����ж�ȡ���ݣ����������ȴ���ȡ��ɣ���ȡ��������أ�
		 * @notice	����ͬRead����ӦRead����֧�ֵ��������ͣ�������ɸ��ݾ��������д�ú���
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
		 * @brief	��ǰ����������n�ֽ�
		 * @param	size Ҫ�������ֽ���
		 * @retval	�ɹ������򷵻�true�����򷵻�false
		 */
		virtual bool Skip(size_type size) = 0;
		
		
		
		/*
		 * @brief	��ǰ�������Ƿ���Ч���ɶ�ȡ
		 * @notice	ʵ�ִ˷������Բ��������ȴ�����Ч
		 * @retval	������Ч����true�����򷵻�false
		 */
		virtual bool Available() = 0;



		/*
		 * @brief	�ж����Ƿ��ȡ���
		 * @notice	�����֧�ִ˹��������ʵ���������
		 *			�������Ҫ�˹��ܣ��뷵��true
		 * @retval	��ȡ��ɷ���true�����򷵻�false
		 */
		virtual bool ReadCompleted() = 0;



		/*
		 * @brief	������������ʼ��
		 * @notice	�����֧�ִ˷������뷵��false
		 * @retval	�ɹ��ص�������ʼ������true�����򷵻�false
		 */
		virtual bool Reset() = 0;


	protected:

		virtual ~InputStream() {}

	};
}


#endif  /*_INPUTSTREAM_H_*/
