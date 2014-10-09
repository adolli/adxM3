
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
		 * @brief	ͼ���豸����/���㷽��
		 * @param	x, y ������
		 *			color д��õ����ɫֵ
		 * @retval	�õ����ɫֵ
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
