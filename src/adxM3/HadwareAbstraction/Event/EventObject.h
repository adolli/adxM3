
#ifndef _EVENTOBJECT_H_
#define _EVENTOBJECT_H_

#include "../../common/defs.h"
#include "../../common/Function.h"

namespace adxM3
{



	/*
	 * @brief	事件对象抽象类，带有事件源的类可实现该接口
	 *			事件类型列表（枚举）可在派生类中定义
	 *			一个事件对象的一个事件只能注册一个事件监听器
	 *			重复注册以最后一次注册的监听器为准
	 *			移除事件监听器同时也关闭事件对象的事件通知
	 */
	class EventObject
	{
	public:
		


		/*
		 * @brief	事件类型，为方便派生类传入特定枚举值
		 */
		typedef UINT EventType;


		
		/*
		 * @brief	事件通知开关控制
		 * @param	evt 需要开启或关闭通知的事件类型
		 * @retval	成功使能/禁止某个事件通知则返回1，否则返回0
		 */
		virtual int EnableEventNotify(EventType evt) = 0;
		virtual int DisableEventNotify(EventType evt) = 0;



		/*
		 * @brief	添加事件监听器，监听事件对象的某个事件
		 * @param	evt 对象的事件
		 *			FnPtr 触发该事件时的处理函数，由派生类在实现时转换到所需的类型
		 * @retval	成功注册某个事件监听器则返回1，否则返回0
		 */
		virtual int AddEventListener(EventType evt, void* FnPtr)
		{
			return 0;
		}



		/*
		 * @brief	移除某个事件的监听器对象并关闭该事件通知
		 * @param	evt 对象的事件
		 * @retval	成功移除某个事件监听器则返回1，否则返回0
		 * @ovride	派生类可选择不实现该方法，但建议实现
		 */
		virtual int RemoveEventListener(EventType evt)
		{
			DisableEventNotify(evt);
			return 0;
		}



	protected:

		virtual ~EventObject() {}

	};
}

#endif  /*_EVENTOBJECT_H_*/
