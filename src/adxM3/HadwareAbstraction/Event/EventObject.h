
#ifndef _EVENTOBJECT_H_
#define _EVENTOBJECT_H_

#include "../../common/defs.h"
#include "../../common/Function.h"

namespace adxM3
{



	/*
	 * @brief	�¼���������࣬�����¼�Դ�����ʵ�ָýӿ�
	 *			�¼������б�ö�٣������������ж���
	 *			һ���¼������һ���¼�ֻ��ע��һ���¼�������
	 *			�ظ�ע�������һ��ע��ļ�����Ϊ׼
	 *			�Ƴ��¼�������ͬʱҲ�ر��¼�������¼�֪ͨ
	 */
	class EventObject
	{
	public:
		


		/*
		 * @brief	�¼����ͣ�Ϊ���������ഫ���ض�ö��ֵ
		 */
		typedef UINT EventType;


		
		/*
		 * @brief	�¼�֪ͨ���ؿ���
		 * @param	evt ��Ҫ������ر�֪ͨ���¼�����
		 * @retval	�ɹ�ʹ��/��ֹĳ���¼�֪ͨ�򷵻�1�����򷵻�0
		 */
		virtual int EnableEventNotify(EventType evt) = 0;
		virtual int DisableEventNotify(EventType evt) = 0;



		/**
		 * �Ѿ�����
		 */
		virtual int AddEventListener(EventType, void*) { return 0; }
		virtual int RemoveEventListener(EventType) { return 0; }


	protected:

		virtual ~EventObject() {}

	};
}

#endif  /*_EVENTOBJECT_H_*/
