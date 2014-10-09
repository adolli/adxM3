
#ifndef _ADX_CM_MAX7219MATRIXLED_H_
#define _ADX_CM_MAX7219MATRIXLED_H_

#include "MAX7219.h"
#include "../HadwareAbstraction/GDI/GraphicsDevice.h"

namespace adxM3
{

	typedef bool BinaryColor;

	class MAX7219_MatrixLED 
		: protected MAX7219
		, public GraphicsDevice<INT8, BinaryColor>
	{
	public:


		typedef INT8 CoordType;
		typedef UINT8 SizeType;
		typedef BinaryColor ColorType;
		
		enum
		{
			MAX_WIDTH = 8,
			MAX_HEIGHT = 8
		};



		MAX7219_MatrixLED(SynchronousCommunicator& spi)
			:MAX7219(spi)
		{
			SetDecodeMode(NoDecode);

			for (int i = 0; i < MAX_HEIGHT; ++i)
			{
				data[i] = 0;
			}
		}

		
		
		
		/*
		 * @brief	8x8LED 画点/读点方法
		 * @param	x, y 点坐标
		 *			color 写入该点的颜色值
		 * @retval	该点的颜色值
		 */
		virtual void SetPixel(CoordType x, CoordType y, ColorType color)
		{
			if (x >= MAX_WIDTH || y >= MAX_HEIGHT) return;

			if (color != 0)
			{
				data[y] |= _BV(x);
			}
			else
			{
				data[y] &= ~_BV(x);
			}
			Update(y, y);
		}
		virtual ColorType GetPixel(CoordType x, CoordType y)
		{
			if (x >= MAX_WIDTH || y >= MAX_HEIGHT) return 0;

			return data[y] & _BV(x);
		}
		
		

		/*
		 * @brief	8x8LED 连续写入数据到设备显存中
		 * @param	x, y 起始点坐标，每写入一个数据后，其值会自动根据设备的情况而变化
		 *			color 连续写入的每一点的值的首地址
		 *			const_color 连续写入的颜色值，用于填充一个区域
		 *			length 连续写入长度
		 */
		virtual void SequentialSetPixel(CoordType x, CoordType y, ColorType color[], unsigned int length)
		{
			if (x >= MAX_WIDTH || y >= MAX_HEIGHT) return;

			CoordType origin_y = y;
			while (length--)
			{
				if (*color++ != 0)
				{
					data[y] |= _BV(x);
				}
				else
				{
					data[y] &= ~_BV(x);
				}
				++x;
				if (x == MAX_WIDTH)
				{
					x = 0;
					++y;
					if (y == MAX_HEIGHT)
					{
						break;
					}
				}
			}
			Update(origin_y, y - 1);
		}
		virtual void SequentialSetPixel(CoordType x, CoordType y, ColorType const_color, unsigned int length)
		{
			if (x >= MAX_WIDTH || y >= MAX_HEIGHT) return;

			CoordType origin_y = y;
			while (length--)
			{
				if (const_color != 0)
				{
					data[y] |= _BV(x);
				}
				else
				{
					data[y] &= ~_BV(x);
				}
				++x;
				if (x == MAX_WIDTH)
				{
					x = 0;
					++y;
					if (y == MAX_HEIGHT)
					{
						break;
					}
				}
			}
			Update(origin_y, y - 1);
		}




		/*
		 * @brief	清除屏幕全部内容
		 */
		virtual void Clear()
		{
			for (int i = D0; i <= D7; ++i)
			{
				SetValue(DigitBits(i), 0);
			}
		}



		using MAX7219::SetIntensity;

	private:


		/*
		 * @brief	8x8LED 更新屏幕
		 * @param	y_start 起始行号
		 *			y_end 终止行号（包含此行）
		 */
		void Update(CoordType y_start, CoordType y_end)
		{
			for (int r = y_start; r <= y_end; ++r)
			{
				SetValue(DigitBits(r + 1), data[r]);
			}
		}



		BYTE data[MAX_HEIGHT];
		
	};

}

#endif  /*_ADX_CM_MAX7219MATRIXLED_H_*/
