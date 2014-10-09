
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
	 * @brief	�ź�������
	 */
	template
	< 
		class SignalType, 
		bool SwitchALTPolicy = AutomaticSwitchingMultiplexChannel 
	> 
	class SignalPin;



	/*
	 * @brief	�ź������ࣨSignal class templateƫ�ػ���
	 * @tparam	Signal �ź���ģ��
	 *			PT �ź�ȫ�ֶ˿���Ϣ��
	 *			PIN_INDEX �ź�ȫ�ֶ˿����ź�
	 *			ALT_INDEX �źŶ�·������ͨ����
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
				// ���๹��ʱ�Ѿ���ʱ��
				if (SwitchALTPolicy == AutomaticSwitchingMultiplexChannel)
				{
					MultiplexConfig(ALT_INDEX);
				}
			}

			~SignalPin()
			{
				// ����ʱ�ָ��л���ALT0�Ա��ڽڵ磬ALT0�����Ϊģ��ͨ��
				MultiplexConfig(ALT0);
			}

		};


}

#endif  /*_ADX_K60_SIGNALPIN_H_*/

