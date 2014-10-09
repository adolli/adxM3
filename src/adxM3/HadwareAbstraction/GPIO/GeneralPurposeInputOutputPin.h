
#ifndef _GENERALPURPOSEINPUTOUTPUTPIN_H_
#define _GENERALPURPOSEINPUTOUTPUTPIN_H_

#include "GeneralPurposeInputOutput.h"
#include "MultiplexedPin.h"

namespace adxM3
{


	/*
	 * @brief	ͨ��Ŀ���������Ӳ������
	 *			���ų���ģ��
	 *			����������ݽӿ�˵���ϸ�ʵ�ֽӿڹ���
	 *			�����麯���ѵ��ýӿڷ���ʵ���书�ܣ������������и���Ч�Ľ���override�÷���
	 */
	class GeneralPurposeInputOutputPin : public GeneralPurposeInputOutput
	{
	public:





		/*
		 * @brief	ͨ��Ŀ���������������λ
		 * @notice	���ģʽ��Ϊ���ø����ŵ�ƽ
		 *			����ģʽ�²�ʹ�ø÷���
		 */
		virtual void Set() = 0;
		


		/*
		 * @brief	ͨ��Ŀ������������Ÿ�λ
		 * @notice	���ģʽ��Ϊ���ø����ŵ�ƽ
		 *			����ģʽ�²�ʹ�ø÷���
		 */
		virtual void Reset() = 0;
		


		/*
		 * @brief	ͨ��Ŀ������������ŵ�ƽ����
		 * @param	level Ҫ���õĵ�ƽ��true��1��ʾ�ߵ�ƽ��false��0��ʾ�͵�ƽ
		 * @notice	���ģʽ��Ϊ���ø����ŵ�ƽ
		 *			����ģʽ�²�ʹ�ø÷���
		 */
		virtual void SetLevel(const bool level) = 0;



		/*
		 * @brief	ͨ��Ŀ�����������ȡ���ŵ�ƽ
		 * @retval	���ŵĵ�ƽ��true��1��ʾ�ߵ�ƽ��false��0��ʾ�͵�ƽ
		 * @notice	���ģʽ��Ϊ����ĵ�ƽ
		 *			����ģʽ��Ϊ����ĵ�ƽ
		 */
		virtual bool GetLevel() const = 0;



		/*
		 * @brief	ͨ��Ŀ������������ŵ�ƽ��ת
		 * @notice	���ģʽ�½���ƽ��ת
		 *			����ģʽ�²�ʹ�ø÷���
		 */
		virtual void Toggle()
		{
			SetLevel(!GetLevel());
		}



		/*
		 * @brief	ͨ��Ŀ�����������ȡ���ŵ�ƽȡ��ֵ
		 * @retval	�������ŵ�ƽȡ����ֵ
		 */
		virtual bool operator!()
		{
			return !GetLevel();
		}



		/*
		 * @brief	ͨ��Ŀ������������ŵ�ƽ����
		 *			���� ��ֵ= �����������ͬSetLevel����
		 * @param	level Ҫ���õĵ�ƽ��true��1��ʾ�ߵ�ƽ��false��0��ʾ�͵�ƽ
		 *			(levelΪint������Ϊ��ƥ������������)
		 * @retval	���ض�����������
		 */
		virtual GeneralPurposeInputOutputPin& operator=(const int level)
		{
			SetLevel(level);
			return *this;
		}
		virtual GeneralPurposeInputOutputPin& operator=(const GeneralPurposeInputOutputPin& pin)
		{
			SetLevel(pin.GetLevel());
			return *this;
		}
		


		/*
		 * @brief	ͨ��Ŀ����������Ƚ����ŵ�ƽֵ
		 * @param	level ���ŵ�ƽ���ֵ���Ƚ�(levelΪint������Ϊ��ƥ������������)
		 *			pin ���ŵ�ƽ��pin�ĵ�ƽ���Ƚ�
		 * @retval	�ȽϽ����ͬʱ����true�����򷵻�false
		 */
		virtual bool operator==(const int level)
		{
			return level == GetLevel();
		}
		virtual bool operator==(const GeneralPurposeInputOutputPin& pin)
		{
			return GetLevel() == pin.GetLevel();
		}



		/*
		 * @brief	ͨ��Ŀ�����������ʽת����ȡ���ŵ�ƽֵ������ͬGetLevel������
		 * @retval	���ŵ�ƽֵ
		 */
		virtual operator bool()
		{
			return GetLevel();
		}



		/*
		 * @brief	ͨ��Ŀ�����������ȡ���ű��
		 * @notice	���ű��Ϊ�˿��е�������
		 */
		virtual PinIndex GetPinSource() const = 0;


	};

}


#endif  /*_GENERALPURPOSEINPUTOUTPUTPIN_H_*/
