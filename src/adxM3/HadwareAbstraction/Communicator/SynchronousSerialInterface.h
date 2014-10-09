
#ifndef _ADX_HA_SynchronousSerialInterface_H_
#define _ADX_HA_SynchronousSerialInterface_H_

#include "../GPIO/GeneralPurposeInputOutputPin.h"
#include "../Timer/BlockingDelayController.h"

namespace adxM3
{


	template< class T >
	struct _CSn
	{
		typedef T Result;
	};

	template< class T >
	struct _CLK
	{
		typedef T Result;
	};

	template< class T >
	struct _DAT
	{
		typedef T Result;
	};



	template< class, class, class, class > class SynchronousSerialInterface;



	/*
	 * @breif	����ͬ���ӿ�
	 *			���ģ��ʵ��
	 * @tparam	CSnSignal GPIOPin���� OUT
	 *			CLKSignal GPIOPin���� OUT
	 *			DATSignal GPIOPin���� IN
	 *				������������Ҫ��ʵ�����·���
	 *				@required SetDirection
	 *				@required OutputConfig
	 *				@required InputConfig
	 *				@required operator=
	 *				@required GetLevel
	 *			DataBits ��֡��������λ��
	 */
	template
	<
		class CSnSignal,
		class CLKSignal,
		class DATSignal,
		class SystemType
	>
	class SynchronousSerialInterface
	<
		_CSn<CSnSignal>,
		_CLK<CLKSignal>,
		_DAT<DATSignal>,
		SystemType
	>
	{
	public:



		SynchronousSerialInterface(BYTE databits)
			: dataBits(databits)
		{
			CSn.OutputConfig(GeneralPurposeInputOutput::PushPull, GeneralPurposeInputOutput::HighSpeed);
			CLK.OutputConfig(GeneralPurposeInputOutput::PushPull, GeneralPurposeInputOutput::HighSpeed);
			DAT.InputConfig(GeneralPurposeInputOutput::PullUp);
		}




		/*
		 * @brief	SSI�ӿڽ��շ���
		 * @param	in ���յ����ݣ�����������DataType����
		 */
		virtual int Receive(UINT32& in)
		{
			in = 0;
			CSn = 0;			
			SystemType::StaticDelay_ns(500);				// Tclk_fe == 500 ns
			for(int i = 0; i < dataBits; ++i)
			{ 
				CLK = 0;
				SystemType::StaticDelay_ns(500);			// Tclk_/2 == 500 ns
				CLK = 1;
				SystemType::StaticDelay_ns(500);			// Tdo_valid == 413 ns
				in <<= 1;
				in |= DAT;
			}
			CSn = 1;
			return 1;
		}




		virtual void SetDataBits(const BYTE databits)
		{
			dataBits = databits;
		}
		virtual BYTE GetDataBits() const
		{
			return dataBits;
		}



	protected:

		CSnSignal CSn;
		CLKSignal CLK;
		DATSignal DAT;

		BYTE dataBits;

	};
}

#endif  /*_ADX_HA_SynchronousSerialInterface_H_*/
