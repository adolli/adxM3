
#ifndef _PULSEWIDTHMODULATOR_H_
#define _PULSEWIDTHMODULATOR_H_

#include "../../common/defs.h"


namespace adxM3
{

	/*
	 * @brief	�������������ģ��
	 *			������������ź�������ܵ������ʵ�ָ���
	 */
	class PulseWidthModulator
	{
	public:


		/*
		 * @brief	�������������ռ�ձȣ��ٷֱȣ���ȷ��1%��
		 * @param	duty ռ�ձȣ�ȡֵ��Χ[0,1]���˷���ֻȡ��С������
		 * @param	duty_percent ռ�ձȣ�ȡֵ��Χ[0,100]��
		 *				����һ���Ǹ���İ汾��ʵ��ʱ�벻Ҫ�漰��������
		 */
		virtual void SetDutyCycle(float duty) = 0;
		virtual void SetDutyCyclePercent(const UINT8 duty_percent)
		{
			return;
		}



		/*
		 * @brief	�����������ȡռ�ձȣ��ٷֱȣ���ȷ��1%��
		 * @retval	ռ�ձȣ�ȡֵ��Χ[0,1]
		 */
		virtual float GetDutyCycle() const = 0;
		virtual UINT8 GetDutyCyclePercent() const = 0;



		/*
		 * @brief	���������������������
		 *			�����������ڲ���ı�ռ�ձ�
		 * @param	period_us �������ڣ���λus
		 */
		virtual void SetPulsePeriod(const UINT32 period_us) = 0;
		




		/*
		 * @brief	���������������������
		 * @retval	�������ڣ���λus
		 */
		virtual UINT32 GetPulsePeriod() const = 0;




		/*
		 * @brief	������������÷���Ƶ��
		 * @param	freq ���壨���������ڣ���λHz��Ƶ���벻Ҫ����1MHz
		 * @retval	���壨���������ڣ���λHz
		 * @ovride	����ҪƵ�ʳ���1MHz����������д�÷���
		 */
		virtual void SetFrequency(const UINT32 freq)
		{
			SetPulsePeriod(1000000 / freq);
		}
		virtual UINT32 GetFrequency() const
		{
			return 1000000 / GetPulsePeriod();
		}



		/*
		 * @brief	������������øߵ�ƽ����
		 * @param	width �����ȣ���λus����ֵ�벻Ҫ������������
		 * @retval	�����ȣ���λus
		 * @ovride	Ϊ��ø������ܣ�����������д�÷���
		 */
		virtual void SetPulseWidth(const UINT32 width)
		{
			SetDutyCycle((float)width / GetPulsePeriod());
		}
		virtual UINT32 GetPulseWidth() const
		{
			return GetDutyCycle() * GetPulsePeriod();
		}



		/*
		 * @brief	PWM����ѡ��
		 */
		enum PWMPOL
		{
			ActiveLow,
			ActiveHigh
		};



		/*
		 * @brief	PWM��������
		 * @param	pol ���ԣ�������PWMPOLö�����κ�һ��
		 *				@arg ActiveLow �ɹ��Ƚ�ʱ�͵�ƽ(LowTruePulse)
		 *				@arg ActiveHigh �ɹ��Ƚ�ʱ�ߵ�ƽ(HighTruePulse)
		 */
		virtual void SetPolarity(const PWMPOL pol) = 0;




		/*
		 * @brief	PWM����ģʽѡ��
		 */
		enum PWMALIGN
		{
			EdgeAligned,
			CenterAligned
		};




		virtual ~PulseWidthModulator() {}

	};

}

#endif  /*_PULSEWIDTHMODULATOR_H_*/
