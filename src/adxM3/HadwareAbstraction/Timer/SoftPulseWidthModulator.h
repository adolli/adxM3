
#ifndef _ADX_HA_SOFTPULSEWIDTHMODULATOR_H_
#define _ADX_HA_SOFTPULSEWIDTHMODULATOR_H_


#include "../../common/defs.h"
#include "../GPIO/GeneralPurposeInputOutputPin.h"


namespace adxM3
{


	/*
	 * @brief	������������
	 *			ռ�ձ������ڸ�����һ�����ں����
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
	class SoftPulseWidthModulator
		: public TimerType
		, public PulseWidthModulator
		, public TimerIOEnabler
	{
	protected:

		using TimerType::AddEventListener;
		using TimerType::RemoveEventListener;
		using TimerType::StartOnce;
		using TimerType::Start;
		using TimerType::Stop;
		using TimerType::SetTimeOut;


	public:



		SoftPulseWidthModulator(float duty = 0, UINT32 period = 0)
			: PulsePeriod(period), PulseWidth(0), ActivePolarity(ActiveHigh)
		{
			OutputChannel.SetDirection(GeneralPurposeInputOutput::OUT);
			OutputChannel.OutputConfig(GeneralPurposeInputOutput::PushPull, GeneralPurposeInputOutput::HighSpeed);
			OutputChannel.SetLevel(!ActivePolarity);

			if (period != 0)
			{
				SetPulsePeriod(period);
			}
			SetDutyCycle(duty);

			struct TimerBaseEventListener
			{
				static void OnUpdate(Timer* _This)
				{
					SoftPulseWidthModulator* This = dynamic_cast<SoftPulseWidthModulator*>(_This);
					if (!This)
					{
						return;
					}
					
					// ��ʼ����
					static bool currentOutputLevel = This->ActivePolarity;
					if (This->PulsePeriod != 0)
					{
						if (currentOutputLevel == This->ActivePolarity)
						{
							// �����ǰPWM�����ƽ���ڻ״̬������һ�ζ�ʱ�ǻ״̬����
							This->SetTimeOut(This->PulsePeriod - This->PulseWidth);
						}
						else
						{
							This->SetTimeOut(This->PulseWidth);
						}
						This->OutputChannel.SetLevel(currentOutputLevel);
						currentOutputLevel = !currentOutputLevel;
					}
				}
			};
			AddEventListener(Timer::TIME_OUT, TimerBaseEventListener::OnUpdate);
		}





		/*
		 * @brief	������������ʹ��/��ֹ
		 *			�����ú�����������ģʽ����
		 * @retval	�ɹ�����/�ر��������1�����򷵻�0
		 */
		virtual int EnableOutput()
		{
			Start();
			return 1;
		}
		virtual int DisableOutput()
		{
			Stop();
			OutputChannel = !ActivePolarity;
			return 1;
		}






		/*
		 * @brief	�������������ռ�ձȣ��ٷֱȣ���ȷ��1%��
		 * @param	duty ռ�ձȣ�ȡֵ��Χ[0,1]���˷���ֻȡ��С������
		 * @param	duty_percent ռ�ձȣ�ȡֵ��Χ[0,100]��
		 *				����һ���Ǹ���İ汾��ʵ��ʱ�벻Ҫ�漰��������
		 */
		virtual void SetDutyCycle(float duty)
		{
			if (PulsePeriod == 0) return;

			RangeLimit(0.001, <=, duty, <=, 0.999);
			PulseWidth = PulsePeriod * duty;
		}
		virtual void SetDutyCyclePercent(const UINT8 duty_percent)
		{
			if (PulsePeriod == 0) return;

			UINT8 dty = duty_percent;
			RangeLimit(1, <=, dty, <=, 99);
			PulseWidth = PulsePeriod * dty / 100;
		}



		/*
		 * @brief	�����������ȡռ�ձȣ��ٷֱȣ���ȷ��1%��
		 * @retval	ռ�ձȣ�ȡֵ��Χ[0,1]
		 */
		virtual float GetDutyCycle() const
		{
			if (PulsePeriod == 0) return 0;

			return (float)PulseWidth / PulsePeriod;
		}
		virtual UINT8 GetDutyCyclePercent() const
		{
			if (PulsePeriod == 0) return 0;

			return PulseWidth * 100 / PulsePeriod;
		}



		/*
		 * @brief	���������������������
		 *			�����������ڲ���ı�ռ�ձ�
		 * @param	period_us �������ڣ���λus
		 */
		virtual void SetPulsePeriod(const UINT32 period_us)
		{
			PulsePeriod = period_us;
		}
		




		/*
		 * @brief	���������������������
		 * @retval	�������ڣ���λus
		 */
		virtual UINT32 GetPulsePeriod() const
		{
			return PulsePeriod;
		}





		/*
		 * @brief	������������øߵ�ƽ����
		 * @param	width �����ȣ���λus����ֵ�벻Ҫ������������
		 * @retval	�����ȣ���λus
		 * @ovride	Ϊ��ø������ܣ�����������д�÷���
		 */
		virtual void SetPulseWidth(const UINT32 width)
		{
			PulseWidth = width;
		}
		virtual UINT32 GetPulseWidth() const
		{
			return PulseWidth;
		}





		/*
		 * @brief	PWM��������
		 * @param	pol ���ԣ�������PWMPOLö�����κ�һ��
		 *				@arg ActiveLow �ɹ��Ƚ�ʱ�͵�ƽ(LowTruePulse)
		 *				@arg ActiveHigh �ɹ��Ƚ�ʱ�ߵ�ƽ(HighTruePulse)
		 */
		virtual void SetPolarity(const PWMPOL pol)
		{
			ActivePolarity = pol;
		}





	public:


		PinType OutputChannel;
		volatile UINT32 PulsePeriod;
		volatile UINT32 PulseWidth;
		volatile PWMPOL ActivePolarity;

	};
}

#endif  /*_ADX_HA_SOFTPULSEWIDTHMODULATOR_H_*/
