
#ifndef _CM4_SYSTICK_H_
#define _CM4_SYSTICK_H_

#include "cm4_peripheral.h"

namespace CM4
{

	class $SysTick
	{
	public:


		static void WaitTicks(UINT32 ticks)
		{
			SysTick->LOAD  = (ticks & SysTick_LOAD_RELOAD_Msk) - 1;      /* set reload register */
			SysTick->VAL   = 0;                                          /* Load the SysTick Counter Value */
			SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
							 SysTick_CTRL_ENABLE_Msk;                    /* Enable SysTick IRQ and SysTick Timer */
			UINT32 temp;	    	 
			do 
			{
				temp = SysTick->CTRL;
			} while((temp & 0x01) && !(temp & _BV(16)));	// �ȴ�ʱ�䵽��   
			SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;		// �رռ�����	
		}

	};
}

#endif  /*_CM4_SYSTICK_H_*/
