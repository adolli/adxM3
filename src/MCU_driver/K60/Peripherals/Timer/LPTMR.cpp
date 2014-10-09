
#include "LPTMR.h"
using namespace K60;
using namespace adxM3;


void LPTimer_IRQHandler(void)
{
	Peripheral<LPTMR0>& InstanceRef
		= *static_cast<Peripheral<LPTMR0>*>(LPTMR0::InstancePtr);

	if (LPTMR0::BaseAccess()->CSR.TCF == SET)
	{
		InstanceRef.timeOutISR(&InstanceRef);
		LPTMR0::BaseAccess()->CSR.TCF = SET;
	}
}
