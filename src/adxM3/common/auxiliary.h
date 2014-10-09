// auxiliary.h

#pragma once

#ifndef _AUXILIARY_H_
#define _AUXILIARY_H_

#include "defs.h"
#include <cstdlib>


/*
 * @brief	������Χ���ƣ�˫�෶Χ��
 * @param	var Ҫ��������ı���
 *			lb  �½�
 *			ub  �Ͻ�
 */
#define RangeLimit(lb,op1,var,op2,ub) ((var) = (var) op1 (lb) ? (lb) : (ub) op2 (var) ? (ub) : (var));



namespace adxM3
{

	class __Convertor
	{
	public:
		// �������ַ���ת������
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
	 * ��ѧ���࣬���ɻ�������ѧ����
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
	 * max/minģ�溯��
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
	 * absģ�溯��
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
	 * @brief	�����ַ���������ͬstdlib����Ĺ���
	 * @param	dest Ŀ���ַ���
	 *			source Դ�ַ���
	 * @notice	Ŀ���ַ����ռ����û����룬һֱ������source��Ϊֹ
	 * @retval	����dest�ַ���
	 */
	inline char *	
	BasicString::strcpy(char * dest, const char * source)
	{
		char* ret = dest;
		while ((*dest++ = *source++) != '\0');
		return ret;
	}



	/*
	 * @brief	���ַ������ȣ����������һ��\0
	 * @param	str Դ�ַ���
	 * @retval	�����ַ����ĳ���ֵ
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
	 * @brief	Դ�ַ������ӵ�Ŀ���ַ���ĩβ
	 * @param	dest Ŀ���ַ���
	 *			src Դ�ַ���
	 * @notice	�뱣֤Ŀ���ַ����������㹻������Խ��
	 * @retval	����Ŀ���ַ����׵�ַ
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
