
#include "EXTI.h"
using namespace adxM3;
using namespace K60;


#define PORTx_IRQH(x)															\
void PORT ## x ## _IRQHandler(void)												\
{																				\
	/* 进入中断立刻保存端口电平 */												\
	/* 以免在后续检测中电平发生变化而进入到错误的回调函数 */					\
	UINT32 PortInputValue = EXTI ## x::BaseAccess()->PDIR;						\
	PortInputValue = EXTI ## x::BaseAccess()->PDIR;								\
																				\
	for (int i = 0; i < EXTI ## x::GlobalPortInfo::PIN_MAX; ++i)				\
	{																			\
		if (EXTI ## x::GlobalPortInfo::BitBandAccess()->ISFR[i] == SET)			\
		{																		\
			/* 区分上升沿与下降沿 */											\
			if (PortInputValue & _BV(i))										\
			{																	\
				/* 高电平处之前必为上升沿 */									\
				EXTI ## x::ExtPinLevelRisingCBK[i](EXTI ## x::InstancePtr[i]);	\
			}																	\
			else																\
			{																	\
				EXTI ## x::ExtPinLevelFallingCBK[i](EXTI ## x::InstancePtr[i]);	\
			}																	\
																				\
			/* 清除中断标记 */													\
			EXTI ## x::GlobalPortInfo::BitBandAccess()->ISFR[i] = 1;			\
		}																		\
	}																			\
}


PORTx_IRQH(A)
PORTx_IRQH(B)
PORTx_IRQH(C)
PORTx_IRQH(D)
PORTx_IRQH(E)

