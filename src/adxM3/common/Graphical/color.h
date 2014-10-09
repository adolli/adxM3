/*
 * color.h
 *
 *  Created on: 2013-9-11
 *      Author: Administrator
 */

#ifndef _COLOR_H_
#define _COLOR_H_

#include "../defs.h"



namespace adxM3
{


	typedef UINT16 RGB565Type;


	union RGB565
	{
		UINT16 color;
		struct 
		{					
			unsigned Blue	: 5;			
			unsigned Green	: 6;
			unsigned Red	: 5;
		} PrimaryColors;
	};



	/* 
	 * 颜色值枚举
	 */
	enum
	{
		clWHITE         	= 0xFFFF,
		clBLACK         	= 0x0000,
		clBLUE         		= 0x001F,
		clBRED				= 0XF81F,
		clGRED 				= 0XFFE0,
		clGBLUE				= 0X07FF,
		clRED           	= 0xF800,
		clMAGENTA       	= 0xF81F,
		clGREEN         	= 0x07E0,
		clCYAN          	= 0x7FFF,
		clYELLOW        	= 0xFFE0,
		clBROWN 			= 0XBC40,						//棕色
		clBRRED 			= 0XFC07,						//棕红色
		clGRAY  			= 0X8430,						//灰色
		clDARKBLUE      	= 0X01CF,						//深蓝色
		clLIGHTBLUE      	= 0X7D7C,						//浅蓝色
		clGRAYBLUE       	= 0X5458,						//灰蓝色
		clLIGHTGREEN     	= 0X841F,						//浅绿色
		clLIGHTGRAY			= 0XEF5B,						//浅灰色(PANNEL)
		clLGRAY 			= 0XC618,						//浅灰色(PANNEL),窗体背景色
		clLGRAYBLUE			= 0XA651,						//浅灰蓝色(中间层颜色)
		clLBBLUE			= 0X2B12						//浅棕蓝色(选择条目的反色)
	};

}



#endif /* _COLOR_H_ */
