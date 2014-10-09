
#ifndef _ADX_CM_ILIX_H_
#define _ADX_CM_ILIX_H_

//////////////////////////////////////////////////////////////////////////
//	
//////////////////////////////////////////////////////////////////////////

#include "../common/assert.h"
#include "../HadwareAbstraction/Communicator/SynchronousCommunicator.h"
#include "../HadwareAbstraction/GPIO/GeneralPurposeInputOutputPin.h"
#include "../HadwareAbstraction/Timer/BlockingDelayController.h"
#include "../HadwareAbstraction/GDI/GraphicsDevice.h"


namespace adxM3
{



	template < class T >
	struct _CS
	{
		typedef T Result;
	};

	template < class T >
	struct _RS
	{
		typedef T Result;
	};

	template < class T >
	struct _WR
	{
		typedef T Result;
	};

	template < class T >
	struct _RD
	{
		typedef T Result;
	};

	template < class T >
	struct _DataPort
	{
		typedef T Result;
	};



	/* 
	 * 9320/9325 LCD寄存器  
	 */
	enum ILI932x_REG
	{
		R0					= 0x00,
		R1					= 0x01,
		R2					= 0x02,
		R3					= 0x03,
		R4					= 0x04,
		R5					= 0x05,
		R6					= 0x06,
		R7					= 0x07,
		R8					= 0x08,
		R9					= 0x09,
		R10					= 0x0A,
		R12					= 0x0C,
		R13					= 0x0D,
		R14					= 0x0E,
		R15					= 0x0F,
		R16					= 0x10,
		R17					= 0x11,
		R18					= 0x12,
		R19					= 0x13,
		R20					= 0x14,
		R21					= 0x15,
		R22					= 0x16,
		R23					= 0x17,
		R24					= 0x18,
		R25					= 0x19,
		R26					= 0x1A,
		R27					= 0x1B,
		R28					= 0x1C,
		R29					= 0x1D,
		R30					= 0x1E,
		R31					= 0x1F,
		R32					= 0x20,
		R33					= 0x21,
		R34					= 0x22,
		R36					= 0x24,
		R37					= 0x25,
		R40					= 0x28,
		R41					= 0x29,
		R43					= 0x2B,
		R45					= 0x2D,
		R48					= 0x30,
		R49					= 0x31,
		R50					= 0x32,
		R51					= 0x33,
		R52					= 0x34,
		R53					= 0x35,
		R54					= 0x36,
		R55					= 0x37,
		R56					= 0x38,
		R57					= 0x39,
		R59					= 0x3B,
		R60					= 0x3C,
		R61					= 0x3D,
		R62					= 0x3E,
		R63					= 0x3F,
		R64					= 0x40,
		R65					= 0x41,
		R66					= 0x42,
		R67					= 0x43,
		R68					= 0x44,
		R69					= 0x45,
		R70					= 0x46,
		R71					= 0x47,
		R72					= 0x48,
		R73					= 0x49,
		R74					= 0x4A,
		R75					= 0x4B,
		R76					= 0x4C,
		R77					= 0x4D,
		R78					= 0x4E,
		R79					= 0x4F,
		R80					= 0x50,
		R81					= 0x51,
		R82					= 0x52,
		R83					= 0x53,
		R96					= 0x60,
		R97					= 0x61,
		R106				= 0x6A,
		R118				= 0x76,
		R128				= 0x80,
		R129				= 0x81,
		R130				= 0x82,
		R131				= 0x83,
		R132				= 0x84,
		R133				= 0x85,
		R134				= 0x86,
		R135				= 0x87,
		R136				= 0x88,
		R137				= 0x89,
		R139				= 0x8B,
		R140				= 0x8C,
		R141				= 0x8D,
		R143				= 0x8F,
		R144				= 0x90,
		R145				= 0x91,
		R146				= 0x92,
		R147				= 0x93,
		R148				= 0x94,
		R149				= 0x95,
		R150				= 0x96,
		R151				= 0x97,
		R152				= 0x98,
		R153				= 0x99,
		R154				= 0x9A,
		R157				= 0x9D,
		R192				= 0xC0,
		R193				= 0xC1,
		R229				= 0xE5
	};




	template
	<
		UINT32 DeviceID,
		class CSSignal,
		class RSSignal,
		class WRSignal,
		class RDSignal,
		class DataPortType,
		class SystemDelayType
	>
	class ILIx;





	/*
	 * @brief	ILIx 系列LCD驱动器抽象
	 * @tparam	DeviceID 驱动器系列ID
	 *				@arg 9320, 9325, 9328...
	 *			CSSignal 片选信号
	 *			RSSignal 数据/命令选择信号
	 *			WRSignal 读信号
	 *			RDSignal 写信号
	 *				信号类型需实现为GeneralPurposedInputOutputPin
	 *			DataPort 数据端口，并行端口
	 *				数据端口需实现为GeneralPurposedInputOutputPort
	 */
	template
	<
		UINT32 DeviceID,
		class CSSignal,
		class RSSignal,
		class WRSignal,
		class RDSignal,
		class DataPortType,
		class SystemDelayType
	>
	class ILIx
	<
		DeviceID,
		_CS<CSSignal>,
		_RS<RSSignal>,
		_WR<WRSignal>,
		_RD<RDSignal>,
		_DataPort<DataPortType>,
		SystemDelayType

	>
		: public GraphicsDevice<INT16, RGB565Type>
	{
	public:


		typedef INT16 CoordType;
		typedef RGB565Type ColorType;


		enum
		{
			MAX_WIDTH = 320,
			MAX_HEIGHT = 240,
		};


		


		ILIx()
		{	
			CS.OutputConfig(GeneralPurposeInputOutput::PushPull);
			RS.OutputConfig(GeneralPurposeInputOutput::PushPull);
			WR.OutputConfig(GeneralPurposeInputOutput::PushPull);
			RD.OutputConfig(GeneralPurposeInputOutput::PushPull);
			
			DataPort.OutputConfig(GeneralPurposeInputOutput::PushPull);

			CS = 1;
			RS = 1;
			WR = 1;
			RD = 1;

			FirstRunPrepare();
			Clear();
			DisplayOn();
		}





		/*
		 * @brief	图形设备画点/读点方法
		 * @param	x, y 点坐标
		 *			color 写入该点的颜色值
		 * @retval	该点的颜色值
		 */
		virtual void SetPixel(CoordType x, CoordType y, ColorType color)
		{
			SetCursor(x, y);			// 设置光标位置 
			if (DeviceID == 0x9341) 
			{
				//ILI9341是以0X2C开始的
				WriteReg(0x2C, color);
			} 
			else 
			{
				WriteReg(R34, color);
			}
		}
		virtual ColorType GetPixel(CoordType x, CoordType y)
		{
			// 坐标范围检测，超过了范围，直接返回
			if (x >= MAX_WIDTH || y >= MAX_HEIGHT) return 0;

			UINT16 r = 0, g = 0, b = 0;
			SetCursor(x, y);

			// 读GRAM前准备
			if (DeviceID == 0x9341) 
			{
				IndexRegister(0x2E);						// ILI9341发送读GRAM指令
			} 
			else 
			{
				IndexRegister(R34);      		 			// 其他IC发送读GRAM指令
			}

			r = ReadData();  
			if (DeviceID==0X9341) {	 
				RD = 0;					   
				RD = 1;
				b = ReadData();		// 读取蓝色值  	  
				g = r & 0xFF;		// 对于9341,第一次读取的是RG的值,R在前,G在后,各占8位
				g <<= 8;
			}

			if (DeviceID == 0x4535 || DeviceID==0x4531 || DeviceID==0x8989 || DeviceID==0xB505)
			{
				return r;														// 这几种IC直接返回颜色值
			}
			else if (DeviceID == 0X9341) 
			{ 
				return (((r>>11)<<11)|((g>>10)<<5)|(b>>11));					// ILI9341需要公式转换一下
			}
			else 
			{
				return BGR2RGB(r);												// 其他IC
			}
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
			SetCursor(x, y);
			if (DeviceID == 0x9341) 
			{
				IndexRegister(0x2C);
			} 
			else 
			{
				IndexRegister(R34);
			}

			RS = 1;
			CS = 0;
			for (UINT32 i = 0; i < length; ++i) 
			{
				DataPort = *color++;
				WR = 0;
				WR = 1;
			}
			CS = 1;
		}
		virtual void SequentialSetPixel(CoordType x, CoordType y, ColorType const_color, unsigned int length)
		{
			SetCursor(x, y);
			
			if (DeviceID == 0x9341) 
			{
				IndexRegister(0x2C);
			} 
			else 
			{
				IndexRegister(R34);
			}

			RS = 1;
			CS = 0;
			for (UINT32 i = 0; i < length; ++i) 
			{
				DataPort = const_color;
				WR = 0;
				WR = 1;
			}
			CS = 1;
		}
		


		/*
		 * @brief	清除屏幕全部内容
		 */
		virtual void Clear()
		{
			SequentialSetPixel(0, 0, (ColorType)0, MAX_WIDTH * MAX_HEIGHT);
		}
		virtual void Clear(CoordType left, CoordType top, CoordType right, CoordType bottom)
		{
			// 确定输出的最大最小边界，避免超出屏幕
			CoordType xStart = left < 0 ? 0 : left > MAX_WIDTH ? MAX_WIDTH : left;
			CoordType yStart = top < 0 ? 0 : top > MAX_HEIGHT ? MAX_HEIGHT : top;
			CoordType xEnd = min(right, (CoordType)MAX_WIDTH);
			CoordType yEnd = min(bottom, (CoordType)MAX_HEIGHT);

			for (CoordType i = yStart; i < yEnd; ++i) 
			{
				SequentialSetPixel(xStart, i, (ColorType)0, xEnd - xStart);
			}
		}




		/*
		 * @brief	显示屏开/关
		 */
		void DisplayOn()
		{
			if (DeviceID == 0x9341) 
			{
				IndexRegister(0x29);
			} 
			else 
			{
				WriteReg(R7, 0x0173);
			}
		}
		void DisplayOff()
		{
			if (DeviceID == 0x9341) 
			{
				IndexRegister(0x28);
			} 
			else 
			{
				WriteReg(R7, 0x00);
			}
		}




	private:





		/*
		 * @brief	液晶驱动器设置光标位置
		 * @param	x 横坐标 [0, MAX_WIDTH)
		 *			y 纵坐标 [0, MAX_HEIGHT)
		 */
		void SetCursor(int x, int y)
		{
			if (DeviceID == 0x8989) 
			{
				WriteReg(0x4E, x);
				WriteReg(0x4F, y);	// i swaped x,y position register
			} 
			else if (DeviceID == 0x9341) 
			{		
				IndexRegister(0x2B); 
				WriteData(y >> 8); 
				WriteData(y & 0xFF);			 
				IndexRegister(0x2A); 
				WriteData(x >> 8); 
				WriteData(x & 0xFF);
			} 
			else 
			{
				// 暂时在这里设置映射方向的坐标变换
				//y = MAX_HEIGHT - 1 - y;
				x = MAX_WIDTH - 1 - x;
				WriteReg(R32, y);
				WriteReg(R33, x);
			}	
		}







		/*
		 * @brief	快速端口读数据
		 * @retval	数据端口返回的16位数据	
		 */
		UINT16 ReadData()
		{
			DataPort.SetDirection(DataPort.IN);
			DataPort.InputConfig(DataPort.PullUp);

			RS = 1;
			CS = 0;
			// 读取数据(读寄存器时,并不需要读2次)
			RD = 0;		
			if (DeviceID == 0x8989) SystemDelayType::StaticDelay_us(2);	//FOR 8989,延时2us
			RD = 1;
			UINT16 result = DataPort.GetValue();		// 一定要这样？？
			//UINT16 result = DataPort;
			CS = 1; 

			DataPort.SetDirection(DataPort.OUT);
			DataPort.OutputConfig(DataPort.PushPull);
			return result;  
		}



		/*
		 * @brief	读写寄存器
		 * @param	reg 寄存器地址
					val	写入的值
		 * @retval	寄存器的值
		 */
		void WriteReg(UINT8 reg, UINT16 val)
		{
			CS = 0;

			// 索引寄存器
			RS = 0;
			DataPort = reg; 		
			WR = 0;
			WR = 1;

			// 写入寄存器
			RS = 1;
			DataPort = val;
			WR = 0;
			WR = 1;

			CS = 1;
		}
		UINT16 ReadReg(UINT8 reg)
		{
			CS = 0;
			
			RS = 0;
			DataPort = reg; 		
			WR = 0;
			WR = 1;

			DataPort.SetDirection(DataPort.IN);
			DataPort.InputConfig(DataPort.PullUp);

			RS = 1;
			RD = 0;		
			if (DeviceID == 0x8989) SystemDelayType::StaticDelay_us(2);	//FOR 8989,延时2us
			RD = 1;
			UINT16 result = DataPort.GetInputValue();	// 一定要这样？？
			//typename DataPortType::PortValueType result = DataPort;
			CS = 1; 

			DataPort.SetDirection(DataPort.OUT);
			DataPort.OutputConfig(DataPort.PushPull);
			return result;  
		}





		/*
		 * @brief	快速端口写入数据/命令
		 * @notice	data 16位并口数据
		 */
		void IndexRegister(UINT16 data)
		{	
			RS = 0;
			CS = 0;
			DataPort = data; 		
			WR = 0;
			WR = 1;
			CS = 1;
		}
		void WriteData(UINT16 data)
		{
			RS = 1;
			CS = 0;
			DataPort = data; 		
			WR = 0;
			WR = 1;
			CS = 1;
		}



		/*
		 * @brief	将BGR格式的颜色值转换到RGB格式
		 * @param	RGB_Code 16位RGB颜色代码
		 */
		ColorType BGR2RGB(ColorType bgr)
		{
			// 当前代码仅为RGB565模式
			UINT16 b = (bgr >> 0) & 0x1f;
			UINT16 g = (bgr >> 5) & 0x3f;
			UINT16 r = (bgr >> 11) & 0x1f;	 
			return (b << 11) | (g << 5) | (r << 0);		 
		}




	public:


		/*
		 * 屏幕输出扫描方向（映射模式）
		 */
		enum SCANDIR
		{
			L2R_U2D						= 0,												//从左到右,从上到下
			L2R_D2U						= 1,												//从左到右,从下到上
			R2L_U2D						= 2,												//从右到左,从上到下
			R2L_D2U						= 3,												//从右到左,从下到上
			U2D_L2R						= 4,												//从上到下,从左到右
			U2D_R2L						= 5,												//从上到下,从右到左
			D2U_L2R						= 6,												//从下到上,从左到右
			D2U_R2L						= 7,												//从下到上,从右到左
			DFT_SCAN_DIR				= L2R_U2D											//默认的扫描方向
		};



		/*
		 * @brief	设置扫描方向
		 * @param	dir 可选的方向枚举
		 */
		void SetScanDir(SCANDIR dir)
		{
			UINT16 regval=0;
			UINT8 dirreg=0;
			if (DeviceID == 0x9341) {
				//9341,很特殊
				switch(dir)
				{
				case L2R_U2D://从左到右,从上到下
					regval|=(0<<7)|(0<<6)|(0<<5); 
					break;
				case L2R_D2U://从左到右,从下到上
					regval|=(1<<7)|(0<<6)|(0<<5); 
					break;
				case R2L_U2D://从右到左,从上到下
					regval|=(0<<7)|(1<<6)|(0<<5); 
					break;
				case R2L_D2U://从右到左,从下到上
					regval|=(1<<7)|(1<<6)|(0<<5); 
					break;	 
				case U2D_L2R://从上到下,从左到右
					regval|=(0<<7)|(0<<6)|(1<<5); 
					break;
				case U2D_R2L://从上到下,从右到左
					regval|=(0<<7)|(1<<6)|(1<<5); 
					break;
				case D2U_L2R://从下到上,从左到右
					regval|=(1<<7)|(0<<6)|(1<<5); 
					break;
				case D2U_R2L://从下到上,从右到左
					regval|=(1<<7)|(1<<6)|(1<<5); 
					break;	 
				}
				dirreg = 0x36;
				regval |= 0x08; //BGR   
				WriteReg(dirreg,regval);
				if(regval & 0x20) 
				{
					IndexRegister(0x2A); 
					WriteData(0);
					WriteData(0);
					WriteData(319>>8);
					WriteData(319&0XFF);
					IndexRegister(0x2B); 
					WriteData(0);
					WriteData(0);
					WriteData(239>>8);
					WriteData(239&0XFF);
				} 
				else 
				{
					IndexRegister(0x2A); 
					WriteData(0);
					WriteData(0);
					WriteData(239 >> 8);
					WriteData(239 & 0xFF);
					IndexRegister(0x2B); 
					WriteData(0);
					WriteData(0);
					WriteData(319 >> 8);
					WriteData(319 & 0xFF);
				}  
			} 
			else 
			{
				switch(dir)
				{
				case L2R_U2D://从左到右,从上到下
					regval|=(1<<5)|(1<<4)|(0<<3); 
					break;
				case L2R_D2U://从左到右,从下到上
					regval|=(0<<5)|(1<<4)|(0<<3); 
					break;
				case R2L_U2D://从右到左,从上到下
					regval|=(1<<5)|(0<<4)|(0<<3);
					break;
				case R2L_D2U://从右到左,从下到上
					regval|=(0<<5)|(0<<4)|(0<<3); 
					break;	 
				case U2D_L2R://从上到下,从左到右
					regval|=(1<<5)|(1<<4)|(1<<3); 
					break;
				case U2D_R2L://从上到下,从右到左
					regval|=(1<<5)|(0<<4)|(1<<3); 
					break;
				case D2U_L2R://从下到上,从左到右
					regval|=(0<<5)|(1<<4)|(1<<3); 
					break;
				case D2U_R2L://从下到上,从右到左
					regval|=(0<<5)|(0<<4)|(1<<3); 
					break;	 
				}
				if (DeviceID==0x8989) 
				{
					dirreg=0x11;
					regval|=0X6040;	//65K   
				} 
				else 
				{
					dirreg=0x03;
					regval|=1<<12;  
				}
				WriteReg(dirreg,regval);
			}
		}




	protected:
		public:

		
		CSSignal CS;
		RSSignal RS;
		WRSignal WR;
		RDSignal RD;
		DataPortType DataPort;


	public:



		void FirstRunPrepare()
		{
			if (DeviceID==0X9341) 
			{
				IndexRegister(0xCF);  
				WriteData(0x00); 
				WriteData(0xC1); 
				WriteData(0X30); 
				IndexRegister(0xED);  
				WriteData(0x64); 
				WriteData(0x03); 
				WriteData(0X12); 
				WriteData(0X81); 
				IndexRegister(0xE8);  
				WriteData(0x85); 
				WriteData(0x10); 
				WriteData(0x7A); 
				IndexRegister(0xCB);  
				WriteData(0x39); 
				WriteData(0x2C); 
				WriteData(0x00); 
				WriteData(0x34); 
				WriteData(0x02); 
				IndexRegister(0xF7);  
				WriteData(0x20); 
				IndexRegister(0xEA);  
				WriteData(0x00); 
				WriteData(0x00); 
				IndexRegister(0xC0);    //Power control 
				WriteData(0x1B);   //VRH[5:0] 
				IndexRegister(0xC1);    //Power control 
				WriteData(0x01);   //SAP[2:0];BT[3:0] 
				IndexRegister(0xC5);    //VCM control 
				WriteData(0x30); 	 //3F
				WriteData(0x30); 	 //3C
				IndexRegister(0xC7);    //VCM control2 
				WriteData(0XB7); 
				IndexRegister(0x36);    // Memory Access Control 
				WriteData(0x48); 
				IndexRegister(0x3A);   
				WriteData(0x55); 
				IndexRegister(0xB1);   
				WriteData(0x00);   
				WriteData(0x1A); 
				IndexRegister(0xB6);    // Display Function Control 
				WriteData(0x0A); 
				WriteData(0xA2); 
				IndexRegister(0xF2);    // 3Gamma Function Disable 
				WriteData(0x00); 
				IndexRegister(0x26);    //Gamma curve selected 
				WriteData(0x01); 
				IndexRegister(0xE0);    //Set Gamma 
				WriteData(0x0F); 
				WriteData(0x2A); 
				WriteData(0x28); 
				WriteData(0x08); 
				WriteData(0x0E); 
				WriteData(0x08); 
				WriteData(0x54); 
				WriteData(0XA9); 
				WriteData(0x43); 
				WriteData(0x0A); 
				WriteData(0x0F); 
				WriteData(0x00); 
				WriteData(0x00); 
				WriteData(0x00); 
				WriteData(0x00); 		 
				IndexRegister(0XE1);    //Set Gamma 
				WriteData(0x00); 
				WriteData(0x15); 
				WriteData(0x17); 
				WriteData(0x07); 
				WriteData(0x11); 
				WriteData(0x06); 
				WriteData(0x2B); 
				WriteData(0x56); 
				WriteData(0x3C); 
				WriteData(0x05); 
				WriteData(0x10); 
				WriteData(0x0F); 
				WriteData(0x3F); 
				WriteData(0x3F); 
				WriteData(0x0F); 
				IndexRegister(0x2B); 
				WriteData(0x00);
				WriteData(0x00);
				WriteData(0x01);
				WriteData(0x3f);
				IndexRegister(0x2A); 
				WriteData(0x00);
				WriteData(0x00);
				WriteData(0x00);
				WriteData(0xef);	 
				IndexRegister(0x11); //Exit Sleep
				SystemDelayType::StaticDelay_ms(120);
				IndexRegister(0x29); //display on	
			} else if (DeviceID==0x9325) {
				WriteReg(0x00E5,0x78F0); 
				WriteReg(0x0001,0x0100); 
				WriteReg(0x0002,0x0700); 
				WriteReg(0x0003,0x1030); 
				WriteReg(0x0004,0x0000); 
				WriteReg(0x0008,0x0202);  
				WriteReg(0x0009,0x0000);
				WriteReg(0x000A,0x0000); 
				WriteReg(0x000C,0x0000); 
				WriteReg(0x000D,0x0000);
				WriteReg(0x000F,0x0000);
				//power on sequence VGHVGL
				WriteReg(0x0010,0x0000);   
				WriteReg(0x0011,0x0007);  
				WriteReg(0x0012,0x0000);  
				WriteReg(0x0013,0x0000); 
				WriteReg(0x0007,0x0000); 
				//vgh 
				WriteReg(0x0010,0x1690);   
				WriteReg(0x0011,0x0227);
				//delayms(100);
				//vregiout 
				WriteReg(0x0012,0x009D); //0x001b
				//delayms(100); 
				//vom amplitude
				WriteReg(0x0013,0x1900);
				//delayms(100); 
				//vom H
				WriteReg(0x0029,0x0025); 
				WriteReg(0x002B,0x000D); 
				//gamma
				WriteReg(0x0030,0x0007);
				WriteReg(0x0031,0x0303);
				WriteReg(0x0032,0x0003);// 0006
				WriteReg(0x0035,0x0206);
				WriteReg(0x0036,0x0008);
				WriteReg(0x0037,0x0406); 
				WriteReg(0x0038,0x0304);//0200
				WriteReg(0x0039,0x0007); 
				WriteReg(0x003C,0x0602);// 0504
				WriteReg(0x003D,0x0008); 
				//ram
				WriteReg(0x0050,0x0000); 
				WriteReg(0x0051,0x00EF);
				WriteReg(0x0052,0x0000); 
				WriteReg(0x0053,0x013F);  
				WriteReg(0x0060,0xA700); 
				WriteReg(0x0061,0x0001); 
				WriteReg(0x006A,0x0000); 
				//
				WriteReg(0x0080,0x0000); 
				WriteReg(0x0081,0x0000); 
				WriteReg(0x0082,0x0000); 
				WriteReg(0x0083,0x0000); 
				WriteReg(0x0084,0x0000); 
				WriteReg(0x0085,0x0000); 
				//
				WriteReg(0x0090,0x0010); 
				WriteReg(0x0092,0x0600); 

				WriteReg(0x0007,0x0133);
				WriteReg(0x00,0x0022);//	
			} else if (DeviceID==0x9328) {
				WriteReg(0x00e7,0x0010);      
				WriteReg(0x0000,0x0001);//开启内部时钟
				WriteReg(0x0001,0x0100);     
				WriteReg(0x0002,0x0700);//电源开启                    
				//WriteReg(0x0003,(1<<3)|(1<<4)); 	//65K  RGB
				//DRIVE TABLE(寄存器 03H)
				//BIT3=AM BIT4:5=ID0:1
				//AM ID0 ID1   FUNCATION
				// 0  0   0	   R->L D->U
				// 1  0   0	   D->U	R->L
				// 0  1   0	   L->R D->U
				// 1  1   0    D->U	L->R
				// 0  0   1	   R->L U->D
				// 1  0   1    U->D	R->L
				// 0  1   1    L->R U->D 正常就用这个.
				// 1  1   1	   U->D	L->R
				WriteReg(0x0003,(1<<12)|(3<<4)|(0<<3));//65K    
				WriteReg(0x0004,0x0000);                                   
				WriteReg(0x0008,0x0207);	           
				WriteReg(0x0009,0x0000);         
				WriteReg(0x000a,0x0000);//display setting         
				WriteReg(0x000c,0x0001);//display setting          
				WriteReg(0x000d,0x0000);//0f3c          
				WriteReg(0x000f,0x0000);
				//电源配置
				WriteReg(0x0010,0x0000);   
				WriteReg(0x0011,0x0007);
				WriteReg(0x0012,0x0000);                                                                 
				WriteReg(0x0013,0x0000);                 
				SystemDelayType::StaticDelay_ms(50); 
				WriteReg(0x0010,0x1590);   
				WriteReg(0x0011,0x0227);
				SystemDelayType::StaticDelay_ms(50); 
				WriteReg(0x0012,0x009c);                  
				SystemDelayType::StaticDelay_ms(50); 
				WriteReg(0x0013,0x1900);   
				WriteReg(0x0029,0x0023);
				WriteReg(0x002b,0x000e);
				SystemDelayType::StaticDelay_ms(50); 
				WriteReg(0x0020,0x0000);                                                            
				WriteReg(0x0021,0x013f);           
				SystemDelayType::StaticDelay_ms(50); 
				//伽马校正
				WriteReg(0x0030,0x0007); 
				WriteReg(0x0031,0x0707);   
				WriteReg(0x0032,0x0006);
				WriteReg(0x0035,0x0704);
				WriteReg(0x0036,0x1f04); 
				WriteReg(0x0037,0x0004);
				WriteReg(0x0038,0x0000);        
				WriteReg(0x0039,0x0706);     
				WriteReg(0x003c,0x0701);
				WriteReg(0x003d,0x000f);
				SystemDelayType::StaticDelay_ms(50); 
				WriteReg(0x0050,0x0000); //水平GRAM起始位置 
				WriteReg(0x0051,0x00ef); //水平GRAM终止位置                    
				WriteReg(0x0052,0x0000); //垂直GRAM起始位置                    
				WriteReg(0x0053,0x013f); //垂直GRAM终止位置  

				WriteReg(0x0060,0xa700);        
				WriteReg(0x0061,0x0001); 
				WriteReg(0x006a,0x0000);
				WriteReg(0x0080,0x0000);
				WriteReg(0x0081,0x0000);
				WriteReg(0x0082,0x0000);
				WriteReg(0x0083,0x0000);
				WriteReg(0x0084,0x0000);
				WriteReg(0x0085,0x0000);

				WriteReg(0x0090,0x0010);     
				WriteReg(0x0092,0x0000);  
				WriteReg(0x0093,0x0003);
				WriteReg(0x0095,0x0110);
				WriteReg(0x0097,0x0000);        
				WriteReg(0x0098,0x0000);  
				//开启显示设置    
				WriteReg(0x0007,0x0133);   
				WriteReg(0x0020,0x0000);                                                            
				WriteReg(0x0021,0x013f);
			} else if (DeviceID==0x9320 || DeviceID==0x9300) {
				WriteReg(0x00,0x0000);
				WriteReg(0x01,0x0100);	//Driver Output Contral.
				WriteReg(0x02,0x0700);	//LCD Driver Waveform Contral.
				WriteReg(0x03,0x1030);  //Entry Mode Set.
				//WriteReg(0x03,0x1018);	//Entry Mode Set.

				WriteReg(0x04,0x0000);	//Scalling Contral.
				WriteReg(0x08,0x0202);	//Display Contral 2.(0x0207)
				WriteReg(0x09,0x0000);	//Display Contral 3.(0x0000)
				WriteReg(0x0a,0x0000);	//Frame Cycle Contal.(0x0000)
				WriteReg(0x0c,(1<<0));	//Extern Display Interface Contral 1.(0x0000)
				WriteReg(0x0d,0x0000);	//Frame Maker Position.
				WriteReg(0x0f,0x0000);	//Extern Display Interface Contral 2.	    
				SystemDelayType::StaticDelay_ms(50); 
				WriteReg(0x07,0x0101);	//Display Contral.
				SystemDelayType::StaticDelay_ms(50); 								  
				WriteReg(0x10,(1<<12)|(0<<8)|(1<<7)|(1<<6)|(0<<4));	//Power Control 1.(0x16b0)
				WriteReg(0x11,0x0007);								//Power Control 2.(0x0001)
				WriteReg(0x12,(1<<8)|(1<<4)|(0<<0));				//Power Control 3.(0x0138)
				WriteReg(0x13,0x0b00);								//Power Control 4.
				WriteReg(0x29,0x0000);								//Power Control 7.

				WriteReg(0x2b,(1<<14)|(1<<4));	    
				WriteReg(0x50,0);	//Set X Star
				//水平GRAM终止位置Set X End.
				WriteReg(0x51,239);	//Set Y Star
				WriteReg(0x52,0);	//Set Y End.t.
				WriteReg(0x53,319);	//

				WriteReg(0x60,0x2700);	//Driver Output Control.
				WriteReg(0x61,0x0001);	//Driver Output Control.
				WriteReg(0x6a,0x0000);	//Vertical Srcoll Control.

				WriteReg(0x80,0x0000);	//Display Position? Partial Display 1.
				WriteReg(0x81,0x0000);	//RAM Address Start? Partial Display 1.
				WriteReg(0x82,0x0000);	//RAM Address End-Partial Display 1.
				WriteReg(0x83,0x0000);	//Displsy Position? Partial Display 2.
				WriteReg(0x84,0x0000);	//RAM Address Start? Partial Display 2.
				WriteReg(0x85,0x0000);	//RAM Address End? Partial Display 2.

				WriteReg(0x90,(0<<7)|(16<<0));	//Frame Cycle Contral.(0x0013)
				WriteReg(0x92,0x0000);	//Panel Interface Contral 2.(0x0000)
				WriteReg(0x93,0x0001);	//Panel Interface Contral 3.
				WriteReg(0x95,0x0110);	//Frame Cycle Contral.(0x0110)
				WriteReg(0x97,(0<<8));	//
				WriteReg(0x98,0x0000);	//Frame Cycle Contral.	   
				WriteReg(0x07,0x0173);	//(0x0173)
			} else if (DeviceID==0x5408) {
				WriteReg(0x01,0x0100);								  
				WriteReg(0x02,0x0700);//LCD Driving Waveform Contral 
				WriteReg(0x03,0x1030);//Entry Mode设置 	   
				//指针从左至右自上而下的自动增模式
				//Normal Mode(Window Mode disable)
				//RGB格式
				//16位数据2次传输的8总线设置
				WriteReg(0x04,0x0000); //Scalling Control register     
				WriteReg(0x08,0x0207); //Display Control 2 
				WriteReg(0x09,0x0000); //Display Control 3	 
				WriteReg(0x0A,0x0000); //Frame Cycle Control	 
				WriteReg(0x0C,0x0000); //External Display Interface Control 1 
				WriteReg(0x0D,0x0000); //Frame Maker Position		 
				WriteReg(0x0F,0x0000); //External Display Interface Control 2 
				SystemDelayType::StaticDelay_ms(20);
				//TFT 液晶彩色图像显示方法14
				WriteReg(0x10,0x16B0); //0x14B0 //Power Control 1
				WriteReg(0x11,0x0001); //0x0007 //Power Control 2
				WriteReg(0x17,0x0001); //0x0000 //Power Control 3
				WriteReg(0x12,0x0138); //0x013B //Power Control 4
				WriteReg(0x13,0x0800); //0x0800 //Power Control 5
				WriteReg(0x29,0x0009); //NVM read data 2
				WriteReg(0x2a,0x0009); //NVM read data 3
				WriteReg(0xa4,0x0000);	 
				WriteReg(0x50,0x0000); //设置操作窗口的X轴开始列
				WriteReg(0x51,0x00EF); //设置操作窗口的X轴结束列
				WriteReg(0x52,0x0000); //设置操作窗口的Y轴开始行
				WriteReg(0x53,0x013F); //设置操作窗口的Y轴结束行
				WriteReg(0x60,0x2700); //Driver Output Control
				//设置屏幕的点数以及扫描的起始行
				WriteReg(0x61,0x0001); //Driver Output Control
				WriteReg(0x6A,0x0000); //Vertical Scroll Control
				WriteReg(0x80,0x0000); //Display Position – Partial Display 1
				WriteReg(0x81,0x0000); //RAM Address Start – Partial Display 1
				WriteReg(0x82,0x0000); //RAM address End - Partial Display 1
				WriteReg(0x83,0x0000); //Display Position – Partial Display 2
				WriteReg(0x84,0x0000); //RAM Address Start – Partial Display 2
				WriteReg(0x85,0x0000); //RAM address End – Partail Display2
				WriteReg(0x90,0x0013); //Frame Cycle Control
				WriteReg(0x92,0x0000);  //Panel Interface Control 2
				WriteReg(0x93,0x0003); //Panel Interface control 3
				WriteReg(0x95,0x0110);  //Frame Cycle Control
				WriteReg(0x07,0x0173);		 
				SystemDelayType::StaticDelay_ms(50);
			} else if (DeviceID==0x1505) {
				// second release on 3/5  ,luminance is acceptable,water wave appear during camera preview
				WriteReg(0x0007,0x0000);
				SystemDelayType::StaticDelay_ms(50); 
				WriteReg(0x0012,0x011C);//0x011A   why need to set several times?
				WriteReg(0x00A4,0x0001);//NVM	 
				WriteReg(0x0008,0x000F);
				WriteReg(0x000A,0x0008);
				WriteReg(0x000D,0x0008);	    
				//伽马校正
				WriteReg(0x0030,0x0707);
				WriteReg(0x0031,0x0007); //0x0707
				WriteReg(0x0032,0x0603); 
				WriteReg(0x0033,0x0700); 
				WriteReg(0x0034,0x0202); 
				WriteReg(0x0035,0x0002); //?0x0606
				WriteReg(0x0036,0x1F0F);
				WriteReg(0x0037,0x0707); //0x0f0f  0x0105
				WriteReg(0x0038,0x0000); 
				WriteReg(0x0039,0x0000); 
				WriteReg(0x003A,0x0707); 
				WriteReg(0x003B,0x0000); //0x0303
				WriteReg(0x003C,0x0007); //?0x0707
				WriteReg(0x003D,0x0000); //0x1313//0x1f08
				SystemDelayType::StaticDelay_ms(50); 
				WriteReg(0x0007,0x0001);
				WriteReg(0x0017,0x0001);//开启电源
				SystemDelayType::StaticDelay_ms(50); 
				//电源配置
				WriteReg(0x0010,0x17A0); 
				WriteReg(0x0011,0x0217);//reference voltage VC[2:0]   Vciout = 1.00*Vcivl
				WriteReg(0x0012,0x011E);//0x011c  //Vreg1out = Vcilvl*1.80   is it the same as Vgama1out ?
				WriteReg(0x0013,0x0F00);//VDV[4:0]-->VCOM Amplitude VcomL = VcomH - Vcom Ampl
				WriteReg(0x002A,0x0000);  
				WriteReg(0x0029,0x000A);//0x0001F  Vcomh = VCM1[4:0]*Vreg1out    gate source voltage??
				WriteReg(0x0012,0x013E);// 0x013C  power supply on
				//Coordinates Control//
				WriteReg(0x0050,0x0000);//0x0e00
				WriteReg(0x0051,0x00EF); 
				WriteReg(0x0052,0x0000); 
				WriteReg(0x0053,0x013F); 
				//Pannel Image Control//
				WriteReg(0x0060,0x2700); 
				WriteReg(0x0061,0x0001); 
				WriteReg(0x006A,0x0000); 
				WriteReg(0x0080,0x0000); 
				//Partial Image Control//
				WriteReg(0x0081,0x0000); 
				WriteReg(0x0082,0x0000); 
				WriteReg(0x0083,0x0000); 
				WriteReg(0x0084,0x0000); 
				WriteReg(0x0085,0x0000); 
				//Panel Interface Control//
				WriteReg(0x0090,0x0013);//0x0010 frenqucy
				WriteReg(0x0092,0x0300); 
				WriteReg(0x0093,0x0005); 
				WriteReg(0x0095,0x0000); 
				WriteReg(0x0097,0x0000); 
				WriteReg(0x0098,0x0000); 

				WriteReg(0x0001,0x0100); 
				WriteReg(0x0002,0x0700); 
				WriteReg(0x0003,0x1030); 
				WriteReg(0x0004,0x0000); 
				WriteReg(0x000C,0x0000); 
				WriteReg(0x000F,0x0000); 
				WriteReg(0x0020,0x0000); 
				WriteReg(0x0021,0x0000); 
				WriteReg(0x0007,0x0021); 
				SystemDelayType::StaticDelay_ms(20);
				WriteReg(0x0007,0x0061); 
				SystemDelayType::StaticDelay_ms(20);
				WriteReg(0x0007,0x0173); 
				SystemDelayType::StaticDelay_ms(20);
			} else if (DeviceID==0xB505) {
				WriteReg(0x0000,0x0000);
				WriteReg(0x0000,0x0000);
				WriteReg(0x0000,0x0000);
				WriteReg(0x0000,0x0000);

				WriteReg(0x00a4,0x0001);
				SystemDelayType::StaticDelay_ms(20);		  
				WriteReg(0x0060,0x2700);
				WriteReg(0x0008,0x0202);

				WriteReg(0x0030,0x0214);
				WriteReg(0x0031,0x3715);
				WriteReg(0x0032,0x0604);
				WriteReg(0x0033,0x0e16);
				WriteReg(0x0034,0x2211);
				WriteReg(0x0035,0x1500);
				WriteReg(0x0036,0x8507);
				WriteReg(0x0037,0x1407);
				WriteReg(0x0038,0x1403);
				WriteReg(0x0039,0x0020);

				WriteReg(0x0090,0x001a);
				WriteReg(0x0010,0x0000);
				WriteReg(0x0011,0x0007);
				WriteReg(0x0012,0x0000);
				WriteReg(0x0013,0x0000);
				SystemDelayType::StaticDelay_ms(20);

				WriteReg(0x0010,0x0730);
				WriteReg(0x0011,0x0137);
				SystemDelayType::StaticDelay_ms(20);

				WriteReg(0x0012,0x01b8);
				SystemDelayType::StaticDelay_ms(20);

				WriteReg(0x0013,0x0f00);
				WriteReg(0x002a,0x0080);
				WriteReg(0x0029,0x0048);
				SystemDelayType::StaticDelay_ms(20);

				WriteReg(0x0001,0x0100);
				WriteReg(0x0002,0x0700);
				WriteReg(0x0003,0x1230);
				WriteReg(0x0008,0x0202);
				WriteReg(0x000a,0x0000);
				WriteReg(0x000c,0x0000);
				WriteReg(0x000d,0x0000);
				WriteReg(0x000e,0x0030);
				WriteReg(0x0050,0x0000);
				WriteReg(0x0051,0x00ef);
				WriteReg(0x0052,0x0000);
				WriteReg(0x0053,0x013f);
				WriteReg(0x0060,0x2700);
				WriteReg(0x0061,0x0001);
				WriteReg(0x006a,0x0000);
				//WriteReg(0x0080,0x0000);
				//WriteReg(0x0081,0x0000);
				WriteReg(0x0090,0X0011);
				WriteReg(0x0092,0x0600);
				WriteReg(0x0093,0x0402);
				WriteReg(0x0094,0x0002);
				SystemDelayType::StaticDelay_ms(20);

				WriteReg(0x0007,0x0001);
				SystemDelayType::StaticDelay_ms(20);
				WriteReg(0x0007,0x0061);
				WriteReg(0x0007,0x0173);

				WriteReg(0x0020,0x0000);
				WriteReg(0x0021,0x0000);	  
				WriteReg(0x00,0x22);  
			} else if (DeviceID==0xC505) {
				WriteReg(0x0000,0x0000);
				WriteReg(0x0000,0x0000);
				SystemDelayType::StaticDelay_ms(20);		  
				WriteReg(0x0000,0x0000);
				WriteReg(0x0000,0x0000);
				WriteReg(0x0000,0x0000);
				WriteReg(0x0000,0x0000);
				WriteReg(0x00a4,0x0001);
				SystemDelayType::StaticDelay_ms(20);		  
				WriteReg(0x0060,0x2700);
				WriteReg(0x0008,0x0806);

				WriteReg(0x0030,0x0703);//gamma setting
				WriteReg(0x0031,0x0001);
				WriteReg(0x0032,0x0004);
				WriteReg(0x0033,0x0102);
				WriteReg(0x0034,0x0300);
				WriteReg(0x0035,0x0103);
				WriteReg(0x0036,0x001F);
				WriteReg(0x0037,0x0703);
				WriteReg(0x0038,0x0001);
				WriteReg(0x0039,0x0004);



				WriteReg(0x0090, 0x0015);	//80Hz
				WriteReg(0x0010, 0X0410);	//BT,AP
				WriteReg(0x0011,0x0247);	//DC1,DC0,VC
				WriteReg(0x0012, 0x01BC);
				WriteReg(0x0013, 0x0e00);
				SystemDelayType::StaticDelay_ms(120);
				WriteReg(0x0001, 0x0100);
				WriteReg(0x0002, 0x0200);
				WriteReg(0x0003, 0x1030);

				WriteReg(0x000A, 0x0008);
				WriteReg(0x000C, 0x0000);

				WriteReg(0x000E, 0x0020);
				WriteReg(0x000F, 0x0000);
				WriteReg(0x0020, 0x0000);	//H Start
				WriteReg(0x0021, 0x0000);	//V Start
				WriteReg(0x002A,0x003D);	//vcom2
				SystemDelayType::StaticDelay_ms(20);
				WriteReg(0x0029, 0x002d);
				WriteReg(0x0050, 0x0000);
				WriteReg(0x0051, 0xD0EF);
				WriteReg(0x0052, 0x0000);
				WriteReg(0x0053, 0x013F);
				WriteReg(0x0061, 0x0000);
				WriteReg(0x006A, 0x0000);
				WriteReg(0x0092,0x0300); 

				WriteReg(0x0093, 0x0005);
				WriteReg(0x0007, 0x0100);
			} else if (DeviceID==0x8989) {	   
				WriteReg(0x0000,0x0001);//打开晶振
				WriteReg(0x0003,0xA8A4);//0xA8A4
				WriteReg(0x000C,0x0000);    
				WriteReg(0x000D,0x080C);   
				WriteReg(0x000E,0x2B00);    
				WriteReg(0x001E,0x00B0);    
				WriteReg(0x0001,0x2B3F);//驱动输出控制320*240  0x6B3F
				WriteReg(0x0002,0x0600);
				WriteReg(0x0010,0x0000);  
				WriteReg(0x0011,0x6070); //定义数据格式  16位色 		横屏 0x6058
				WriteReg(0x0005,0x0000);  
				WriteReg(0x0006,0x0000);  
				WriteReg(0x0016,0xEF1C);  
				WriteReg(0x0017,0x0003);  
				WriteReg(0x0007,0x0233); //0x0233       
				WriteReg(0x000B,0x0000);  
				WriteReg(0x000F,0x0000); //扫描开始地址
				WriteReg(0x0041,0x0000);  
				WriteReg(0x0042,0x0000);  
				WriteReg(0x0048,0x0000);  
				WriteReg(0x0049,0x013F);  
				WriteReg(0x004A,0x0000);  
				WriteReg(0x004B,0x0000);  
				WriteReg(0x0044,0xEF00);  
				WriteReg(0x0045,0x0000);  
				WriteReg(0x0046,0x013F);  
				WriteReg(0x0030,0x0707);  
				WriteReg(0x0031,0x0204);  
				WriteReg(0x0032,0x0204);  
				WriteReg(0x0033,0x0502);  
				WriteReg(0x0034,0x0507);  
				WriteReg(0x0035,0x0204);  
				WriteReg(0x0036,0x0204);  
				WriteReg(0x0037,0x0502);  
				WriteReg(0x003A,0x0302);  
				WriteReg(0x003B,0x0302);  
				WriteReg(0x0023,0x0000);  
				WriteReg(0x0024,0x0000);  
				WriteReg(0x0025,0x8000);  
				WriteReg(0x004f,0);        //行首址0
				WriteReg(0x004e,0);        //列首址0
			} else if (DeviceID==0x4531) {
				WriteReg(0X00,0X0001);   
				SystemDelayType::StaticDelay_ms(10);   
				WriteReg(0X10,0X1628);   
				WriteReg(0X12,0X000e);//0x0006    
				WriteReg(0X13,0X0A39);   
				SystemDelayType::StaticDelay_ms(10);   
				WriteReg(0X11,0X0040);   
				WriteReg(0X15,0X0050);   
				SystemDelayType::StaticDelay_ms(10);   
				WriteReg(0X12,0X001e);//16    
				SystemDelayType::StaticDelay_ms(10);   
				WriteReg(0X10,0X1620);   
				WriteReg(0X13,0X2A39);   
				SystemDelayType::StaticDelay_ms(10);   
				WriteReg(0X01,0X0100);   
				WriteReg(0X02,0X0300);   
				WriteReg(0X03,0X1030);//改变方向的   
				WriteReg(0X08,0X0202);   
				WriteReg(0X0A,0X0008);   
				WriteReg(0X30,0X0000);   
				WriteReg(0X31,0X0402);   
				WriteReg(0X32,0X0106);   
				WriteReg(0X33,0X0503);   
				WriteReg(0X34,0X0104);   
				WriteReg(0X35,0X0301);   
				WriteReg(0X36,0X0707);   
				WriteReg(0X37,0X0305);   
				WriteReg(0X38,0X0208);   
				WriteReg(0X39,0X0F0B);   
				WriteReg(0X41,0X0002);   
				WriteReg(0X60,0X2700);   
				WriteReg(0X61,0X0001);   
				WriteReg(0X90,0X0210);   
				WriteReg(0X92,0X010A);   
				WriteReg(0X93,0X0004);   
				WriteReg(0XA0,0X0100);   
				WriteReg(0X07,0X0001);   
				WriteReg(0X07,0X0021);   
				WriteReg(0X07,0X0023);   
				WriteReg(0X07,0X0033);   
				WriteReg(0X07,0X0133);   
				WriteReg(0XA0,0X0000); 
			} else if (DeviceID==0x4535) {			      
				WriteReg(0X15,0X0030);   
				WriteReg(0X9A,0X0010);   
				WriteReg(0X11,0X0020);   
				WriteReg(0X10,0X3428);   
				WriteReg(0X12,0X0002);//16    
				WriteReg(0X13,0X1038);   
				SystemDelayType::StaticDelay_ms(40);   
				WriteReg(0X12,0X0012);//16    
				SystemDelayType::StaticDelay_ms(40);   
				WriteReg(0X10,0X3420);   
				WriteReg(0X13,0X3038);   
				SystemDelayType::StaticDelay_ms(70);   
				WriteReg(0X30,0X0000);   
				WriteReg(0X31,0X0402);   
				WriteReg(0X32,0X0307);   
				WriteReg(0X33,0X0304);   
				WriteReg(0X34,0X0004);   
				WriteReg(0X35,0X0401);   
				WriteReg(0X36,0X0707);   
				WriteReg(0X37,0X0305);   
				WriteReg(0X38,0X0610);   
				WriteReg(0X39,0X0610); 

				WriteReg(0X01,0X0100);   
				WriteReg(0X02,0X0300);   
				WriteReg(0X03,0X1030);//改变方向的   
				WriteReg(0X08,0X0808);   
				WriteReg(0X0A,0X0008);   
				WriteReg(0X60,0X2700);   
				WriteReg(0X61,0X0001);   
				WriteReg(0X90,0X013E);   
				WriteReg(0X92,0X0100);   
				WriteReg(0X93,0X0100);   
				WriteReg(0XA0,0X3000);   
				WriteReg(0XA3,0X0010);   
				WriteReg(0X07,0X0001);   
				WriteReg(0X07,0X0021);   
				WriteReg(0X07,0X0023);   
				WriteReg(0X07,0X0033);   
				WriteReg(0X07,0X0133);   
			}	
			SetScanDir(DFT_SCAN_DIR);

			// 清屏
			Clear();
		}


	};







}


#endif  /*_ADX_CM_ILIX_H_*/
