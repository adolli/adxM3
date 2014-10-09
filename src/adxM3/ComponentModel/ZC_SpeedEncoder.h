
#ifndef _ADX_CM_ZCSPEEDENCODER_H_
#define _ADX_CM_ZCSPEEDENCODER_H_

namespace adxM3
{

	/*
	 * @brief	չ���Զ�����˾���ľ���ʽ����������
	 * @tparam	PPR ÿȦ������
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
		 * @brief	��������ȡ��ǰ����ֵ
		 * @retval	����ʽ������ֱ�ӷ��ؽ�λ����
		 */
		CounterValueType GetCounter()
		{
			CounterValueType val = 0;
			SSI.Receive(val);

			// ��6λΪ��Ч����
			return val >> 6;
		}




	protected:

		SynchronousSerialInterface SSI;		

	};

}

#endif  /*_ADX_CM_ZCSPEEDENCODER_H_*/
