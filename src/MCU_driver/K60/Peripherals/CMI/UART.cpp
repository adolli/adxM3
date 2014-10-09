
#include "UART.h"
using namespace K60;



#define UARTx_RX_TX_IRQH(x)															 \
void UART ## x ## _RX_TX_IRQHandler(void)											 \
{																					 \
	Peripheral<UART ## x>& InstanceRef 											 	 \
		= *static_cast<Peripheral<UART ## x>*>(UART ## x ::InstancePtr);		 	 \
																					 \
	if (UART ## x ::BaseAccess()->S1 & _BV(UART_BitBandType::S1_RDRF))				 \
	{																				 \
		/* data available */														 \
		UINT8 received = UART ## x ::BaseAccess()->D;								 \
		InstanceRef.dataAvailableListenerWrapper.GetEventListener().OnReceived(&InstanceRef, received);		 \
	}																				 \
	else if (UART ## x ::BaseAccess()->S1 & _BV(UART_BitBandType::S1_TC))			 \
	{																				 \
		/* send complete */															 \
		InstanceRef.sendCompletedISR(&InstanceRef);									 \
		UART ## x ::BaseAccess()->S1 &= ~_BV(UART_BitBandType::S1_TC);				 \
	}																				 \
	else if (UART ## x ::BaseAccess()->S1 & _BV(UART_BitBandType::S1_TDRE))			 \
	{																				 \
		/* ready to send */															 \
		InstanceRef.readyToSendISR(&InstanceRef);									 \
	}																				 \
}

UARTx_RX_TX_IRQH(0)	
UARTx_RX_TX_IRQH(1)
UARTx_RX_TX_IRQH(2)
UARTx_RX_TX_IRQH(3)
UARTx_RX_TX_IRQH(4)
UARTx_RX_TX_IRQH(5)
