
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
		 * @brief	�ر���
		 * @notice	�ں������������������û�д˹���ֱ�ӷ���true����
		 */
		virtual bool Close() = 0;
		
	};


}

#endif  /*_STREAMBASE_H_*/
