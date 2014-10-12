
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



		/**
		 * 已经弃用
		 */
		virtual int AddEventListener(EventType, void*) { return 0; }
		virtual int RemoveEventListener(EventType) { return 0; }


	protected:

		virtual ~EventObject() {}

	};
}

#endif  /*_EVENTOBJECT_H_*/
