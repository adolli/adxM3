
#ifndef _ADX_CM_DISPLAYER_H_
#define _ADX_CM_DISPLAYER_H_

#include "../HadwareAbstraction/GDI/GraphicsDevice.h"

namespace adxM3
{

	template< class ioAdapter >
	class Displayer
		: public ioAdapter
	{
	public:


		Displayer()
			: ioAdapter()
		{
		}


		enum MapMode
		{
			MM_L2R_U2D					= 0,	
			MM_L2R_D2U					= 1,	
			MM_R2L_U2D					= 2,	
			MM_R2L_D2U					= 3,	
			MM_U2D_L2R					= 4,	
			MM_U2D_R2L					= 5,	
			MM_D2U_L2R					= 6,	
			MM_D2U_R2L					= 7,	
			MM_DFT						= MM_L2R_U2D
		};

		void SetMapMode(MapMode mm)
		{
			SetScanDir(typename ioAdapter::SCANDIR(mm));
		}

	protected:

		using ioAdapter::SetScanDir;
		

	};

}


#endif  /*_ADX_CM_DISPLAYER_H_*/
