
#ifndef _ADX_K60_SYSTEM_H_
#define _ADX_K60_SYSTEM_H_

#include "adxM3.h"
#include "../Peripherals/Clock/ClockDistributor.h"
#include "../Peripherals/PeripheralInfo.h"
#include "../../../CortexM4/NVIC.h"
#include "../../../CortexM4/SysTick.h"
#include "Flash.h"

namespace K60
{


	// @defgroup Preemption_Priority_Group 
	#define NVIC_PriorityGroup_0			((uint32_t)0x700) /*!<	0 bits for pre-emption priority
																	4 bits for subpriority */
	#define NVIC_PriorityGroup_1			((uint32_t)0x600) /*!<	1 bits for pre-emption priority
																	3 bits for subpriority */
	#define NVIC_PriorityGroup_2			((uint32_t)0x500) /*!<	2 bits for pre-emption priority
																	2 bits for subpriority */
	#define NVIC_PriorityGroup_3			((uint32_t)0x400) /*!<	3 bits for pre-emption priority
																	1 bits for subpriority */
	#define NVIC_PriorityGroup_4			((uint32_t)0x300) /*!<	4 bits for pre-emption priority
																	0 bits for subpriority */



	/*
	 * @brief	K60ϵͳ�࣬������ļ��������
	 *			���붨���Signleton���͡�
	 */
	class SystemType
		: public adxM3::BlockingDelayController
	{
	public:

		/*
		 * @brief	ϵͳ��׼���캯��
		 *			low level��ʼ����ִ�еĳ�ʼ������
		 */
		SystemType()
		{
			WatchingDogConfig(10);

			NVIC_SetPriorityGrouping(NVIC_PriorityGroup_4);
			__enable_irq();
			Flash::_Init();
		}








		/*
		 * @brief	���ÿ��Ź�
		 * @param	time_ms ���Ź���ʱʱ��
		 */
		static void WatchingDogConfig(UINT32 time_ms)
		{
			if (time_ms < 4)
			{
				return;
			}

			// �������Ź�
			UnlockWatchingDog();

			WDOG->PRESC = WDOG_PRESC_PRESCVAL(0);            // ���÷�Ƶϵ�� = PRESCVAL +1(PRESCVALȡֵ��ΧΪ0~7)

			WDOG->TOVALH = time_ms >> 16;                    // ����ι��ʱ��
			WDOG->TOVALL = (UINT16)time_ms;

			// ѡ��LPO��Ϊʱ��
			WDOG->STCTRLH &= ~_BV(1);

			WDOG->STCTRLH &= ~WDOG_STCTRLH_WDOGEN_MASK;
		}



		/*
		 * @brief	ʹ��/��ֹ���Ź�
		 */
		static void DisableWatchingDog()
		{
			UnlockWatchingDog();                            // �Ƚ���������
			WDOG->STCTRLH &= ~WDOG_STCTRLH_WDOGEN_MASK;
		}
		static void EnableWatchingDog()
		{
			UnlockWatchingDog();                            // �Ƚ���������
			WDOG->STCTRLH |= WDOG_STCTRLH_WDOGEN_MASK;
		}




		/*
		 * @brief	ι���Ź�
		 */
		static void FeedWatchingDog()
		{
			// �����ȹر����жϺ����ι��������һ��������жϴ������ι��ʧ��
			__disable_irq();
			WDOG->REFRESH = 0xA602;
			WDOG->REFRESH = 0xB480;
			__enable_irq();
		}





		typedef ClockDistributor<CoreClock> SysTickClock;

		enum { TicksPerUS = SysTickClock::Frequency / 1000000 };


		virtual void Delay_ns(unsigned int ns) const
		{
			CM4::$SysTick::WaitTicks(ns * TicksPerUS / 1000);
		}

		virtual void Delay_us(unsigned int us) const
		{
			CM4::$SysTick::WaitTicks(us * TicksPerUS);
		}

		virtual void Delay_ms(unsigned int ms) const
		{
			while (ms--)
			{
				Delay_us(1000);
			}
		}


		static void StaticDelay_ns(unsigned int ns)
		{
			CM4::$SysTick::WaitTicks(ns * TicksPerUS / 1000);
		}
		static void StaticDelay_us(unsigned int us)
		{
			CM4::$SysTick::WaitTicks(us * TicksPerUS);
		}
		static void StaticDelay_ms(unsigned int ms)
		{
			while (ms--)
			{
				StaticDelay_us(1000);
			}
		}



	private:


		/*
		 * @brief	�������Ź���ʹ�ø÷���ʱ�벻Ҫ�����ж�
		 *			�÷���������20��������������ɷ������ʧ��
		 */
		static void UnlockWatchingDog()
		{
			__disable_irq();
			WDOG->UNLOCK = 0xC520;
			WDOG->UNLOCK = 0xD928;
			__enable_irq();
		}

	};


}

extern K60::SystemType System;

#endif  /*_ADX_K60_SYSTEM_H_*/
