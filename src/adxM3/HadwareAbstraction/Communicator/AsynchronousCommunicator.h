
#ifndef _ASYNCHRONOUSCOMMUNICATOR_H_
#define _ASYNCHRONOUSCOMMUNICATOR_H_

#include "Communicator.h"

namespace adxM3
{


	/*
	 * @brief	异步通讯器接口类，继承于基本通讯器
	 *			该接口提供异步通讯停止位配置方法
	 */
	class AsynchronousCommunicator : public ConfigurableCommunicator
	{
	public:



		/*
		 * @brief	停止位枚举
		 */
		enum STB
		{
			STOPBITS_05			= 0x01,				// 0.5个停止位
			STOPBITS_10			= 0x00,				// 1.0个停止位
			STOPBITS_15			= 0x03,				// 1.5个停止位
			STOPBITS_20			= 0x02,				// 2.0个停止位
		};



		/*
		 * @brief	异步通讯器停止位配置/获取
		 * @param	stb 停止位枚举
		 *				@arg STOPBITS_05
		 *				@arg STOPBITS_10
		 *				@arg STOPBITS_15
		 *				@arg STOPBITS_20
		 * @retval	设置方法中，成功配置则返回1，否则返回0
		 *			获取方法中，不支持停止位返回STOPBITS_10，没有对应的停止位则返回匹配值最近的
		 */
		virtual int SetStopBits(const STB stb) = 0;
		virtual STB GetStopBits() const = 0;


	};

}


#endif  /*_ASYNCHRONOUSCOMMUNICATOR_H_*/
