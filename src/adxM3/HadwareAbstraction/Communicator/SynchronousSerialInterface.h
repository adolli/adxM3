
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
	 * @breif	串行同步接口
	 *			软件模拟实现
	 * @tparam	CSnSignal GPIOPin类型 OUT
	 *			CLKSignal GPIOPin类型 OUT
	 *			DATSignal GPIOPin类型 IN
	 *				以上三个参数要求实现以下方法
	 *				@required SetDirection
	 *				@required OutputConfig
	 *				@required InputConfig
	 *				@required operator=
	 *				@required GetLevel
	 *			DataBits 单帧传输数据位数
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
		 * @brief	SSI接口接收方法
		 * @param	in 接收的数据，数据类型由DataType决定
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
