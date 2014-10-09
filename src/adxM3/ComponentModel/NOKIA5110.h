
#ifndef _ADX_CM_NOKIA5110_H_
#define _ADX_CM_NOKIA5110_H_

//////////////////////////////////////////////////////////////////////////
//	这是Nokia5110显示屏驱动器的部件模型
//	其型号为飞利浦的NOKIA5110
//	NOKIA5110只是一个液晶屏驱动器，不是一个液晶屏
//////////////////////////////////////////////////////////////////////////

#include "../common/assert.h"
#include "../HadwareAbstraction/Communicator/SynchronousCommunicator.h"
#include "../HadwareAbstraction/GPIO/GeneralPurposeInputOutputPin.h"
#include "../HadwareAbstraction/Timer/BlockingDelayController.h"
#include "../HadwareAbstraction/GDI/GraphicsDevice.h"


namespace adxM3
{

	typedef bool BinaryColor;


	// 值类型使用16位整数，避免某些图形计算中间值超出范围
	class NOKIA5110
		: public GraphicsDevice<INT16, BinaryColor>
	{
	public:


		
		
		enum
		{
			MAX_WIDTH = 84,
			MAX_HEIGHT = 48
		};


		NOKIA5110(SynchronousCommunicator& _SPI,
				GeneralPurposeInputOutputPin& _DC,
				GeneralPurposeInputOutputPin& _RST,
				GeneralPurposeInputOutputPin& _BackLight,
				BlockingDelayController& _MCU)
				: SPI(_SPI), DC(_DC), RST(_RST), BackLight(_BackLight), MCU(_MCU)
		{
			SPI.SetMode(ConfigurableCommunicator::MASTER);
			SPI.SetBaudRate(4000000);							// 4Mbps MAX
			SPI.SetFirstBit(ConfigurableCommunicator::MSB);
			SPI.SetDataBits(8);
			SPI.SetClockPolarity(SynchronousCommunicator::CPOL_LOW);
			SPI.SetClockPhase(SynchronousCommunicator::CPHA_1st_EDGE);

			DC.OutputConfig(GeneralPurposeInputOutput::PushPull);	
			RST.OutputConfig(GeneralPurposeInputOutput::PushPull);	
			BackLight.OutputConfig(GeneralPurposeInputOutput::PushPull, GeneralPurposeInputOutput::UltraLowSpeed);


			// 复位信号
			Reset();

			// 配置模式
			
			WriteReg(COMMAND, 0x21);			// (H=1) LCD功能设置：芯片活动，水平寻址，使用扩展指令
			WriteReg(COMMAND, _BV(3) | 1);			// 设置temperature
			WriteReg(COMMAND, _BV(5) | 1);			// 设置bias
			WriteReg(COMMAND, _BV(7) | 64);			// 设置VOP值，室温下的编程范围为3.00-10.68
			
			WriteReg(COMMAND, 0x20);			// (H=0) LCD功能设置：芯片活动，水平寻址，使用基本指令
			WriteReg(COMMAND, _BV(3) | 4);			// 设定显示配置:普通模式

			// 清屏
			Clear();

			// 初始化完毕后开启背光
			EnableBackLight();
		}



		/*
		 * @brief	液晶驱动器复位
		 */
		void Reset()
		{
			RST = 1;
			MCU.Delay_us(200);
			RST = 0;
			MCU.Delay_ms(10);
			RST = 1;
			MCU.Delay_us(200);
		}



		/*
		 * @brief	液晶驱动器开启背光
		 */
		void EnableBackLight()
		{
			BackLight = 0;
		}
		void DisableBackLight()
		{
			BackLight = 1;
		}



		/*
		 * @brief	图形设备画点/读点方法
		 * @param	x, y 点坐标
		 *			color 写入该点的颜色值
		 * @retval	该点的颜色值
		 */
		virtual void SetPixel(CoordType x, CoordType y, ColorType color)
		{
			_SetPixelData(x, y, color);
			CoordType row = y / 8;
			Update(row, x, row, x);
		}
		virtual ColorType GetPixel(CoordType x, CoordType y)
		{
			if (x >= MAX_WIDTH || y >= MAX_HEIGHT) return 0;
			
			CoordType row = y / 8;
			UINT8 row_bitpos = y % 8;
			return data[row][x] & _BV(row_bitpos);
		}
		


		/*
		 * @brief	图形设备连续写入数据到设备显存中
		 * @param	x, y 起始点坐标，每写入一个数据后，其值会自动根据设备的情况而变化
		 *			color 连续写入的每一点的值的首地址
		 *			const_color 连续写入的颜色值，用于填充一个区域
		 *			length 连续写入长度
		 */
		virtual void SequentialSetPixel(CoordType x, CoordType y, ColorType color[], unsigned int length)
		{
			if (x >= MAX_WIDTH || y >= MAX_HEIGHT) return;

			// 标记一个最小修改范围的矩形边界
			UINT8 start_row = y / 8;
			UINT8 start_col = x;
			UINT8 end_col = x + length;

			CoordType row = start_row;
			UINT8 row_bitpos = y % 8;

			while (length--)
			{
				if (*color++ != 0)
				{
					data[row][x] |= _BV(row_bitpos);
				}
				else
				{
					data[row][x] &= ~_BV(row_bitpos);
				}
				++x;
				if (x == MAX_WIDTH)
				{
					x = 0;
					
					start_col = 0;			// 如果发生了换行，则刷新边界要从第0col开始，并且刷新到最后一列
					end_col = MAX_WIDTH;
					
					++y;
					if (y == MAX_HEIGHT)
					{
						break;
					}
					else
					{
						row = y / 8;
						row_bitpos = y % 8;
					}
				}
			}
			Update(start_row, start_col, row, end_col);
		}
		virtual void SequentialSetPixel(CoordType x, CoordType y, ColorType const_color, unsigned int length)
		{
			if (x >= MAX_WIDTH || y >= MAX_HEIGHT) return;

			// 标记一个最小修改范围的矩形边界
			UINT8 start_row = y / 8;
			UINT8 start_col = x;
			UINT8 end_col = x + length;

			CoordType row = start_row;
			UINT8 row_bitpos = y % 8;

			while (length--)
			{
				if (const_color != 0)
				{
					data[row][x] |= _BV(row_bitpos);
				}
				else
				{
					data[row][x] &= ~_BV(row_bitpos);
				}
				++x;
				if (x == MAX_WIDTH)
				{
					x = 0;
					
					start_col = 0;			// 如果发生了换行，则刷新边界要从第0col开始，并且刷新到最后一列
					end_col = MAX_WIDTH;
					
					++y;
					if (y == MAX_HEIGHT)
					{
						break;
					}
					else
					{
						row = y / 8;
						row_bitpos = y % 8;
					}
				}
			}
			Update(start_row, start_col, row, end_col);
		}
		


		/*
		 * @brief	清除屏幕全部内容
		 */
		virtual void Clear()
		{
			SetCursor(0, 0);
			for (int col = 0; col < MAX_WIDTH; ++col)
			{
				for (int row = 0; row < MAX_HEIGHT / 8; ++row)
				{
					WriteReg(DATA, 0);
					data[row][col] = 0;
				}
			}
		}




		/*
		 * @brief	NOKIA5110输出文字
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
			// 判断范围是否有效
			SizeType wid = size / 2;
			if (pos.x >= MAX_WIDTH - wid || pos.y >= MAX_HEIGHT - size) return;

			// 计算更新区域并保存留到最后一次过更新
			CoordType x_start = pos.x, x_end = pos.x + wid;
			CoordType row_start = pos.y / 8, row_end = (pos.y + size) / 8;

			CoordType lineOfChar = 0;      
			ch -= ' ';											//偏移换算
			char* LatticeCodeTable = size==12 ? (char*)ASCII1206[ch] : (char*)ASCII1608[ch];
	
			if (alpha) 
			{
				for (CoordType line = 0; line < size; ++line) 
				{
					lineOfChar = LatticeCodeTable[line];
					for (CoordType col = 0; col < wid; ++col) 
					{
						if (lineOfChar & 0x01) 
						{
							_SetPixelData(pos.x, pos.y, forecolor);
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
					
					for (CoordType col = 0; col < wid; ++col) 
					{
						if (lineOfChar & 0x01) 
						{
							_SetPixelData(pos.x + col, pos.y, forecolor);
						}
						else 
						{
							_SetPixelData(pos.x + col, pos.y, backcolor);
						}
						lineOfChar >>= 1;
					}
					++pos.y;
				}
			}

			// 计算更新区域
			Update(row_start, x_start, row_end, x_end);
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
					SizeType spaceCount = len - setw;
					for (SizeType i = 0; i < spaceCount; ++i)
					{
						TextOut(pos, ' ', size, forecolor, backcolor, alpha);
						pos.x += wid;
					}
					TextOut(pos, str, size, forecolor, backcolor, alpha);
				}
			}
		}




	private:


		/*
		 * @brief	将内存中的虚拟显存更新到设备显存中
		 *			row_start, row_end 5110的起始行号和终止行号
		 *			x_start, x_end 5110的起始列号和终止列号
		 *				以上参数均为闭区间参数
		 */
		void Update(CoordType row_start, CoordType x_start,
					CoordType row_end, CoordType x_end)
		{
			for (CoordType row = row_start; row <= row_end; ++row)
			{
				SetCursor(x_start, row);
				for (CoordType col = x_start; col <= x_end; ++col)
				{
					WriteReg(DATA, data[row][col]);
				}
			}
		}



		/*
		 * @brief	发送数据或命令选项
		 */
		enum DTCMD
		{
			COMMAND,
			DATA
		};



		/*
		 * @brief	液晶驱动器寄存器写入
		 * @param	dc 命令或者数据选项，可以是DTCMD枚举中的任何一个
		 *			val 命令值或者是数据值，depends on dc
		 */
		void WriteReg(const DTCMD dc, UINT8 val)
		{
			STATIC_ASSERT(DATA == 1 && COMMAND == 0,
				enumerate_is_not_campacted_with_this_configuration);
			DC = dc;
			SPI.SynchronouslyTranceive(val);
		}




		/*
		 * @brief	液晶驱动器设置光标位置
		 * @param	x 横坐标（行），范围[0,83]
		 *			x 纵坐标（列），范围[0,5]
		 */
		void SetCursor(int x, int y)
		{
			RangeLimit(0, <=, x, <=, MAX_WIDTH - 1);
			RangeLimit(0, <=, y, <=, 5);
			WriteReg(COMMAND, 0x40 | y);				// row
			WriteReg(COMMAND, 0x80 | x);				// col
		}



	protected:

		/*
		 * @brief	向内存（显存）中写入内容，但不更新到屏幕上
		 */
		void _SetPixelData(CoordType x, CoordType y, ColorType color)
		{
			if (x >= MAX_WIDTH || y >= MAX_HEIGHT) return;

			CoordType row = y / 8;
			UINT8 row_bitpos = y % 8;
			if (color != 0)
			{
				data[row][x] |= _BV(row_bitpos);
			}
			else
			{
				data[row][x] &= ~_BV(row_bitpos);
			}
		}


	

		SynchronousCommunicator& SPI;					// 模块SPI通讯接口
		GeneralPurposeInputOutputPin& DC;				// 指令与数据选择信号
		GeneralPurposeInputOutputPin& RST;				// 复位信号
		GeneralPurposeInputOutputPin& BackLight;		// 背光电源
		BlockingDelayController& MCU;					// 系统延时


	private:

		UINT8 data[MAX_HEIGHT / 8][MAX_WIDTH];

	};

}


#endif  /*_ADX_CM_NOKIA5110_H_*/
