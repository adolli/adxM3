
#ifndef _ADX_K60_SIGNALPIN_H_
#define _ADX_K60_SIGNALPIN_H_

#include "adxM3.h"
#include "SignalMultiplexing.h"
#include "../PeripheralBase.h"

namespace K60
{
	
	enum 
	{
		RetainCurrentMultiplexChannel = false,
		AutomaticSwitchingMultiplexChannel = !RetainCurrentMultiplexChannel
	};



	/*
	 * @brief	信号引脚类
	 */
	template
	< 
		class SignalType, 
		bool SwitchALTPolicy = AutomaticSwitchingMultiplexChannel 
	> 
	class SignalPin;



	/*
	 * @brief	信号引脚类（Signal class template偏特化）
	 * @tparam	Signal 信号类模版
	 *			PT 信号全局端口信息类
	 *			PIN_INDEX 信号全局端口引脚号
	 *			ALT_INDEX 信号多路复用器通道号
	 */
	template< class PT, PinIndex PIN_INDEX, MUX_MODE ALT_INDEX, bool SwitchALTPolicy >
	class SignalPin
		< 
			Signal<PT, PIN_INDEX, ALT_INDEX>, 
			SwitchALTPolicy
		>
		: public Peripheral<PT>::template ConstPin<PIN_INDEX>
		{
		public:

			using Peripheral<PT>::template ConstPin<PIN_INDEX>::MultiplexConfig;

			static const MUX_MODE ALT = ALT_INDEX;

			SignalPin()
			{
				// 基类构造时已经打开时钟
				if (SwitchALTPolicy == AutomaticSwitchingMultiplexChannel)
				{
					MultiplexConfig(ALT_INDEX);
				}
			}

			~SignalPin()
			{
				// 析构时恢复切换到ALT0以便于节电，ALT0大多数为模拟通道
				MultiplexConfig(ALT0);
			}

		};


}

#endif  /*_ADX_K60_SIGNALPIN_H_*/

