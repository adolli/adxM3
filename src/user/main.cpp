  
#include "adxM3.h"
#include "adx_K60.h"

using namespace K60;
using namespace adxM3;

#include <cstdlib>
using namespace std;

Peripheral<UART0> URT;
Peripheral<GPIOB>::ConstPin<8> LED;

int main()
{
	
	LED.SetDirection(GeneralPurposeInputOutput::OUT);
	LED.OutputConfig(GeneralPurposeInputOutput::PushPull);
	LED = 0;
	

	TextStream<Communicator> urtios(URT);
	urtios << "hello K60!\r\n";


	

	struct UART_EventListener : public ReceiveListener<UINT8>
	{
		virtual void OnReceived(void* This, UINT8 received)
		{
			TextStream<Communicator> ts(*(Communicator*)This);
			ts<<"received: "<<by_char<<received<<" (ASCII 0x"<<by_ascii<<hex<<received<<")"<<endl;
		}
	};
	URT.SetEventListener(new UART_EventListener());


	Peripheral<PIT>::CH<0> timer0;
	struct PIT_EventListener : public TimerUpdateListener<Timer&>
	{
		virtual void OnUpdate(Timer& This)
		{		
			
		}
	};
	timer0.SetTimeOut(100000);
	timer0.SetEventListener(new PIT_EventListener());
	timer0.Start();

	
	
	Peripheral<PIT>::CH<1> timer2_for_test;
	struct timer2_EventListener : public TimerUpdateListener<Timer&>
	{
		const char* message;
		
		timer2_EventListener(const char* msg)
			: message(msg)
		{}

		virtual void OnUpdate(Timer& This)
		{
			This.Stop();
			TextStream<Communicator> urtios(URT);
			urtios << message << endl;
		}
	};
	timer2_for_test.SetTimeOut(2000);
	timer2_for_test.SetEventListener(new timer2_EventListener("test!"));

	

	while (1)
	{	
	}
	
	// unreached
	// return 0;
}







