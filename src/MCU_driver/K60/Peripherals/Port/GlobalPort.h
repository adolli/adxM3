
#ifndef _ADX_K60_GLOBALPORT_H_
#define _ADX_K60_GLOBALPORT_H_

#include "../PeripheralBase.h"
#include "../PeripheralInfo.h"
#include "../K60_reg.h"

namespace K60
{

	
	

	/*
	 * @brief	全局端口外设
	 * @tparam	BASE 外设基址
	 *				@arg PORTA_BASE
	 *				@arg PORTB_BASE
	 *				...
	 *				@arg PORTE_BASE
	 */
	template< UINT32 BASE >
	class Peripheral< PORT_INFO<BASE> >
	{
	public:

		typedef PORT_INFO<BASE> PeripheralInfo;
		typedef ClockGateController<typename PeripheralInfo::ClockSourceGate> ClockSource;


		/*
		 * @brief	标准构造函数
		 *			开启全局端口时钟
		 */
		Peripheral()
		{
			ClockSource::ClockOn();
		}



		/*
		 * @brief	标准析构函数
		 */
		virtual ~Peripheral()
		{
			// 使用了智能外设引用计数后，会在最后一个外设实例析构时关闭时钟
			ClockSource::ClockOff();
		}




		/*
		 * @brief	端口引脚类
		 */
		class Pin
		{
		public:

			Pin(unsigned int _index)
				: index(_index)
			{
			}

			~Pin()
			{
			}
		


			/*
			 * @brief	配置引脚多路复用
			 *			选通某个信号连接到外部引脚
			 * @param	alt_type 端口信号通道号
			 */
			void MultiplexConfig(unsigned int alt_type)
			{
				PeripheralInfo::BaseAccess()->PCR[index].MUX = alt_type;
			}





		protected:

			// 引脚索引号
			const PinIndex index;
		};




		/*
		 * @brief	模版常引脚类型
		 */
		template< PinIndex INDEX >
		class ConstPin
		{
		public:

			ConstPin()
			{
				// 检查引脚是否超出该端口的引脚数
				STATIC_ASSERT(INDEX < PeripheralInfo::PIN_MAX,
					this_port_does_not_have_so_much_pins);

				// 常量引脚需在此开启时钟
				ClockSource::ClockOn();
			}


			~ConstPin()
			{
				// 同样最后析构后需要关闭时钟
				ClockSource::ClockOff();
			}



			/*
			 * @brief	配置引脚多路复用
			 *			选通某个信号连接到外部引脚
			 * @param	alt_type 端口信号通道号
			 */
			static void MultiplexConfig(unsigned int alt)
			{
				PeripheralInfo::BaseAccess()->PCR[INDEX].MUX = alt;
			}
			static unsigned int GetMultiplexing()
			{
				return PeripheralInfo::BaseAccess()->PCR[INDEX].MUX;
			}

		};


	


		/*
		 * @brief	端口类
		 * @tparam	BitWidth 端口位宽
		 * @tparam	StartBitShift 起始位相对于端口索引号，默认从0开始
		 */
		template< UINT32 BitWidth, PinIndex StartBitShift = 0 >
		class Port
		{
		public:




			Port()
			{
				// 检查引脚是否超出该端口的引脚数
				STATIC_ASSERT(BitWidth + StartBitShift < PeripheralInfo::PIN_MAX && StartBitShift >= 0,
					this_port_does_not_have_so_much_pins);

				// 常量引脚需在此开启时钟
				ClockSource::ClockOn();
			}



			~Port()
			{
				// 同样最后析构后需要关闭时钟
				ClockSource::ClockOff();
			}




			

			/*
			 * @brief	配置引脚多路复用
			 *			选通某个信号连接到外部引脚
			 * @param	alt_type 端口信号通道号
			 */
			static void MultiplexConfig(unsigned int alt)
			{
				for (PinIndex i = StartBitShift; i < StartBitShift + BitWidth; ++i)
				{
					PeripheralInfo::BaseAccess()->PCR[i].MUX = alt;
				}
			}
			static unsigned int GetMultiplexing()
			{
				// 一个Port对象的所有Pin必须是相同的MUX类型
				return PeripheralInfo::BaseAccess()->PCR[StartBitShift].MUX;
			}






		};






		/*
		 * @brief	索引引脚
		 * @param	index 引脚号（PTA31则index为31）
		 * @retval	返回该类中Pin类型的临时对象
		 */
		Pin operator[](const unsigned int index) const
		{
			return Pin(index);
		}



	};



	/*
	 * @brief	全局端口（模拟端口集合）
	 */
	template<>
	class Peripheral< PORT_INFO<ANALOG_PORT> >
	{
	public:



		template< PinIndex INDEX >
		class ConstPin
		{
		public:

			
			/*
			 * @brief	模拟端口强制为ALT0状态，不需多路复用器配置
			 *			（方法在此起占位作用）
			 */
			static void MultiplexConfig(unsigned int)
			{
				// always ALT0
			}

		};
	};



	

}

#endif  /*_ADX_K60_GLOBALPORT_H_*/
