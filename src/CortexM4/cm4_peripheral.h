
#ifndef _CM4_CM4PERIPHERAL_H_
#define _CM4_CM4PERIPHERAL_H_

#include "adxM3.h"


/* ----------------------------------------------------------------------------
   -- Cortex M4 Core Configuration
   ---------------------------------------------------------------------------- */

/*! \addtogroup Cortex_Core_Configuration Cortex M4 Core Configuration */
/*! \{ */

#define __MPU_PRESENT                  0         /*!< Defines if an MPU is present or not */
#define __NVIC_PRIO_BITS               4         /*!< Number of priority bits implemented in the NVIC */
#define __Vendor_SysTickConfig         0         /*!< Vendor specific implementation of SysTickConfig is defined */

#include "core_cm4.h"                  /* Core Peripheral Access Layer */


/*! \} */ /* end of group Cortex_Core_Configuration */




#endif  /*_CM4_CM4PERIPHERAL_H_*/
