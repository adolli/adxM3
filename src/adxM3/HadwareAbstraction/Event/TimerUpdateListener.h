
#ifndef _ADX_HA_TIMERUPDATELISTENER_H_
#define _ADX_HA_TIMERUPDATELISTENER_H_

namespace adxM3
{
	class TimerUpdateListener
	{
	public:

		typedef void ReturnType;
		typedef void* ContextType;
		typedef void ParamType;

		virtual ReturnType OnUpdate(ContextType context)
		{
			// to be overrided
		}
	};

}

#endif  /*_ADX_HA_TIMERUPDATELISTENER_H_*/
