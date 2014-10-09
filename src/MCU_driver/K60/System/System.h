
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
	 * @brief	K60系统类，处理核心级别的事物
	 *			【请定义成Signleton类型】
	 */
	class SystemType
		: public adxM3::BlockingDelayController
	{
	public:

		/*
		 * @brief	系统标准构造函数
		 *			low level初始化后执行的初始化任务
		 */
		SystemType()
		{
			WatchingDogConfig(10);

			NVIC_SetPriorityGrouping(NVIC_PriorityGroup_4);
			__enable_irq();
			Flash::_Init();
		}








		/*
		 * @brief	配置看门狗
		 * @param	time_ms 看门狗定时时长
		 */
		static void WatchingDogConfig(UINT32 time_ms)
		{
			if (time_ms < 4)
			{
				return;
			}

			// 解锁看门狗
			UnlockWatchingDog();

			WDOG->PRESC = WDOG_PRESC_PRESCVAL(0);            // 设置分频系数 = PRESCVAL +1(PRESCVAL取值范围为0~7)

			WDOG->TOVALH = time_ms >> 16;                    // 设置喂狗时间
			WDOG->TOVALL = (UINT16)time_ms;

			// 选择LPO作为时钟
			WDOG->STCTRLH &= ~_BV(1);

			WDOG->STCTRLH &= ~WDOG_STCTRLH_WDOGEN_MASK;
		}



		/*
		 * @brief	使能/禁止看门狗
		 */
		static void DisableWatchingDog()
		{
			UnlockWatchingDog();                            // 先解锁后配置
			WDOG->STCTRLH &= ~WDOG_STCTRLH_WDOGEN_MASK;
		}
		static void EnableWatchingDog()
		{
			UnlockWatchingDog();                            // 先解锁后配置
			WDOG->STCTRLH |= WDOG_STCTRLH_WDOGEN_MASK;
		}




		/*
		 * @brief	喂看门狗
		 */
		static void FeedWatchingDog()
		{
			// 必须先关闭总中断后才能喂狗，否则一旦被别的中断打断立刻喂狗失败
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
		 * @brief	解锁看门狗，使用该方法时请不要打开总中断
		 *			该方法必须在20个总线周期内完成否则解锁失败
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
