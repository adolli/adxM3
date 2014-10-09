
#ifndef _ABSTRACTPERIPHERAL_H_
#define _ABSTRACTPERIPHERAL_H_

#include "../../common/defs.h"

namespace adxM3
{

	/*
	 * @brief	外设基址访问许可
	 *			若外设模型需要访问寄存器基址请继承该类
	 *			（另外还有外设位带基址访问许可，可参见BitBandBaseAccessible）
	 */
	class BaseAccessible
	{
	public:
		
		/*
		 * @brief	外设基址类型默认为UINT32
		 */
		typedef UINT32 AddressType;



		/*
		 * @brief	获取外设基址（cortex-m3外设必有一基址地址）
		 * @retval	外设基址的地址值
		 */
		virtual AddressType GetPeripheralBase() const = 0;

	};

}

#endif  /*_ABSTRACTPERIPHERAL_H_*/
