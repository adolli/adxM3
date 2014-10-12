
#ifndef _ADX_HA_EVENTLISENERCONTAINER_H_
#define _ADX_HA_EVENTLISENERCONTAINER_H_

namespace adxM3
{


	/*
	 * @brief	�¼��������⸲��
	 *			���ڰ�װ�¼���������ʹ��ָ��Ĭ�ϵĿհ��¼�������
	 * @tparam	ListenerType �¼�����������
	 */
	template< class ListenerType >
	class EventListenerWrapper
	{
	public:

		/*
		 * @brief	�¼��������⸽��Ĭ�Ϲ��캯��
		 * @param	l ������ָ�룬��ʼ��Ĭ��ָ��һ���հ׼�����
		 */
		EventListenerWrapper(ListenerType* l = new ListenerType())
			: listener(l)
		{
		}

		virtual ~EventListenerWrapper()
		{
			// �ն�������1�ֽ�
			// ��Ҫ�����ڴ渺�ɲ���
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
