
#ifndef _ADX_K60_CLOCKDISTRIBUTOR_H_
#define _ADX_K60_CLOCKDISTRIBUTOR_H_

#include "adxM3.h"


namespace K60
{


	template< typename CLOCK >
	class ClockDistributor
	{
	public:

		enum
		{
			Frequency = CLOCK::Frequency
		};

	};

}

#endif  /*_ADX_K60_CLOCKDISTRIBUTOR_H_*/
