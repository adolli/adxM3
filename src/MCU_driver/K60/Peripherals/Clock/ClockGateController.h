
#ifndef _ADX_K60_CLOCKGATECONTROLLER_H_
#define _ADX_K60_CLOCKGATECONTROLLER_H_

#include "adxM3.h"


namespace K60
{

	/*
	 * @brief	����ʱ�ӿ���policy
	 * @tparam	Gate ʱ���ſؼĴ���λ����
	 */
	template< class Gate > 
	struct SmartClockOnOff
	{
		static void ClockOn()
		{
			++PeripheralReferenceCount;
			if (PeripheralReferenceCount == 1)
			{
				Gate::ON();
			}
		}
		static void ClockOff()
		{
			if (--PeripheralReferenceCount == 0)
			{
				Gate::OFF();
			}
		}

	private:
		static unsigned int PeripheralReferenceCount;
	};

	template< class Gate > 
	unsigned int SmartClockOnOff<Gate>::PeripheralReferenceCount = 0;




	/*
	 * @brief	�Զ�ʱ�ӿ����ֶ�ʱ�ӹر�policy
	 * @tparam	Gate ʱ���ſؼĴ���λ����
	 */
	template< class Gate > 
	struct ClockAutoOnManuallyOff
	{
		static void ClockOn()
		{
			Gate::ON();
		}
		static void ClockOff()
		{
			// ���Զ��ر�
		}
		static void ClockManuallyOff()
		{
			Gate::OFF();
		}
	};




	/*
	 * @brief	ʱ�ӷ�����host class
	 * @tparam	Gate ʱ���ſؼĴ���λ����
	 *			PeripheralClockSourceMgrPolicy ʱ���ſع������
	 */
	template
	< 
		class Gate,
		template<class> class PeripheralClockSourceMgrPolicy = SmartClockOnOff
	> 
	class ClockGateController
		: public PeripheralClockSourceMgrPolicy<Gate>
	{
	};


}

#endif  /*_ADX_K60_CLOCKGATECONTROLLER_H_*/
