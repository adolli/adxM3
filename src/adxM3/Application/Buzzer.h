
#ifndef _ADX_APP_BUZZER_H_
#define _ADX_APP_BUZZER_H_


#include "../common/defs.h"
#include "../HadwareAbstraction/GPIO/GeneralPurposeInputOutputPin.h"
#include "../HadwareAbstraction/Timer/BlockingDelayController.h"


namespace adxM3
{

	/*
	 * @brief	��������
	 * @tparam	gpiopin gpio���������ͣ�Ҫ��ʵ�����·���
	 *				@required Set
	 *				@required Reset
	 *				@required SetDirection(IODIR)
	 *				@required OutputConfig(OT)
	 */ 
	template< class gpiopin >
	struct Beeper
	{
		Beeper()
		{
			pin.SetDirection(GeneralPurposeInputOutput::OUT);
			pin.OutputConfig(GeneralPurposeInputOutput::PushPull);
			Off();
		}
		~Beeper()
		{
			Off();
		}
		void On()
		{
			pin.Reset();
		}
		void Off()
		{
			pin.Set();
		}

		gpiopin pin;
	};







	/*
	 * @brief	����������࣬�̳��ڷ�������
	 * @tparam	gpiopin gpio���������ͣ����������ͬҪ��
	 */ 
	template< class gpiopin >
	struct MultiFrequencyBeeper
		: public Beeper<gpiopin>
	{

		using Beeper<gpiopin>::On;
		using Beeper<gpiopin>::Off;


		MultiFrequencyBeeper(BlockingDelayController* _sys)
			: sys(_sys)
		{
		}
		void Tune(UINT32 freq)
		{
			UINT32 time_us = 1000000 / 2 / freq;
			for (int i = 0; i < 60; ++i)
			{
				On();
				sys->Delay_ms(time_us / 1000);
				sys->Delay_us(time_us % 1000);
				Off();
				sys->Delay_ms(time_us / 1000);
				sys->Delay_us(time_us % 1000);
			}
		}

	protected:

		BlockingDelayController* sys;

	};



}

#endif  /*_ADX_APP_BUZZER_H_*/
