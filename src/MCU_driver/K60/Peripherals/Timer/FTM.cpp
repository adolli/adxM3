
#include "FTM.h"
using namespace K60;
using namespace adxM3;

#define FTMx_IRQH_where_x_equal(x)											\
void FTM ##x## _IRQHandler(void)											\
{																			\
	Peripheral<FTM ##x >& InstanceRef										\
		= *static_cast<Peripheral<FTM ##x >*>(FTM ##x ::InstancePtr);		\
																			\
	if (   FTM ##x ::BaseAccess()->SC.TOF == SET							\
		&& FTM ##x ::BaseAccess()->SC.TOIE == SET)							\
	{																		\
		InstanceRef.timeOutISR(&InstanceRef);								\
																			\
		/* Clear by writing 0 to corresponding bit */						\
		FTM ##x ::BaseAccess()->SC.TOF = RESET;								\
	}																		\
}

FTMx_IRQH_where_x_equal(0)
FTMx_IRQH_where_x_equal(1)
FTMx_IRQH_where_x_equal(2)
