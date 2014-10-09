
#ifndef _PERIPHERALONOFF_H_
#define _PERIPHERALONOFF_H_


namespace adxM3
{
	

	/*
	 * @brief	������Ϊ���ؿ��ƽӿ���
	 *			�޷����п��ؿ��Ƶ����轨�鲻Ҫ�̳и���
	 */
	class PeripheralOnOff
	{
	public:


		/*
		 * @brief	���迪��/�رսӿڷ���
		 * @retval	�������޷�ʵ����ִ��Ч�����ʱ�뷵��0�����򷵻�1
		 *			e.g. ������EnablePeripheral������ĳ����ʼ�մ��ڿ���״̬�򷵻�1
		 *				 ������DisablePeripheral������ĳ����ʼ�մ��ڿ���״̬�޷��ر�ʱ�뷵��0
		 */
		virtual int EnablePeripheral() = 0;
		virtual int DisablePeripheral() = 0;


	};

}


#endif  /*_PERIPHERALONOFF_H_*/
