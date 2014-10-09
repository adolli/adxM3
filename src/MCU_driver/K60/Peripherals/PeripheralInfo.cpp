
#include "PeripheralInfo.h"
using namespace K60;



const IRQn_Type DMA_INFO<DMA_BASE>::TransferCompleteIRQ[] = 
{
	DMA0_IRQn,
	DMA1_IRQn,
	DMA2_IRQn,
	DMA3_IRQn,
	DMA4_IRQn,
	DMA5_IRQn,
	DMA6_IRQn,
	DMA7_IRQn,
	DMA8_IRQn,
	DMA9_IRQn,
	DMA10_IRQn,
	DMA11_IRQn,
	DMA12_IRQn,
	DMA13_IRQn,
	DMA14_IRQn,
	DMA15_IRQn
};

volatile UINT16 DMA_INFO<DMA_BASE>::ChannelIdleFlagStatus = 0xFFFF;
adxM3::Function< void() > DMA_INFO<DMA_BASE>::transferCompleteCBK[];
volatile UINT16 DMA_INFO<DMA_BASE>::ChannelTCEventMask = 0xFFFF;
void* DMA_INFO<DMA_BASE>::TransferSourceBuffer[] = 
{
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL
};


const IRQn_Type PIT_INFO<PIT_BASE>::ChannelIQR[] = 
{ 
	PIT0_IRQn, PIT1_IRQn, PIT2_IRQn, PIT3_IRQn
};
const UINT32 PIT_INFO<PIT_BASE>::InteruptPriority[] = 
{
	6, 8, 7, 9
};



void* EXTI_INFO<PTA_BASE>::InstancePtr[];
void* EXTI_INFO<PTB_BASE>::InstancePtr[];
void* EXTI_INFO<PTC_BASE>::InstancePtr[];
void* EXTI_INFO<PTD_BASE>::InstancePtr[];
void* EXTI_INFO<PTE_BASE>::InstancePtr[];

adxM3::Function< void(void*) > EXTI_INFO<PTA_BASE>::ExtPinLevelRisingCBK[];
adxM3::Function< void(void*) > EXTI_INFO<PTB_BASE>::ExtPinLevelRisingCBK[];
adxM3::Function< void(void*) > EXTI_INFO<PTC_BASE>::ExtPinLevelRisingCBK[];
adxM3::Function< void(void*) > EXTI_INFO<PTD_BASE>::ExtPinLevelRisingCBK[];
adxM3::Function< void(void*) > EXTI_INFO<PTE_BASE>::ExtPinLevelRisingCBK[];

adxM3::Function< void(void*) > EXTI_INFO<PTA_BASE>::ExtPinLevelFallingCBK[];
adxM3::Function< void(void*) > EXTI_INFO<PTB_BASE>::ExtPinLevelFallingCBK[];
adxM3::Function< void(void*) > EXTI_INFO<PTC_BASE>::ExtPinLevelFallingCBK[];
adxM3::Function< void(void*) > EXTI_INFO<PTD_BASE>::ExtPinLevelFallingCBK[];
adxM3::Function< void(void*) > EXTI_INFO<PTE_BASE>::ExtPinLevelFallingCBK[];




//////////////////////////////////////////////////////////////////////////
//	Instance Pointer static initializing here
//////////////////////////////////////////////////////////////////////////

void* UART_INFO<UART0_BASE>::InstancePtr = NULL;
void* UART_INFO<UART1_BASE>::InstancePtr = NULL;
void* UART_INFO<UART2_BASE>::InstancePtr = NULL;
void* UART_INFO<UART3_BASE>::InstancePtr = NULL;
void* UART_INFO<UART4_BASE>::InstancePtr = NULL;
void* UART_INFO<UART5_BASE>::InstancePtr = NULL;

void* PIT_INFO<PIT_BASE>::InstancePtr[] =
{
	NULL, NULL, NULL, NULL
};

void* FTM_INFO<FTM0_BASE>::InstancePtr = NULL;
void* FTM_INFO<FTM1_BASE>::InstancePtr = NULL;
void* FTM_INFO<FTM2_BASE>::InstancePtr = NULL;

void* LPTMR_INFO<LPTMR0_BASE>::InstancePtr = NULL;


void* SPI_INFO<SPI0_BASE>::InstancePtr = NULL;
void* SPI_INFO<SPI1_BASE>::InstancePtr = NULL;
void* SPI_INFO<SPI2_BASE>::InstancePtr = NULL;


