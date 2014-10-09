
#include "PIT.h"
using namespace K60;

#define PITx_IRQ(x)																		\
void PIT ##x## _IRQHandler(void)														\
{																						\
	Peripheral<PIT>::CH< x >& InstanceRef												\
		= *static_cast<Peripheral<PIT>::CH< x >*>(PIT::InstancePtr[ x ]);				\
	InstanceRef.updateListener.GetEventListener().OnUpdate(&InstanceRef);				\
																						\
	if (PIT::BaseAccess()->CHANNEL[ x ].LDVAL == 0)										\
	{																					\
		/* ����Զ���װֵΪ0�����Զ���ֹ��ʱͨ�� */										\
		PIT::BitBandAccess()->CHANNEL[ x ].TCTRL[PIT_BitBandType::TCTRL_TEN] = RESET;	\
																						\
		/* Ȼ����Զ���װֵ�ָ� */														\
		InstanceRef.UpdateAutoReloadValue();											\
	}																					\
																						\
	/* д1����ñ�־λ */																\
	PIT::BitBandAccess()->CHANNEL[ x ].TFLG[PIT_BitBandType::TFLG_TIF] = SET;			\
}

PITx_IRQ(0)
PITx_IRQ(1)
PITx_IRQ(2)
PITx_IRQ(3)
