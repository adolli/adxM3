
#ifndef _BLOCKINGDELAYCONTROLLER_H_
#define _BLOCKINGDELAYCONTROLLER_H_

namespace adxM3
{
	

	class BlockingDelayController
	{
	public:


		/*
		 * @brief	ϵͳ����ʽ��ʱ����
		 */
		virtual void Delay_ms(const UINT32 ms) const = 0;
		virtual void Delay_us(const UINT32 us) const = 0;
		virtual void Delay_ns(const UINT32 ns) const = 0;

	};

}

#endif  /*_BLOCKINGDELAYCONTROLLER_H_*/
