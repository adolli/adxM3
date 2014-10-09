
#ifndef _ABSTRACTPERIPHERAL_H_
#define _ABSTRACTPERIPHERAL_H_

#include "../../common/defs.h"

namespace adxM3
{

	/*
	 * @brief	�����ַ�������
	 *			������ģ����Ҫ���ʼĴ�����ַ��̳и���
	 *			�����⻹������λ����ַ������ɣ��ɲμ�BitBandBaseAccessible��
	 */
	class BaseAccessible
	{
	public:
		
		/*
		 * @brief	�����ַ����Ĭ��ΪUINT32
		 */
		typedef UINT32 AddressType;



		/*
		 * @brief	��ȡ�����ַ��cortex-m3�������һ��ַ��ַ��
		 * @retval	�����ַ�ĵ�ֵַ
		 */
		virtual AddressType GetPeripheralBase() const = 0;

	};

}

#endif  /*_ABSTRACTPERIPHERAL_H_*/
