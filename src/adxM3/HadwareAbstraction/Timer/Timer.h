
#ifndef _TIMER_H_
#define _TIMER_H_

#include "../Event/EventObject.h"
#include "../Event/EventListenerWrapper.h"
#include "../Event/TimerUpdateListener.h"

namespace adxM3
{

	/*
	 * @brief	��ʱ��������
	 *			�����г���ʱ��������Ϊ
	 *			�������ö�ʱʱ����Ƶ�ʣ��¼����ͣ���ʱ������ֹͣ��ͣ�Ȼ�������
	 */
	class Timer : public EventObject
	{
	public:


		/*
		 * @brief	��ʱ���¼�����ö��
		 */
		enum EVT
		{
			TIME_OUT,						// ��ʱʱ�䵽������ʱ���м���������ĳ��ֵ�󴥷����¼�
			EVT_COUNT
		};



		/*
		 * @brief	������ʱ
		 */
		virtual void Start() = 0;



		/*
		 * @brief	������ʱ��ʱ�䵽����ֹͣ��ֻ��ʱһ�Σ�
		 */
		virtual void StartOnce() = 0;



		/*
		 * @brief	ֹͣ��ʱ����ʱ�����㣩
		 */
		virtual void Stop() = 0;



		/*
		 * @brief	��ͣ��ʱ����ʱֵ������
		 */
		virtual void Pause() = 0;



		/*
		 * @brief	������ʱ����Pause���ʹ��
		 */
		virtual void Continue() = 0;



		/*
		 * @brief	��ʱ���������ȹ����������������������¼�
		 */
		virtual void Restart()
		{
			Stop();
			Start();
		}



		/*
		 * @brief	��ȡ��ǰ��ʱ����ʱʱ��ֵ
		 */
		virtual UINT32 GetTime() const = 0;



		/*
		 * @brief	���ü�ʱ����ʱʱ��
		 */
		virtual void SetTimeOut(UINT32 time) = 0;



		/*
		 * @brief	���ö�ʱ����ʱƵ�ʣ�
		 * @notice	����ͬSetTimeOut�������õ���TimeOut�ĵ�������Ƶ��
		 */
		virtual void SetFrequency(UINT32 freq) = 0;	



		/*
		 * @brief	��ȡ��ʱ����ʱʱ��
		 */
		virtual UINT32 GetTimeOut() const = 0;	



		/*
		 * @brief	��ȡ��ʱ����ʱƵ��
		 */
		virtual UINT32 GetFrequency() const = 0;	




		/*
		 * @brief	�ж϶�ʱ���Ƿ����ڼ�ʱ
		 * @retval	���ڼ�ʱ����true�����򷵻�false
		 */
		virtual bool TimerIsRunning() const = 0;


		

		EventListenerWrapper<TimerUpdateListener<Timer&> > updateListenerWrapper;

	protected:

		virtual ~Timer()
		{
		}

	};


}

#endif  /*_TIMER_H_*/
