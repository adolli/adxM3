
#include "SPI.h"
using namespace K60;
using namespace adxM3;


#define SPIx_IRQH_where_x_equal(x)													 \
void SPI ## x ## _IRQHandler(void)													 \
{																					 \
	Peripheral<SPI ## x>& InstanceRef 											 	 \
		= *static_cast<Peripheral<SPI ## x>*>(SPI ## x ::InstancePtr);		 		 \
																					 \
	if (	SPI ## x ::BaseAccess()->SR.RFDF == 1									 \
		&&  SPI ## x ::BaseAccess()->RSER.RFDF_RE == 1)								 \
	{																				 \
		/* data available */														 \
		UINT16 received = SPI ## x ::BaseAccess()->POPR;							 \
		InstanceRef.dataAvailableISR(&InstanceRef, received);						 \
		SPI ## x ::BaseAccess()->SR.RFDF = 1;										 \
	}																				 \
	else if (	SPI ## x ::BaseAccess()->SR.TCF == 1								 \
			&&  SPI ## x ::BaseAccess()->RSER.TCF_RE == 1)							 \
	{																				 \
		/* send complete */															 \
		InstanceRef.sendCompletedISR(&InstanceRef);									 \
		SPI ## x ::BaseAccess()->SR.TCF = 1;										 \
	}																				 \
	else if (	SPI ## x ::BaseAccess()->SR.TFFF == 1								 \
			&&  SPI ## x ::BaseAccess()->RSER.TFFF_RE == 1)							 \
	{																				 \
		/* ready to send */															 \
		InstanceRef.readyToSendISR(&InstanceRef);									 \
		SPI ## x ::BaseAccess()->SR.TFFF = 1;										 \
	}																				 \
}

SPIx_IRQH_where_x_equal(0)
SPIx_IRQH_where_x_equal(1)
SPIx_IRQH_where_x_equal(2)
