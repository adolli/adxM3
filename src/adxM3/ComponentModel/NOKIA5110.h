
#ifndef _ADX_CM_NOKIA5110_H_
#define _ADX_CM_NOKIA5110_H_

//////////////////////////////////////////////////////////////////////////
//	����Nokia5110��ʾ���������Ĳ���ģ��
//	���ͺ�Ϊ�����ֵ�NOKIA5110
//	NOKIA5110ֻ��һ��Һ����������������һ��Һ����
//////////////////////////////////////////////////////////////////////////

#include "../common/assert.h"
#include "../HadwareAbstraction/Communicator/SynchronousCommunicator.h"
#include "../HadwareAbstraction/GPIO/GeneralPurposeInputOutputPin.h"
#include "../HadwareAbstraction/Timer/BlockingDelayController.h"
#include "../HadwareAbstraction/GDI/GraphicsDevice.h"


namespace adxM3
{

	typedef bool BinaryColor;


	// ֵ����ʹ��16λ����������ĳЩͼ�μ����м�ֵ������Χ
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


			// ��λ�ź�
			Reset();

			// ����ģʽ
			
			WriteReg(COMMAND, 0x21);			// (H=1) LCD�������ã�оƬ���ˮƽѰַ��ʹ����չָ��
			WriteReg(COMMAND, _BV(3) | 1);			// ����temperature
			WriteReg(COMMAND, _BV(5) | 1);			// ����bias
			WriteReg(COMMAND, _BV(7) | 64);			// ����VOPֵ�������µı�̷�ΧΪ3.00-10.68
			
			WriteReg(COMMAND, 0x20);			// (H=0) LCD�������ã�оƬ���ˮƽѰַ��ʹ�û���ָ��
			WriteReg(COMMAND, _BV(3) | 4);			// �趨��ʾ����:��ͨģʽ

			// ����
			Clear();

			// ��ʼ����Ϻ�������
			EnableBackLight();
		}



		/*
		 * @brief	Һ����������λ
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
		 * @brief	Һ����������������
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
		 * @brief	ͼ���豸����/���㷽��
		 * @param	x, y ������
		 *			color д��õ����ɫֵ
		 * @retval	�õ����ɫֵ
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
		 * @brief	ͼ���豸����д�����ݵ��豸�Դ���
		 * @param	x, y ��ʼ�����꣬ÿд��һ�����ݺ���ֵ���Զ������豸��������仯
		 *			color ����д���ÿһ���ֵ���׵�ַ
		 *			const_color ����д�����ɫֵ���������һ������
		 *			length ����д�볤��
		 */
		virtual void SequentialSetPixel(CoordType x, CoordType y, ColorType color[], unsigned int length)
		{
			if (x >= MAX_WIDTH || y >= MAX_HEIGHT) return;

			// ���һ����С�޸ķ�Χ�ľ��α߽�
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
					
					start_col = 0;			// ��������˻��У���ˢ�±߽�Ҫ�ӵ�0col��ʼ������ˢ�µ����һ��
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

			// ���һ����С�޸ķ�Χ�ľ��α߽�
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
					
					start_col = 0;			// ��������˻��У���ˢ�±߽�Ҫ�ӵ�0col��ʼ������ˢ�µ����һ��
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
		 * @brief	�����Ļȫ������
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
		 * @brief	NOKIA5110�������
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
			// �жϷ�Χ�Ƿ���Ч
			SizeType wid = size / 2;
			if (pos.x >= MAX_WIDTH - wid || pos.y >= MAX_HEIGHT - size) return;

			// ����������򲢱����������һ�ι�����
			CoordType x_start = pos.x, x_end = pos.x + wid;
			CoordType row_start = pos.y / 8, row_end = (pos.y + size) / 8;

			CoordType lineOfChar = 0;      
			ch -= ' ';											//ƫ�ƻ���
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
					pos.x -= wid;								// �ص����ַ���һ����ʼ��
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

			// �����������
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
		 * @brief	���ڴ��е������Դ���µ��豸�Դ���
		 *			row_start, row_end 5110����ʼ�кź���ֹ�к�
		 *			x_start, x_end 5110����ʼ�кź���ֹ�к�
		 *				���ϲ�����Ϊ���������
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
		 * @brief	�������ݻ�����ѡ��
		 */
		enum DTCMD
		{
			COMMAND,
			DATA
		};



		/*
		 * @brief	Һ���������Ĵ���д��
		 * @param	dc �����������ѡ�������DTCMDö���е��κ�һ��
		 *			val ����ֵ����������ֵ��depends on dc
		 */
		void WriteReg(const DTCMD dc, UINT8 val)
		{
			STATIC_ASSERT(DATA == 1 && COMMAND == 0,
				enumerate_is_not_campacted_with_this_configuration);
			DC = dc;
			SPI.SynchronouslyTranceive(val);
		}




		/*
		 * @brief	Һ�����������ù��λ��
		 * @param	x �����꣨�У�����Χ[0,83]
		 *			x �����꣨�У�����Χ[0,5]
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
		 * @brief	���ڴ棨�Դ棩��д�����ݣ��������µ���Ļ��
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


	

		SynchronousCommunicator& SPI;					// ģ��SPIͨѶ�ӿ�
		GeneralPurposeInputOutputPin& DC;				// ָ��������ѡ���ź�
		GeneralPurposeInputOutputPin& RST;				// ��λ�ź�
		GeneralPurposeInputOutputPin& BackLight;		// �����Դ
		BlockingDelayController& MCU;					// ϵͳ��ʱ


	private:

		UINT8 data[MAX_HEIGHT / 8][MAX_WIDTH];

	};

}


#endif  /*_ADX_CM_NOKIA5110_H_*/
