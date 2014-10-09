
#ifndef _ADX_K60_CLOCKGATECONTROLLER_H_
#define _ADX_K60_CLOCKGATECONTROLLER_H_

#include "adxM3.h"


namespace K60
{

	/*
	 * @brief	智能时钟控制policy
	 * @tparam	Gate 时钟门控寄存器位类型
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
	 * @brief	自动时钟开启手动时钟关闭policy
	 * @tparam	Gate 时钟门控寄存器位类型
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
			// 不自动关闭
		}
		static void ClockManuallyOff()
		{
			Gate::OFF();
		}
	};




	/*
	 * @brief	时钟分配器host class
	 * @tparam	Gate 时钟门控寄存器位类型
	 *			PeripheralClockSourceMgrPolicy 时钟门控管理策略
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
