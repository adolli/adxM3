
#ifndef _OUTPUTSTREAM_H_
#define _OUTPUTSTREAM_H_

#include "StreamBase.h"


namespace adxM3
{


	/*
	 * @brief	����������࣬ʵ�ָýӿ���Ҫʵ�����·�����
	 *			Writeд������Flush��ˢ����
	 *			WriteCompletedд����ɲ��ԣ������Ĭ�ϰ���
	 *			�ֽ�ΪԪ�أ�����ֽ������������ʵ�ֶ��ֽ�
	 *			������������������Լ� OutputStream& operator<<(Type); 
	 *			ϵ�з������ֽ���������������ͣ��ɸ���ʵ��
	 *			���������������дOutputStream& operator<<(Type);
	 *			������ʵ�ֲ�ͬ�������͵����
	 */
	class OutputStream : public StreamBase
	{
	public:


		/*
		 * @brief	д������Ĭ��ֻ��ʵ�ֵ��ֽ�Ϊ��Ԫ��
		 * @param	out ������ֽ�
		 *			buffer �������׵�ַ
		 *			bufferSize ��������С
		 * @notice	����಻֧�ָ÷�����ʵ�ָ÷���ʱֱ�ӷ���true��
		 * @retval	true��ʾ�ɹ���false��ʾʧ��
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
		 * @brief	д������������������أ�
		 * @param	out ������ֽڡ����֡��֡�˫�֣�������ʲô���ͣ�Ĭ�϶�����һ���ֽ�һ���ֽڴ���
		 * @notice	�÷����������ֽ�������޷������ͣ��������п�����д�ú�����ʵ�������������蹦��
		 * @retval	�����������÷��㼶��
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
		 * @brief	��ˢ������
		 * @notice	����಻֧�ָ÷�����ʵ�ָ÷���ʱֱ�ӷ���true��
		 * @retval	true��ʾ�ɹ���false��ʾʧ��
		 */
		virtual bool Flush() = 0;



		/*
		 * @brief	д��������ж�
		 * @notice	����಻֧�ָ÷�����ʵ�ָ÷���ʱֱ�ӷ���true��
		 * @retval	true��ʾ�������д�룬false��ʾδ���
		 */
		virtual bool WriteCompleted() = 0;


	protected:
		virtual ~OutputStream() {}
	};
}


#endif  /*_OUTPUTSTREAM_H_*/
