
#ifndef _ADX_CM_HCTL20XX_H_
#define _ADX_CM_HCTL20XX_H_

#include "../HadwareAbstraction/GPIO/GeneralPurposeInputOutputPin.h"

namespace adxM3
{


	/*
	 * @brief	HCTL20系列正交解码器抽象
	 * @tparam	DataPort 8位并行数据端口
	 *			SelctionPort 2位选择信号数据端口
	 *			OutputEnableSignal 输出使能信号，active low
	 *			ResetCounterSignal 复位计数器信号，active low
	 */
	template
	<
		class DataPort,
		class SelctionPort,
		class OutputEnableSignal,
		class ResetCounterSignal
	>
	class HCTL20xx
	{
	protected:


		/*
		 * @required	DataPort为8位数据端口，要求实现以下方法
		 *				GetValue 读端口值
		 */
		/*using DataPort::GetValue;


		using SelctionPort::operator=;
		using OutputEnableSignal::Set;
		using ResetCounterSignal::Set;
		using OutputEnableSignal::Reset;
		using ResetCounterSignal::Reset;*/



		/*
		 * @required	要求实现端口IO配置系列方法
		 */
		/*using SelctionPort::SetDirection;
		using SelctionPort::OutputConfig;
		using OutputEnableSignal::SetDirection;
		using OutputEnableSignal::OutputConfig;
		using ResetCounterSignal::SetDirection;
		using ResetCounterSignal::OutputConfig;*/



	public:

		HCTL20xx()
		{
			dataPort.SetDirection(GeneralPurposeInputOutput::IN);
			dataPort.InputConfig(GeneralPurposeInputOutput::Floating);

			// HCTL为5V CMOS电平，3.3V MCU必须外接上拉到5V的点做作电平变换
			// 控制信号输出均为开漏
			selPort.SetDirection(GeneralPurposeInputOutput::OUT);
			selPort.OutputConfig(GeneralPurposeInputOutput::OpenDrain);

			OE.SetDirection(GeneralPurposeInputOutput::OUT);
			OE.OutputConfig(GeneralPurposeInputOutput::OpenDrain);
			OE = 1;

			RST.SetDirection(GeneralPurposeInputOutput::OUT);
			RST.OutputConfig(GeneralPurposeInputOutput::OpenDrain);
			RST = 0;
			RST = 1;
		}





		typedef INT32 CounterValueType;



		/*
		 * @brief	读计数器值
		 * @retval	核心计数单元的计数值
		 */
		CounterValueType GetCounter()
		{
			WordStruct sword;

			selPort = 2;
			selPort = 2;

			OE = 0;
			OE = 0;


			// MSB
			sword.byte[3] = dataPort.GetValue();
			sword.byte[3] = dataPort.GetValue();
			sword.byte[3] = dataPort.GetValue();
			sword.byte[3] = dataPort.GetValue();
			sword.byte[3] = dataPort.GetValue();
			sword.byte[3] = dataPort.GetValue();

			// 2nd
			selPort = 3;
			selPort = 3;
			sword.byte[2] = dataPort.GetValue();
			sword.byte[2] = dataPort.GetValue();
			sword.byte[2] = dataPort.GetValue();
			sword.byte[2] = dataPort.GetValue();
			sword.byte[2] = dataPort.GetValue();
			sword.byte[2] = dataPort.GetValue();

			// 3rd byte
			selPort = 0;
			selPort = 0;
			sword.byte[1] = dataPort.GetValue();
			sword.byte[1] = dataPort.GetValue();
			sword.byte[1] = dataPort.GetValue();
			sword.byte[1] = dataPort.GetValue();
			sword.byte[1] = dataPort.GetValue();
			sword.byte[1] = dataPort.GetValue();

			// LSB
			selPort = 1;
			selPort = 1;
			sword.byte[0] = dataPort.GetValue();
			sword.byte[0] = dataPort.GetValue();
			sword.byte[0] = dataPort.GetValue();
			sword.byte[0] = dataPort.GetValue();
			sword.byte[0] = dataPort.GetValue();
			sword.byte[0] = dataPort.GetValue();

			OE = 1;
			OE = 1;

			return sword.word;
		}





		/*
		 * @brief	复位计数器
		 */
		void ResetCounter()
		{
			RST = 0;
			RST = 0;
			RST = 1;
		}




	protected:


		DataPort dataPort;
		SelctionPort selPort;
		OutputEnableSignal OE;
		ResetCounterSignal RST;

	};


}


#endif  /*_ADX_CM_HCTL20XX_H_*/
