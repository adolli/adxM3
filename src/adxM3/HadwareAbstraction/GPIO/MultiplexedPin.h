
#ifndef _ADX_HA_MULTIPLEXEDPIN_H_
#define _ADX_HA_MULTIPLEXEDPIN_H_

#include "../../common/defs.h"

namespace adxM3
{

	class MultiplexedPin
	{
	public:



		/*
		 * @brief	�ɸ������Ÿ����������ȡ��ǰ����
		 * @param	alt ���Ÿ������ͻ�ͨ���ţ������������
		 */
		virtual int MultiplexConfig(unsigned int alt) = 0;
		virtual unsigned int GetMultiplexing() const = 0;



		virtual ~MultiplexedPin() {}

	};

}


#endif  /*_ADX_HA_MULTIPLEXEDPIN_H_*/
