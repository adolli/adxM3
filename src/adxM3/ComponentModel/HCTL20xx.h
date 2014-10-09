
#ifndef _ADX_CM_HCTL20XX_H_
#define _ADX_CM_HCTL20XX_H_

#include "../HadwareAbstraction/GPIO/GeneralPurposeInputOutputPin.h"

namespace adxM3
{


	/*
	 * @brief	HCTL20ϵ����������������
	 * @tparam	DataPort 8λ�������ݶ˿�
	 *			SelctionPort 2λѡ���ź����ݶ˿�
	 *			OutputEnableSignal ���ʹ���źţ�active low
	 *			ResetCounterSignal ��λ�������źţ�active low
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
		 * @required	DataPortΪ8λ���ݶ˿ڣ�Ҫ��ʵ�����·���
		 *				GetValue ���˿�ֵ
		 */
		/*using DataPort::GetValue;


		using SelctionPort::operator=;
		using OutputEnableSignal::Set;
		using ResetCounterSignal::Set;
		using OutputEnableSignal::Reset;
		using ResetCounterSignal::Reset;*/



		/*
		 * @required	Ҫ��ʵ�ֶ˿�IO����ϵ�з���
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

			// HCTLΪ5V CMOS��ƽ��3.3V MCU�������������5V�ĵ�������ƽ�任
			// �����ź������Ϊ��©
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
		 * @brief	��������ֵ
		 * @retval	���ļ�����Ԫ�ļ���ֵ
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
		 * @brief	��λ������
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
