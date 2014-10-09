
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



		/*
		 * @brief	����¼��������������¼������ĳ���¼�
		 * @param	evt ������¼�
		 *			FnPtr �������¼�ʱ�Ĵ�����������������ʵ��ʱת�������������
		 * @retval	�ɹ�ע��ĳ���¼��������򷵻�1�����򷵻�0
		 */
		virtual int AddEventListener(EventType evt, void* FnPtr)
		{
			return 0;
		}



		/*
		 * @brief	�Ƴ�ĳ���¼��ļ��������󲢹رո��¼�֪ͨ
		 * @param	evt ������¼�
		 * @retval	�ɹ��Ƴ�ĳ���¼��������򷵻�1�����򷵻�0
		 * @ovride	�������ѡ��ʵ�ָ÷�����������ʵ��
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
