
#ifndef _TIMERIOENABLER_H_
#define _TIMERIOENABLER_H_


namespace adxM3
{

	class TimerIOEnabler
	{
	public:


		/*
		 * @brief	������������ʹ��/��ֹ
		 *			�����ú�����������ģʽ����
		 * @retval	�ɹ�����/�ر��������1�����򷵻�0
		 */
		virtual int EnableOutput() = 0;
		virtual int DisableOutput() = 0;



		virtual ~TimerIOEnabler() {}
	};

}


#endif  /*_TIMERIOENABLER_H_*/
