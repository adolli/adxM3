// auxiliary.cpp

#include "auxiliary.h"
#include <cstdio>
#include <cstdlib>
using namespace std;
using namespace adxM3;
 
const __Convertor Convertor;
const __Mathematician Mathematician;

__Convertor::__Convertor(){}

char * 
__Convertor::itoa(UINT val, char* string, UINT8 radix)
{
	char   *p;             
	char   *firstDigit;      
	char   temp = 0;           
	UINT8  digval = 0;     

	firstDigit = p = string;

	// 除radix取余法
	do
	{
		digval = (UINT8)(val % radix);
		val /= radix;


		if  (digval > 9) {
			*p++ = (char)(digval - 10 + 'A'); 
		} else {
			*p++ = (char)(digval + '0');  
		}
	} while (val > 0);

	*p-- = '\0';         

	// 逆序
	do
	{
		temp = *p;
		*p = *firstDigit;
		*firstDigit = temp;
		--p;
		++firstDigit;        
	} while (firstDigit < p);  

	return string;
}

char * 
__Convertor::itoa(int val, char* string, UINT8 radix)
{
	char* p = string;              

	if (val < 0) {
		*p++ = '-';
		val = (unsigned long)(-(long)val);
	}

	itoa((UINT)val,p,radix);
	return string;
}

int	  
__Convertor::atoi(const char* str, UINT8 radix)
{
	int result = 0;
	bool negtive = false;
	
	// 扫描第一个+/-号
	while(str) {
		if (*str == '-') {
			negtive = true;
			break;
		} else if (*str == '+' || *str>='0' && *str<='9' || *str>='A' && *str<='Z' || *str>='a' && *str<='z') {
			break;
		}
		++str;
	}

	// 扫描第一个数字
	while (str && !(*str>='0' && *str<='9' || *str>='A' && *str<='Z' || *str>='a' && *str<='z')) {
		++str;
	}
	if (!str) return result;

	// 读数求和
	while(str) 
	{
		char digitValue = *str;
		if (digitValue >= '0' && digitValue <= '9')
		{
			digitValue -= '0';
		}
		else if (digitValue >= 'A' && digitValue <= 'Z')
		{
			digitValue -= 'A';
			digitValue += 10;
		}
		else if (digitValue >= 'a' && digitValue <= 'z')
		{
			digitValue -= 'a';
			digitValue += 10;
		}
		else
		{
			break;
		}
		result = result * radix + digitValue;
		++str;
	}
	return negtive ? -result : result;
}

float	
__Convertor::atof(const char* str, UINT32 radix)
{
	// str预处理，在末尾强制加上.0，若本身有小数部分则不影响，
	// 本身没小数部分加上.0后可修正奇怪的误差
	// 【请注意！】因此调用atof时，请保证str末尾有额外的2字节缓冲区
	BasicString::strcat(const_cast<char*>(str),".0");

	const UINT32 real_radix = radix;
	float result = 0;
	bool negtive = false;

	// 扫描第一个+/-号或第一个小数点
	while (str) 
	{
		if (*str == '-') 
		{
			negtive = true;
			break;
		} 
		else if (*str == '+' || *str>='0' && *str<='9' || *str>='A' && *str<='Z' || *str>='a' && *str<='z') 
		{
			break;
		}
		else if (*str == '.')
		{
			goto START_FRACTIONAL;
		}
		++str;
	}

	// 扫描到数字为止
	while (str && !(*str >= '0' && *str <= '9' || *str>='A' && *str<='Z' || *str>='a' && *str<='z')) {
		++str;
	}
	if (!str) return result;

	// 整数部分求和
	while (str)
	{
		char digitValue = *str;
		if (digitValue >= '0' && digitValue <= '9')
		{
			digitValue -= '0';
		}
		else if (digitValue >= 'A' && digitValue <= 'Z')
		{
			digitValue -= 'A';
			digitValue += 10;
		}
		else if (digitValue >= 'a' && digitValue <= 'z')
		{
			digitValue -= 'a';
			digitValue += 10;
		}
		else
		{
			break;
		}
		result = result * real_radix + digitValue;
		++str;
	} 
	if (!str) return result;

	// 扫描小数点
	while (str && *str != '.') {
		++str;
	} 
	if (!str) return result;

START_FRACTIONAL:

	// 扫描到数字为止
	while (str && !(*str >= '0' && *str <= '9' || *str>='A' && *str<='Z' || *str>='a' && *str<='z')) {
		++str;
	} 
	if (!str) return result;

	// 小数部分求和
	while (str)
	{
		char digitValue = *str;
		if (digitValue >= '0' && digitValue <= '9')
		{
			digitValue -= '0';
		}
		else if (digitValue >= 'A' && digitValue <= 'Z')
		{
			digitValue -= 'A';
			digitValue += 10;
		}
		else if (digitValue >= 'a' && digitValue <= 'z')
		{
			digitValue -= 'a';
			digitValue += 10;
		}
		else
		{
			break;
		}
		result = result + (float)digitValue / (float)radix;
		radix *= real_radix;
		++str;
	} 

	return negtive ? -1 * result : result;
}

char *	
__Convertor::ftoa(float val, char* string, UINT32 radix)
{
	// 使用std库代码
	sprintf(string, "%f", val);
	return string;


	//// 整数部分与小数部分分离，并按照整数保存，小数部分定长
	//
	//// 整数部分
	//int integer = int(val);
	//char* ip = itoa(integer, string, radix);

	//// 得出小数部分的浮点数，并判断是否有小数部分
	//float f_frac = val - integer;
	//if (f_frac < 0.000001)
	//{
	//	return string;
	//}

	//// 加上小数点
	//while (*ip) ++ip;
	//*ip++ = '.';
	//
	//// 加上小数点后的0
	//while (f_frac < 1 / (float)radix)
	//{
	//	*ip++ = '0';
	//	f_frac *= radix;
	//}

	//// 小数部分（缓冲区设为30）
	//char buf[30];
	//int fractional = int(1000000 * f_frac);
	//char* fp = itoa(fractional, buf, radix);
	//
	//// 把整数部分连接上小数部分
	//BasicString::strcat(ip, fp);
	//
	//return string;
}

double	
__Convertor::atod(const char* str, UINT32 radix)
{
	// 代码同atof，仅仅把float改成double

	// str预处理，在末尾强制加上.0，若本身有小数部分则不影响，
	// 本身没小数部分加上.0后可修正奇怪的误差
	// 【请注意！】因此调用atof时，请保证str末尾有额外的2字节缓冲区
	BasicString::strcat(const_cast<char*>(str),".0");

	const UINT32 real_radix = radix;
	double result = 0;
	bool negtive = false;

	// 扫描第一个+/-号或第一个小数点
	while (str) 
	{
		if (*str == '-') 
		{
			negtive = true;
			break;
		} 
		else if (*str == '+' || *str>='0' && *str<='9' || *str>='A' && *str<='Z' || *str>='a' && *str<='z') 
		{
			break;
		}
		else if (*str == '.')
		{
			goto START_FRACTIONAL;
		}
		++str;
	}

	// 扫描到数字为止
	while (str && !(*str >= '0' && *str <= '9' || *str>='A' && *str<='Z' || *str>='a' && *str<='z')) {
		++str;
	}
	if (!str) return result;

	// 整数部分求和
	while (str)
	{
		char digitValue = *str;
		if (digitValue >= '0' && digitValue <= '9')
		{
			digitValue -= '0';
		}
		else if (digitValue >= 'A' && digitValue <= 'Z')
		{
			digitValue -= 'A';
			digitValue += 10;
		}
		else if (digitValue >= 'a' && digitValue <= 'z')
		{
			digitValue -= 'a';
			digitValue += 10;
		}
		else
		{
			break;
		}
		result = result * real_radix + digitValue;
		++str;
	} 
	if (!str) return result;

	// 扫描小数点
	while (str && *str != '.') {
		++str;
	} 
	if (!str) return result;

START_FRACTIONAL:

	// 扫描到数字为止
	while (str && !(*str >= '0' && *str <= '9' || *str>='A' && *str<='Z' || *str>='a' && *str<='z')) {
		++str;
	} 
	if (!str) return result;

	// 小数部分求和
	while (str)
	{
		char digitValue = *str;
		if (digitValue >= '0' && digitValue <= '9')
		{
			digitValue -= '0';
		}
		else if (digitValue >= 'A' && digitValue <= 'Z')
		{
			digitValue -= 'A';
			digitValue += 10;
		}
		else if (digitValue >= 'a' && digitValue <= 'z')
		{
			digitValue -= 'a';
			digitValue += 10;
		}
		else
		{
			break;
		}
		result = result + (double)digitValue / (double)radix;
		radix *= real_radix;
		++str;
	} 

	return negtive ? -1 * result : result;
}

char *	
__Convertor::dtoa(double val, char* string, UINT32 radix)
{
	// 使用std库代码
	sprintf(string, "%f", val);
	return string;

	//// 代码同ftoa，仅修改小数范围
	//// 整数部分与小数部分分离，并按照整数保存，小数部分定长
	//
	//// 整数部分
	//int integer = int(val);
	//char* ip = itoa(integer, string, radix);

	//// 得出小数部分的浮点数，并判断是否有小数部分
	//double f_frac = val - integer;
	//if (f_frac < 0.000001)
	//{
	//	return string;
	//}

	//// 加上小数点
	//while (*ip) ++ip;
	//*ip++ = '.';
	//
	//// 加上小数点后的0
	//while (f_frac < 1 / (double)radix)
	//{
	//	*ip++ = '0';
	//	f_frac *= radix;
	//}

	//// 小数部分（缓冲区设为30）
	//char buf[60];
	//int fractional = int(1000000000.0F * f_frac);
	//char* fp = itoa(fractional, buf, radix);
	//
	//// 把整数部分连接上小数部分
	//BasicString::strcat(ip, fp);
	//
	//return string;
}



UINT __Mathematician::seeds = 1054026448;

__Mathematician::__Mathematician()
{}



/*
 * @brief	伪随机数产生
 * @param	minNum 最小整数
			maxNum 最大整数（不含）
 * @notice	当随机数较小时，周期也较短，建议产生一个稍微大点的数，再做处理
 */
UINT		
__Mathematician::GenerateRandomNum(UINT minNum,UINT maxNum)
{
	seeds = seeds * 29 + 37;
	return (seeds % (maxNum-minNum)) + minNum;
}



