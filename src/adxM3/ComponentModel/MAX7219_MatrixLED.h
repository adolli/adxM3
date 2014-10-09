
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
		 * @brief	8x8LED ����/���㷽��
		 * @param	x, y ������
		 *			color д��õ����ɫֵ
		 * @retval	�õ����ɫֵ
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
		 * @brief	8x8LED ����д�����ݵ��豸�Դ���
		 * @param	x, y ��ʼ�����꣬ÿд��һ�����ݺ���ֵ���Զ������豸��������仯
		 *			color ����д���ÿһ���ֵ���׵�ַ
		 *			const_color ����д�����ɫֵ���������һ������
		 *			length ����д�볤��
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
		 * @brief	�����Ļȫ������
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
		 * @brief	8x8LED ������Ļ
		 * @param	y_start ��ʼ�к�
		 *			y_end ��ֹ�кţ��������У�
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
