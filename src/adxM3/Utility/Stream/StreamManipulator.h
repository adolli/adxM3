
#ifndef _STREAMMANIPULATOR_H_
#define _STREAMMANIPULATOR_H_

#include "InputStream.h"
#include "OutputStream.h"
#include "../../common/Function.h"


namespace adxM3
{

	template< typename _io_DeviceT, template <class> class StreamType >
	class StreamManipulator : public Function< void(StreamType<_io_DeviceT>&) >
	{
	public:

		virtual void operator()(StreamType<_io_DeviceT>& stream) = 0;

	};



	template< typename _io_DeviceT, template <class> class StreamType >
	class _ManipEndl : public StreamManipulator<_io_DeviceT, StreamType>
	{
	public:

		virtual void operator()(StreamType<_io_DeviceT>& stream)
		{
			stream.Write('\r');
			stream.Write('\n');
		}

	};

	/*
	class _ManipToHex : public StreamManipulator
	{
	public:

		

	};*/

}


#endif  /*_STREAMMANIPULATOR_H_*/
