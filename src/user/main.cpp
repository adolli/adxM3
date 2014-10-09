  
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
	urtios<<"hello K60!\r\n";


	

	struct UART_EventListener : public ReceiveListener<UINT8>
	{
		virtual void OnReceived(void* This, UINT8 received)
		{
			TextStream<Communicator> ts(*(Communicator*)This);
			ts<<"received: "<<by_char<<received<<" (ASCII 0x"<<by_ascii<<hex<<received<<")"<<endl;
		}
	};
	URT.AddEventListener(new UART_EventListener());


	Peripheral<PIT>::CH<0> timer1;
	struct PIT_EventListener
	{
		static void OnUpdate(Timer* This)
		{		
			
		}
	};
	timer1.SetTimeOut(100000);
	timer1.AddEventListener(Timer::TIME_OUT, PIT_EventListener::OnUpdate);
	timer1.Start();

	
	
	Peripheral<PIT>::CH<1> timer2_for_test;
	struct timer2_EventListener
	{
		static void OnUpdate(Timer* This)
		{
			TextStream<Communicator> urtios(URT);
		}
	};
	timer2_for_test.SetTimeOut(2000);
	timer2_for_test.AddEventListener(Timer::TIME_OUT, timer2_EventListener::OnUpdate);

	

	while (1)
	{	
	}
	
	// unreached
	// return 0;
}







