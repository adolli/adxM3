
#ifndef _ADX_CM_HBRIDGEMOTORDRIVER_H_
#define _ADX_CM_HBRIDGEMOTORDRIVER_H_

#include "../common/defs.h"
#include "../../loki/EmptyType.h"
using namespace Loki;



namespace adxM3
{


	template< class T >
	struct ForwardSignal
	{
		typedef T ResultType;
	};


	template< class T >
	struct BackwardSignal
	{
		typedef T ResultType;
	};





	template< class, class T = EmptyType > class HBridgeMotorDriver;
	



	/*
	 * @brief	ֱ�����������HBridgeMotorDriver
	 *			��˫����������ʽ��
	 * @tparam	ForwardSignalType ǰ�������źţ�Ҫ��ʵ��PulseWidthModulator
	 *			BackwardSignalType ���������źţ�Ҫ��ʵ��PulseWidthModulator
	 */
	template
	<
		class ForwardPulse, 
		class BackwardPulse
	> 
	class HBridgeMotorDriver
	<
		ForwardSignal<ForwardPulse>,
		BackwardSignal<BackwardPulse>
	>
	{
	public:

		/*
		 * @type	����ٶȿ�����ΪPWMռ�ձ�
		 */
		typedef float VelocityType;


		HBridgeMotorDriver()
		{
			// ��ʼ���ٶ�
			SetSpeed(0);

			// PWM�����ź�ʹ���������ģʽ����С�Ʋ�
			FW.SetAlignment(FW.CenterAligned);
			BW.SetAlignment(BW.CenterAligned);

			// PWM�����ź����ʹ��
			FW.EnableOutput();
			BW.EnableOutput();
		}



		/*
		 * @brief	��������ٶ�
		 *			˫�����
		 * @param	speed �ٶȷ�������Χ[-1,1]������
		 */
		void SetSpeed(VelocityType speed)
		{
			// ����speedֵ��Χ
			RangeLimit(-1, <=, speed, <=, 1);
			
			// ��[-1,1] -> [0,1]
			FW.SetDutyCycle(0.5 + speed / 2);
			BW.SetDutyCycle(0.5 - speed / 2);
		}



		/*
		 * @brief	�����ȡ�ٶ�
		 * @retval	�ٶȷ�������Χ[0,1]������
		 */
		VelocityType GetSpeed() const
		{
			float FWDuty = FW.GetDutyCycle();

			// ��ʽ �� = 2 * �� - 1;
			// 0.5Ϊ����λ��[0,1] ����ӳ�� [-1,1]
			return 2 * FWDuty - 1;
		}



	private:

		ForwardPulse FW;
		BackwardPulse BW;

	};




}

#endif  /*_ADX_CM_HBRIDGEMOTORDRIVER_H_*/
