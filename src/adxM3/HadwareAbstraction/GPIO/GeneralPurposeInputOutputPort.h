
#ifndef _GENERALPURPOSEINPUTOUTPUTPORT_H_
#define _GENERALPURPOSEINPUTOUTPUTPORT_H_

#include "GeneralPurposeInputOutput.h"

namespace adxM3
{


	/*
	 * @brief	ͨ��Ŀ���������Ӳ������
	 *			�˿ڳ���ģ��
	 *			����������ݽӿ�˵���ϸ�ʵ�ֽӿڹ���
	 *			�����麯���ѵ��ýӿڷ���ʵ���书�ܣ������������и���Ч�Ľ���override�÷���
	 */
	class GeneralPurposeInputOutputPort : public GeneralPurposeInputOutput
	{
	public:


		
		friend class GeneralPurposeInputOutputPin;



		/*
		 * @brief	Ĭ��32λ�˿ڣ�16λ�˿ڻ�8λ�˿�Ҳ�ɼ���
		 */
		typedef UINT32 PortValueType;



		/*
		 * @brief	ͨ��Ŀ������������ö˿�ֵ
		 * @param	value �˿ڵ�ֵ
		 * @notice	���ģʽ��Ϊ���ö˿����ֵ
		 *			����ģʽ�������������ʵ������������������������������ֵ
		 */
		virtual void SetValue(const PortValueType value) = 0;



		/*
		 * @brief	ͨ��Ŀ�����������ȡ�˿�ֵ
		 * @retval	�˿ڵ�ֵ
		 * @notice	���������ֱ��ȡ�����ֵ�������ֵ
		 */
		virtual PortValueType GetInputValue() const = 0;
		virtual PortValueType GetOutputValue() const = 0;






		/*
		 * @brief	ͨ��Ŀ������������ö˿�ֵ
		 *			���� ��ֵ= �������߿ɶ��ԣ����������д�˷���
		 * @notice	δָ���κ�����ʱ���������˿ڸ�ֵ
		 *			ָ���˲�������ʱ��
		 *			��value����0����ѡ���Żᱻ��λ����value������0����ѡ���Żᱻ��λ
		 */
		virtual void operator=(const PortValueType value)
		{
			SetValue(value);
		}
		virtual void operator=(const int value)
		{
			SetValue(PortValueType(value));
		}



		/*
		 * @brief	ͨ��Ŀ������������Զ˿ڵ�ֵ������ֵ�Ƿ����
		 * @param	value �˿�ֵ���ֵ���Ƚ�
		 *			port �˿�ֵ��port�Ķ˿�ֵ���Ƚ�
		 * @retval	����򷵻�true�����򷵻�false
		 * @notice	��ȱȽ������ֻ��������ģʽ��
		 *			���Ҳ�������ѡ���ţ�ֻ�Ƚ������˿ڵ�ֵ
		 */
		virtual bool operator==(const PortValueType value) const
		{
			return value == GetInputValue();
		}
		virtual bool operator==(const int value) const
		{
			return PortValueType(value) == GetInputValue();
		}
		virtual bool operator==(const GeneralPurposeInputOutputPort& port) const
		{
			return GetInputValue() == port.GetInputValue();
		}



		/*
		 * @brief	ͨ��Ŀ����������˿�ֵ��ʽת��������ͬGetInputValue������
		 *			e.g. PortValueType val = PortA;
		 * @retval	�˿�ֵ
		 * @notice	ֻ������ģʽ����Ч�����ģʽ����ʹ�ø÷���
		 */
		virtual operator PortValueType() const
		{
			return GetInputValue();
		}




		/*
		 * @brief	ͨ��Ŀ����������˿ڱ�׼���캯��
		 *			����ʱ��ѡ���κ�����
		 */
		GeneralPurposeInputOutputPort()
		{
		}


	};

	typedef GeneralPurposeInputOutputPort::PortValueType PortValueType;
}


#endif  /*_GENERALPURPOSEINPUTOUTPUTPORT_H_*/
