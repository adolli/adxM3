// auxiliary.h

#pragma once

#ifndef _AUXILIARY_H_
#define _AUXILIARY_H_

#include "defs.h"
#include <cstdlib>


/*
 * @brief	变量范围控制（双侧范围）
 * @param	var 要限制区间的变量
 *			lb  下界
 *			ub  上界
 */
#define RangeLimit(lb,op1,var,op2,ub) ((var) = (var) op1 (lb) ? (lb) : (ub) op2 (var) ? (ub) : (var));



namespace adxM3
{

	class __Convertor
	{
	public:
		// 整数与字符串转换函数
		static	char *	itoa(INT  val, char* string, UINT8 radix = 10);
		static	char *	itoa(UINT val, char* string, UINT8 radix = 10);
		static	int		atoi(const char* str, UINT8 radix = 10);
		static	float	atof(const char* str, UINT32 radix = 10);
		static	char *	ftoa(float val, char* string, UINT32 radix = 10);
		static	double	atod(const char* str, UINT32 radix = 10);
		static	char *	dtoa(double val, char* string, UINT32 radix = 10);
						__Convertor();

	};

	extern const __Convertor Convertor;



	class BasicString
	{
	public:
		static	char *	strcpy(char * dest, const char * source);
		static	UINT	strlen(const char * str);
		static	char *	strcat(char *dest, const char *src);
	};


	/*
	 * 数学家类，集成基本的数学运算
	 */
	class __Mathematician
	{
	public:
		static	UINT	GenerateRandomNum(UINT minNum=0UL,UINT maxNum=0xffffffffUL);
						__Mathematician();

		static	UINT	seeds;
	};

	extern const __Mathematician Mathematician;



	/*
	 * max/min模版函数
	 */
	template <typename T>
	inline T max(T a,T b)
	{
		return a > b ? a : b;
	}
	template <typename T>
	inline T min(T a,T b)
	{
		return a < b ? a : b;
	}

	/*
	 * abs模版函数
	 */
	template <typename T>
	inline T abs(T a)
	{
		return a < 0 ? -a : a;
	}




	template< typename T >
	inline int sgn(T a)
	{
		if (a < 0)
		{
			return -1;
		}
		else if (a > 0)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}



	/*
	 * @brief	基础字符串拷贝，同stdlib里面的功能
	 * @param	dest 目标字符串
	 *			source 源字符串
	 * @notice	目标字符串空间由用户申请，一直拷贝到source完为止
	 * @retval	返回dest字符串
	 */
	inline char *	
	BasicString::strcpy(char * dest, const char * source)
	{
		char* ret = dest;
		while ((*dest++ = *source++) != '\0');
		return ret;
	}



	/*
	 * @brief	求字符串长度，不包括最后一个\0
	 * @param	str 源字符串
	 * @retval	返回字符串的长度值
	 */
	inline UINT		
	BasicString::strlen(const char * str)
	{
		unsigned int len = 0;
		while (*str++)
		{
			++len;
		}
		return len;
	}



	/*
	 * @brief	源字符串附加到目标字符串末尾
	 * @param	dest 目标字符串
	 *			src 源字符串
	 * @notice	请保证目标字符串缓冲区足够，以免越界
	 * @retval	返回目标字符串首地址
	 */
	inline char*
	BasicString::strcat(char *dest, const char *src)
	{
		char* ret = dest;
		while (*dest) ++dest;
		while (*src) *dest++ = *src++;
		*dest = '\0';
		return ret;
	}



}




#endif	/*_AUXILIARY_H_*/
