
#ifndef _ADX_K60_PULSEWIDTHGENERATOR_H_
#define _ADX_K60_PULSEWIDTHGENERATOR_H_

#include "../../common/defs.h"
#include "../GPIO/GeneralPurposeInputOutputPin.h"

namespace adxM3
{



	/*
	 * @brief	�ɱ�̿�����巢���� Programmable Width Pulse Generator
	 * @tparam	TimerType ��ʱ�����ͣ�Ҫ��ʵ�����·�������using TimerType::xxx��
	 *			PinType �������ͣ�Ҫ������ʵ�����·���
	 *				@required Setlevel ���õ�ƽ
	 *				@required Toggle ��ת��ƽ
	 */
	template
	< 
		class TimerType, 
		class PinType
	>
	class PulseWidthGenerator
		: public TimerType
	{

	protected:

		using TimerType::AddEventListener;
		using TimerType::RemoveEventListener;
		using TimerType::StartOnce;
		using TimerType::SetTimeOut;
		using TimerType::TimerIsRunning;

	public:


	

		/*
		 * @brief	���弫��ѡ��
		 */
		enum PT
		{
			PositivePulse,
			NegativePulse
		};
	
		
		
		
		PulseWidthGenerator(PT pol = PositivePulse)
			: enable(false), lastPulseWidth(0)
		{
			pin.SetDirection(GeneralPurposeInputOutput::OUT);
			pin.OutputConfig(GeneralPurposeInputOutput::PushPull, GeneralPurposeInputOutput::UltraHighSpeed);
			pin.SetDriveStrength(GeneralPurposeInputOutput::HighStrength);
			SetPulsePolarity(pol);

			struct TimerEventListener
			{
				static void OnTimeout(Timer* _This)
				{
					PulseWidthGenerator* This = static_cast<PulseWidthGenerator*>(_This);
					This->pin.Toggle();
				}
			};
			AddEventListener(Timer::TIME_OUT, TimerEventListener::OnTimeout);
		}



		



		/*
		 * @brief	�������弫��
		 * @param	pol ���ԣ�PT������PTö���е��κ�һ��
		 *				@arg PositivePulse ��������ʱ��ƽΪ1������ʱ��ƽΪ0
		 *				@arg NegativePulse ��������ʱ��ƽΪ0������ʱ��ƽΪ1
		 */
		void SetPulsePolarity(PT pol)
		{
			STATIC_ASSERT(PositivePulse == 0 && NegativePulse == 1,
				enumerate_value_is_not_campacted_with_this_configuration);

			// �������GPIO����ʱΪ�͵�ƽ
			pin.SetLevel(pol);
		}





		/*
		 * @brief	��������
		 * @param	width �����ȣ���λus
		 * @retval	�ɹ���������1�����򷵻�0����ǰһ��������δ��������Ϊ����ʧ��
		 */
		int GeneratePulse(UINT32 width)
		{
			if (TimerIsRunning() || !enable)
			{
				return 0;
			}

			lastPulseWidth = width;
			SetTimeOut(width);
			pin.Toggle();
			StartOnce();
			return 1;
		}



		/*
		 * @brief	��ȡ�ϴβ���������
		 */
		UINT32 GetLastPulseWidth() const
		{
			return lastPulseWidth;
		}




		void EnableOutput()
		{
			enable = true;
		}
		void DisableOutput()
		{
			enable = false;
		}


	protected:

		PinType pin;
		bool enable;
		UINT32 lastPulseWidth;

	};

}


#endif  /*_ADX_K60_PWPG_H__*/
