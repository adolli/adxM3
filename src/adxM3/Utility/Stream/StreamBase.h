
#ifndef _STREAMBASE_H_
#define _STREAMBASE_H_


namespace adxM3
{
	
	class StreamBase
	{
	protected:
		virtual ~StreamBase() {}

	public:

		typedef unsigned int size_type;

		/*
		 * @brief	关闭流
		 * @notice	在后续派生的流类中如果没有此功能直接返回true即可
		 */
		virtual bool Close() = 0;
		
	};


}

#endif  /*_STREAMBASE_H_*/
