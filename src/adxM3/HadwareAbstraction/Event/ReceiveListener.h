
#ifndef _ADX_HA_RECEIVELISTENER_H_
#define _ADX_HA_RECEIVELISTENER_H_

namespace adxM3
{

	template< typename DataType >
	class ReceiveListener
	{
	public:

		typedef void ReturnType;
		typedef void* ContextType;
		typedef DataType ParamType;

		virtual ReturnType OnReceived(ContextType context, DataType data)
		{
			// to be overrided
		}

	};
}

#endif  /*_ADX_HA_RECEIVELISTENER_H_*/
