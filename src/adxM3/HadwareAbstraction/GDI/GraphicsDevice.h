
#ifndef _ADX_HA_GRAPHICSDEVICE_H_
#define _ADX_HA_GRAPHICSDEVICE_H_

#include "../../common/defs.h"
#include "../../common/Graphical/BasicGraphics.h"
#include "../../common/FONT.H"
#include "../../common/TypeTraits.h"

namespace adxM3
{

	/*
	 * @brief	图形设备抽象接口
	 *			定义了绘制像素点和连续显存写入的接口
	 * @promise	继承于此类的图形设备请在public区段内定义常量
	 *			MAX_WIDTH与MAX_Height，单位为逻辑像素点
	 */
	template< typename _CoordType, class _ColorType >
	class GraphicsDevice
	{ 
	public:

		typedef _CoordType CoordType;
		typedef _ColorType ColorType;
		typedef typename ArithTraits<CoordType>::Unsigned SizeType;
		typedef Point2D<CoordType> PointType;



		/*
		 * @brief	图形设备画点/读点方法
		 * @param	x, y 点坐标
		 *			color 写入该点的颜色值
		 * @retval	该点的颜色值
		 */
		virtual void SetPixel(CoordType x, CoordType y, ColorType color) = 0;
		virtual ColorType GetPixel(CoordType x, CoordType y) = 0;
		


		/*
		 * @brief	图形设备连续写入数据到设备显存中
		 * @param	x, y 起始点坐标，每写入一个数据后，其值会自动根据设备的情况而变化
		 *			color 连续写入的每一点的色值的首地址
		 *			const_color 连续写入的颜色值，用于填充一个区域
		 *			length 连续写入长度
		 */
		virtual void SequentialSetPixel(CoordType x, CoordType y, ColorType color[], unsigned int length) = 0;
		virtual void SequentialSetPixel(CoordType x, CoordType y, ColorType const_color, unsigned int length) = 0;
		


		/*
		 * @brief	清除屏幕全部内容
		 */
		virtual void Clear() = 0;



		/*
		 * 文字输出的背景模式
		 */
		enum BKM
		{
			BackgroundSolid				= 0,
			BackgroundAlpha				= 1
		};



		/*
		 * 文字输出的对齐类型
		 */
		enum ALIGNT
		{
			AlignLeft,
			AlignRight
		};



		/*
		 * @brief	向图形设备输出文字
		 * @param	pos 输出到图形设备的位置，文字的左上角为注册点
		 *			ch, str 要输出的字符或字符串
		 *			size 字符尺寸，当前支持12和16像素高的字体
		 *			forecolor, backcolor 前景色与背景色
		 *			alpha 背景是否透明，即是否带背景输出
		 *			setw 设置输出宽度，字符数大于宽度时多余内容不会输出，小于宽度时会补空格
		 *			align 设置输出对齐模式，
		 *				@arg AlignLeft 输出要输出文字从左数起的setw个，不足补空格
		 *				@arg AlignRight 输出要输出文字从右数起的setw个，不足补空格
		 */
		virtual void TextOut(PointType pos, 
							 char ch, 
							 const SizeType size = 12, 
							 const ColorType forecolor = 1, 
							 const ColorType backcolor = 0,
							 const BKM alpha = BackgroundAlpha)
		{
			// 不执行超界判断

			CoordType lineOfChar = 0;      
			ch -= ' ';											//偏移换算
			char* LatticeCodeTable = size == 12 ? (char*)ASCII1206[ch] : (char*)ASCII1608[ch];
	
			SizeType wid = size / 2;
			if (alpha) 
			{
				for (CoordType line = 0; line < size; ++line) 
				{
					lineOfChar = LatticeCodeTable[line];
					for (CoordType col = 0; col < wid; ++col) 
					{
						if (lineOfChar & 0x01) 
						{
							SetPixel(pos.x, pos.y, forecolor);
						}
						lineOfChar >>= 1;
						++pos.x;
					}
					pos.x -= wid;								// 回到该字符下一线起始处
					++pos.y;
				}
			} 
			else 
			{
				for (CoordType line = 0; line < size; ++line) 
				{
					lineOfChar = LatticeCodeTable[line];
					ColorType lineBitTbl[8];
					for (CoordType i = 0; i < wid; ++i)
					{
						lineBitTbl[i] = (lineOfChar & 0x01) ? forecolor : backcolor;
						lineOfChar >>= 1;
					}
					SequentialSetPixel(pos.x, pos.y, lineBitTbl, wid);	
					++pos.y;
				}
			}
		}
		virtual void TextOut(PointType pos, 
							 const char* str, 
							 const SizeType size = 12, 
							 const ColorType forecolor = 1, 
							 const ColorType backcolor = 0,
							 const BKM alpha = BackgroundAlpha)
		{
			SizeType wid = size / 2;
			while (*str != '\0') 
			{      
				TextOut(pos, *str, size, forecolor, backcolor, alpha);
				pos.x += wid;
				++str;
			} 
		}
		virtual void TextOut(PointType pos,
							 const char* str,
							 SizeType setw,
							 const ALIGNT align,
							 const SizeType size = 12,
							 const ColorType forecolor = 1, 
							 const ColorType backcolor = 0,
							 const BKM alpha = BackgroundAlpha)
		{
			SizeType wid = size / 2;
			if (align == AlignLeft)
			{
				while (*str != '\0' && setw > 0) 
				{ 
					--setw;
					TextOut(pos, *str, size, forecolor, backcolor, alpha);
					pos.x += wid;
					++str;
				}
				while (setw > 0)
				{
					--setw;
					TextOut(pos, ' ', size, forecolor, backcolor, alpha);
					pos.x += wid;
				}
			}
			else	// AlignRight
			{
				SizeType len = BasicString::strlen(str);
				if (len > setw)
				{
					TextOut(pos, str + len - setw, size, forecolor, backcolor, alpha);
				}
				else
				{
					CoordType spaceCount = setw - len;
					for (CoordType i = 0; i < spaceCount; ++i)
					{
						TextOut(pos, ' ', size, forecolor, backcolor, alpha);
						pos.x += wid;
					}
					TextOut(pos, str, size, forecolor, backcolor, alpha);
				}
			}
		}
		


		~GraphicsDevice() {}

	};

}


#endif  /*_ADX_HA_GRAPHICSDEVICE_H_*/
