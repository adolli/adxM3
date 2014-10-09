

#ifndef _CM4_NVIC_H_
#define _CM4_NVIC_H_


/* ----------------------------------------------------------------------------
   -- Interrupt vector numbers
   ---------------------------------------------------------------------------- */

/*! \addtogroup Interrupt_vector_numbers Interrupt vector numbers */
/*! \{ */

#include "irqchx.h"

/*! \} */ /* end of group Interrupt_vector_numbers */


#include "cm4_peripheral.h"


namespace CM4
{



	/*
	 * @brief	Ƕ�������жϿ�����
	 */
	class $NVIC
	{
	public:




		/* 
		 * @group	NVIC methods
		 * @brief	��core-cm4�й���NVIC���õķ������ɵ�����
		 */
		static void SetPriority(IRQn_Type IRQn, uint32_t priority)
		{
			NVIC_SetPriority(IRQn, priority);
		}

		static void EnableIRQ(IRQn_Type IRQn)
		{
			NVIC_EnableIRQ(IRQn);
		}

		static void DisableIRQ(IRQn_Type IRQn)
		{
			NVIC_DisableIRQ(IRQn);
		}

	};

}


#endif  /*_CM4_NVIC_H_*/

