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
	 * ��ɫֵö��
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
		clBROWN 			= 0XBC40,						//��ɫ
		clBRRED 			= 0XFC07,						//�غ�ɫ
		clGRAY  			= 0X8430,						//��ɫ
		clDARKBLUE      	= 0X01CF,						//����ɫ
		clLIGHTBLUE      	= 0X7D7C,						//ǳ��ɫ
		clGRAYBLUE       	= 0X5458,						//����ɫ
		clLIGHTGREEN     	= 0X841F,						//ǳ��ɫ
		clLIGHTGRAY			= 0XEF5B,						//ǳ��ɫ(PANNEL)
		clLGRAY 			= 0XC618,						//ǳ��ɫ(PANNEL),���屳��ɫ
		clLGRAYBLUE			= 0XA651,						//ǳ����ɫ(�м����ɫ)
		clLBBLUE			= 0X2B12						//ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)
	};

}



#endif /* _COLOR_H_ */
