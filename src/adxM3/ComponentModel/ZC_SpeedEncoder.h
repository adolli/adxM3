
#ifndef _ADX_CM_ZCSPEEDENCODER_H_
#define _ADX_CM_ZCSPEEDENCODER_H_

namespace adxM3
{

	/*
	 * @brief	展昌自动化公司出的绝对式编码器抽象
	 * @tparam	PPR 每圈多少线
	 */
	template
	<
		UINT32 PPR,
		class SynchronousSerialInterface
	>
	class ZC_AbsSpeedEncoder
	{
	public:


		enum { CounterMaxValue = PPR };

		typedef UINT32 CounterValueType;

		ZC_AbsSpeedEncoder()
			: SSI(18)
		{
		}



		/*
		 * @brief	编码器读取当前计数值
		 * @retval	绝对式编码器直接返回角位移量
		 */
		CounterValueType GetCounter()
		{
			CounterValueType val = 0;
			SSI.Receive(val);

			// 低6位为无效数据
			return val >> 6;
		}




	protected:

		SynchronousSerialInterface SSI;		

	};

}

#endif  /*_ADX_CM_ZCSPEEDENCODER_H_*/
