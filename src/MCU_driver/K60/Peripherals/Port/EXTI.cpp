
#include "EXTI.h"
using namespace adxM3;
using namespace K60;


#define PORTx_IRQH(x)															\
void PORT ## x ## _IRQHandler(void)												\
{																				\
	/* �����ж����̱���˿ڵ�ƽ */												\
	/* �����ں�������е�ƽ�����仯�����뵽����Ļص����� */					\
	UINT32 PortInputValue = EXTI ## x::BaseAccess()->PDIR;						\
	PortInputValue = EXTI ## x::BaseAccess()->PDIR;								\
																				\
	for (int i = 0; i < EXTI ## x::GlobalPortInfo::PIN_MAX; ++i)				\
	{																			\
		if (EXTI ## x::GlobalPortInfo::BitBandAccess()->ISFR[i] == SET)			\
		{																		\
			/* �������������½��� */											\
			if (PortInputValue & _BV(i))										\
			{																	\
				/* �ߵ�ƽ��֮ǰ��Ϊ������ */									\
				EXTI ## x::ExtPinLevelRisingCBK[i](EXTI ## x::InstancePtr[i]);	\
			}																	\
			else																\
			{																	\
				EXTI ## x::ExtPinLevelFallingCBK[i](EXTI ## x::InstancePtr[i]);	\
			}																	\
																				\
			/* ����жϱ�� */													\
			EXTI ## x::GlobalPortInfo::BitBandAccess()->ISFR[i] = 1;			\
		}																		\
	}																			\
}


PORTx_IRQH(A)
PORTx_IRQH(B)
PORTx_IRQH(C)
PORTx_IRQH(D)
PORTx_IRQH(E)

