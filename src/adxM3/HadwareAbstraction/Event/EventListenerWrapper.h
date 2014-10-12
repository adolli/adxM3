
#ifndef _ADX_HA_EVENTLISENERCONTAINER_H_
#define _ADX_HA_EVENTLISENERCONTAINER_H_

namespace adxM3
{


	/*
	 * @brief	事件监听器外覆类
	 *			用于包装事件监听器并使其指向默认的空白事件监听器
	 * @tparam	ListenerType 事件监听器类型
	 */
	template< class ListenerType >
	class EventListenerWrapper
	{
	public:

		/*
		 * @brief	事件监听器外附类默认构造函数
		 * @param	l 监听器指针，初始化默认指向一个空白监听器
		 */
		EventListenerWrapper(ListenerType* l = new ListenerType())
			: listener(l)
		{
		}

		virtual ~EventListenerWrapper()
		{
			// 空对象消耗1字节
			// 需要进行内存负荷测试
			delete listener;
		}


		void SetEventListener(const ListenerType* l)
		{
			listener = const_cast<ListenerType*>(l);
		}

		ListenerType& GetEventListener() const
		{
			return *listener;
		}



	protected:

		ListenerType* listener;
	};

}

#endif  /*_ADX_HA_EVENTLISENERCONTAINER_H_*/
