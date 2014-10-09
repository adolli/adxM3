
#ifndef _ADX_HA_GRAPHICSDEVICE_H_
#define _ADX_HA_GRAPHICSDEVICE_H_

#include "../../common/defs.h"
#include "../../common/Graphical/BasicGraphics.h"
#include "../../common/FONT.H"
#include "../../common/TypeTraits.h"

namespace adxM3
{

	/*
	 * @brief	ͼ���豸����ӿ�
	 *			�����˻������ص�������Դ�д��Ľӿ�
	 * @promise	�̳��ڴ����ͼ���豸����public�����ڶ��峣��
	 *			MAX_WIDTH��MAX_Height����λΪ�߼����ص�
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
		 * @brief	ͼ���豸����/���㷽��
		 * @param	x, y ������
		 *			color д��õ����ɫֵ
		 * @retval	�õ����ɫֵ
		 */
		virtual void SetPixel(CoordType x, CoordType y, ColorType color) = 0;
		virtual ColorType GetPixel(CoordType x, CoordType y) = 0;
		


		/*
		 * @brief	ͼ���豸����д�����ݵ��豸�Դ���
		 * @param	x, y ��ʼ�����꣬ÿд��һ�����ݺ���ֵ���Զ������豸��������仯
		 *			color ����д���ÿһ���ɫֵ���׵�ַ
		 *			const_color ����д�����ɫֵ���������һ������
		 *			length ����д�볤��
		 */
		virtual void SequentialSetPixel(CoordType x, CoordType y, ColorType color[], unsigned int length) = 0;
		virtual void SequentialSetPixel(CoordType x, CoordType y, ColorType const_color, unsigned int length) = 0;
		


		/*
		 * @brief	�����Ļȫ������
		 */
		virtual void Clear() = 0;



		/*
		 * ��������ı���ģʽ
		 */
		enum BKM
		{
			BackgroundSolid				= 0,
			BackgroundAlpha				= 1
		};



		/*
		 * ��������Ķ�������
		 */
		enum ALIGNT
		{
			AlignLeft,
			AlignRight
		};



		/*
		 * @brief	��ͼ���豸�������
		 * @param	pos �����ͼ���豸��λ�ã����ֵ����Ͻ�Ϊע���
		 *			ch, str Ҫ������ַ����ַ���
		 *			size �ַ��ߴ磬��ǰ֧��12��16���ظߵ�����
		 *			forecolor, backcolor ǰ��ɫ�뱳��ɫ
		 *			alpha �����Ƿ�͸�������Ƿ���������
		 *			setw ���������ȣ��ַ������ڿ��ʱ�������ݲ��������С�ڿ��ʱ�Ჹ�ո�
		 *			align �����������ģʽ��
		 *				@arg AlignLeft ���Ҫ������ִ��������setw�������㲹�ո�
		 *				@arg AlignRight ���Ҫ������ִ��������setw�������㲹�ո�
		 */
		virtual void TextOut(PointType pos, 
							 char ch, 
							 const SizeType size = 12, 
							 const ColorType forecolor = 1, 
							 const ColorType backcolor = 0,
							 const BKM alpha = BackgroundAlpha)
		{
			// ��ִ�г����ж�

			CoordType lineOfChar = 0;      
			ch -= ' ';											//ƫ�ƻ���
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
					pos.x -= wid;								// �ص����ַ���һ����ʼ��
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
