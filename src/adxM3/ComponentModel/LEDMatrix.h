
#ifndef _ADX_CM_LEDMATRIX_H_
#define _ADX_CM_LEDMATRIX_H_

#include "../HadwareAbstraction/GDI/GraphicsDevice.h"

namespace adxM3
{

	template< class DriverType, int WIDTH, int HEIGHT, typename CoordType, class ColorType >
	class LEDMatrix
		: public GraphicsDevice
		, public DriverType<CoordType, ColorType>
	{
	public:

		using DriverType::SetValue;


		/*
		 * @brief	图形设备画点/读点方法
		 * @param	x, y 点坐标
		 *			color 写入该点的颜色值
		 * @retval	该点的颜色值
		 */
		virtual void SetPixel(CoordType x, CoordType y, ColorType color)
		{
			gram[x][y] = color;
			Update(y);
		}
		virtual ColorType GetPixel(CoordType x, CoordType y) const
		{

		}


		void Update()
		{
			SetValue(8 - y, _BV(8 - x));
		}


	private:
		
		ColorType gram[WIDTH][HEIGHT];

	};

}

#endif  /*_ADX_CM_LEDMATRIX_H_*/
