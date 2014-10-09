
#ifndef _PERIPHERALONOFF_H_
#define _PERIPHERALONOFF_H_


namespace adxM3
{
	

	/*
	 * @brief	外设行为开关控制接口类
	 *			无法进行开关控制的外设建议不要继承该类
	 */
	class PeripheralOnOff
	{
	public:


		/*
		 * @brief	外设开启/关闭接口方法
		 * @retval	派生类无法实现与执行效果相符时请返回0，否则返回1
		 *			e.g. 当调用EnablePeripheral方法若某外设始终处于开启状态则返回1
		 *				 当调用DisablePeripheral方法若某外设始终处于开启状态无法关闭时请返回0
		 */
		virtual int EnablePeripheral() = 0;
		virtual int DisablePeripheral() = 0;


	};

}


#endif  /*_PERIPHERALONOFF_H_*/
