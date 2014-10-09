

//////////////////////////////////////////////////////////////////////////
// @file		SignalMultiplexing.h
// @brief		多路复用的信号类型定义
//
//
//
//////////////////////////////////////////////////////////////////////////


#ifndef _ADX_K60_SIGNALMULTIPLEXING_H_
#define _ADX_K60_SIGNALMULTIPLEXING_H_

#include "adxM3.h"

namespace K60
{

	namespace PinMultiplexing
	{

		/*
		 * @brief	信号复合与分配选项
		 */
		enum MUX_MODE
		{
			ALT0,
			ALT1,
			ALT2,
			ALT3,
			ALT4,
			ALT5,
			ALT6,
			ALT7,
			EzPort,
			
			ANALOG	= 0,
			GPIO	= 1
		};

	}
	
	using namespace PinMultiplexing;


	
	/*
	 * @brief	多路复用器信号类型模版
	 *			通过Port Pin MUX 三个参数确定信号通道
	 * @tparam	PORT_INFO_Type 端口信息类型
	 *			PIN_INDEX 引脚索引号
	 *			ALT_INDEX 复用通道索引号
	 */
	template< class PORT_INFO_Type, PinIndex PIN_INDEX, MUX_MODE ALT_INDEX >
	class Signal
	{
	public:
		typedef PORT_INFO_Type PortInfo;
		enum 
		{ 
			Pin = PIN_INDEX
		};

		static const MUX_MODE MUX_ALT = ALT_INDEX;
	};





}

#endif  /*_ADX_K60_SIGNALMULTIPLEXING_H_*/
