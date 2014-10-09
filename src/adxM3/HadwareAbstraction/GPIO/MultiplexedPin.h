
#ifndef _ADX_HA_MULTIPLEXEDPIN_H_
#define _ADX_HA_MULTIPLEXEDPIN_H_

#include "../../common/defs.h"

namespace adxM3
{

	class MultiplexedPin
	{
	public:



		/*
		 * @brief	可复用引脚复用配置与获取当前配置
		 * @param	alt 引脚复用类型或通道号，由派生类决定
		 */
		virtual int MultiplexConfig(unsigned int alt) = 0;
		virtual unsigned int GetMultiplexing() const = 0;



		virtual ~MultiplexedPin() {}

	};

}


#endif  /*_ADX_HA_MULTIPLEXEDPIN_H_*/
