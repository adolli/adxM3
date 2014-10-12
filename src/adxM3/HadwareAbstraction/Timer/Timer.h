
#ifndef _TIMER_H_
#define _TIMER_H_

#include "../Event/EventObject.h"
#include "../Event/EventListenerWrapper.h"
#include "../Event/TimerUpdateListener.h"

namespace adxM3
{

	/*
	 * @brief	定时器抽象类
	 *			此类中抽象定时器所有行为
	 *			包括设置定时时长或频率，事件类型，定时器启动停止暂停等基本控制
	 */
	class Timer : public EventObject
	{
	public:


		/*
		 * @brief	定时器事件类型枚举
		 */
		enum EVT
		{
			TIME_OUT,						// 定时时间到，即定时器中计数器到达某个值后触发的事件
			EVT_COUNT
		};



		/*
		 * @brief	启动计时
		 */
		virtual void Start() = 0;



		/*
		 * @brief	启动计时，时间到后则停止（只计时一次）
		 */
		virtual void StartOnce() = 0;



		/*
		 * @brief	停止计时（计时器归零）
		 */
		virtual void Stop() = 0;



		/*
		 * @brief	暂停计时（计时值保留）
		 */
		virtual void Pause() = 0;



		/*
		 * @brief	继续计时，与Pause配对使用
		 */
		virtual void Continue() = 0;



		/*
		 * @brief	计时器重启，先归零再启动，不产生更新事件
		 */
		virtual void Restart()
		{
			Stop();
			Start();
		}



		/*
		 * @brief	获取当前计时器计时时间值
		 */
		virtual UINT32 GetTime() const = 0;



		/*
		 * @brief	设置计时器定时时长
		 */
		virtual void SetTimeOut(UINT32 time) = 0;



		/*
		 * @brief	设置定时器定时频率，
		 * @notice	功能同SetTimeOut，但设置的是TimeOut的倒数，即频率
		 */
		virtual void SetFrequency(UINT32 freq) = 0;	



		/*
		 * @brief	获取定时器定时时长
		 */
		virtual UINT32 GetTimeOut() const = 0;	



		/*
		 * @brief	获取定时器定时频率
		 */
		virtual UINT32 GetFrequency() const = 0;	




		/*
		 * @brief	判断定时器是否正在计时
		 * @retval	正在计时返回true，否则返回false
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
