
#ifndef _ADX_HA_EXTERNALINTERRUPT_H_
#define _ADX_HA_EXTERNALINTERRUPT_H_

#include "../Event/EventObject.h"


namespace adxM3
{


	class ExternalInterrupt	: public EventObject
	{
	public:


		enum EVT
		{
			LEVEL_RISING,
			LEVEL_FALLING,
			LEVEL_TOGGLE,
			LEVEL_LOGIC_ONE,
			LEVEL_LOGIC_ZERO,
		};



		virtual ~ExternalInterrupt() {}

	};

}
#endif  /*_ADX_HA_EXTERNALINTERRUPT_H_*/
