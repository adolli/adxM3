
#include "DMA.h"
using namespace K60;
using namespace adxM3;


#define DMAx_IRQH(x)																		\
void DMA ## x ## _IRQHandler(void)															\
{																							\
	enum { CH = x };																		\
																							\
	typedef ClockGateController<DMA::ClockSourceGate> ClockSource;							\
	typedef ClockGateController<DMA::MultiplexerClockSourceGate> MultiplexerClockSource;	\
																							\
	/* 传输完成事件处理 */																	\
	if ((~DMA::ChannelTCEventMask) & _BV(CH))												\
	{																						\
		DMA::transferCompleteCBK[CH]();														\
																							\
		/* 注销函数并屏蔽 */																\
		DMA::transferCompleteCBK[CH] = 0;													\
		DMA::ChannelTCEventMask |= _BV(CH);													\
	}																						\
																							\
	/* 清除传输完成标志位 */																\
	DMA::BitBandAccess()->INT[CH] = 1;														\
																							\
	/* 清除ELINK使能位 */																	\
	/*DMA::BaseAccess()->TCD[CH].CSR.MAJORELINK = RESET;*/										\
																							\
	/* 关闭时钟，当全部没有通道正在工作时才会真正关闭时钟 */								\
	MultiplexerClockSource::ClockOff();														\
	ClockSource::ClockOff();																\
	DMA::ChannelIdleFlagStatus |= _BV(CH);													\
	if (DMA::TransferSourceBuffer[CH])														\
	{																						\
		/* 释放传输所用缓冲区 */															\
		delete [] DMA::TransferSourceBuffer[CH];											\
		DMA::TransferSourceBuffer[CH] = NULL;												\
	}																						\
}

DMAx_IRQH(0)
DMAx_IRQH(1)
DMAx_IRQH(2)
DMAx_IRQH(3)
DMAx_IRQH(4)
DMAx_IRQH(5)
DMAx_IRQH(6)
DMAx_IRQH(7)
DMAx_IRQH(8)
DMAx_IRQH(9)
DMAx_IRQH(10)
DMAx_IRQH(11)
DMAx_IRQH(12)
DMAx_IRQH(13)
DMAx_IRQH(14)
DMAx_IRQH(15)


void DMA_Error_IRQHandler(void)
{

}
