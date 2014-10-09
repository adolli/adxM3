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

	// ��radixȡ�෨
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

	// ����
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
	
	// ɨ���һ��+/-��
	while(str) {
		if (*str == '-') {
			negtive = true;
			break;
		} else if (*str == '+' || *str>='0' && *str<='9' || *str>='A' && *str<='Z' || *str>='a' && *str<='z') {
			break;
		}
		++str;
	}

	// ɨ���һ������
	while (str && !(*str>='0' && *str<='9' || *str>='A' && *str<='Z' || *str>='a' && *str<='z')) {
		++str;
	}
	if (!str) return result;

	// �������
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
	// strԤ������ĩβǿ�Ƽ���.0����������С��������Ӱ�죬
	// ����ûС�����ּ���.0���������ֵ����
	// ����ע�⣡����˵���atofʱ���뱣֤strĩβ�ж����2�ֽڻ�����
	BasicString::strcat(const_cast<char*>(str),".0");

	const UINT32 real_radix = radix;
	float result = 0;
	bool negtive = false;

	// ɨ���һ��+/-�Ż��һ��С����
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

	// ɨ�赽����Ϊֹ
	while (str && !(*str >= '0' && *str <= '9' || *str>='A' && *str<='Z' || *str>='a' && *str<='z')) {
		++str;
	}
	if (!str) return result;

	// �����������
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

	// ɨ��С����
	while (str && *str != '.') {
		++str;
	} 
	if (!str) return result;

START_FRACTIONAL:

	// ɨ�赽����Ϊֹ
	while (str && !(*str >= '0' && *str <= '9' || *str>='A' && *str<='Z' || *str>='a' && *str<='z')) {
		++str;
	} 
	if (!str) return result;

	// С���������
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
	// ʹ��std�����
	sprintf(string, "%f", val);
	return string;


	//// ����������С�����ַ��룬�������������棬С�����ֶ���
	//
	//// ��������
	//int integer = int(val);
	//char* ip = itoa(integer, string, radix);

	//// �ó�С�����ֵĸ����������ж��Ƿ���С������
	//float f_frac = val - integer;
	//if (f_frac < 0.000001)
	//{
	//	return string;
	//}

	//// ����С����
	//while (*ip) ++ip;
	//*ip++ = '.';
	//
	//// ����С������0
	//while (f_frac < 1 / (float)radix)
	//{
	//	*ip++ = '0';
	//	f_frac *= radix;
	//}

	//// С�����֣���������Ϊ30��
	//char buf[30];
	//int fractional = int(1000000 * f_frac);
	//char* fp = itoa(fractional, buf, radix);
	//
	//// ����������������С������
	//BasicString::strcat(ip, fp);
	//
	//return string;
}

double	
__Convertor::atod(const char* str, UINT32 radix)
{
	// ����ͬatof��������float�ĳ�double

	// strԤ������ĩβǿ�Ƽ���.0����������С��������Ӱ�죬
	// ����ûС�����ּ���.0���������ֵ����
	// ����ע�⣡����˵���atofʱ���뱣֤strĩβ�ж����2�ֽڻ�����
	BasicString::strcat(const_cast<char*>(str),".0");

	const UINT32 real_radix = radix;
	double result = 0;
	bool negtive = false;

	// ɨ���һ��+/-�Ż��һ��С����
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

	// ɨ�赽����Ϊֹ
	while (str && !(*str >= '0' && *str <= '9' || *str>='A' && *str<='Z' || *str>='a' && *str<='z')) {
		++str;
	}
	if (!str) return result;

	// �����������
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

	// ɨ��С����
	while (str && *str != '.') {
		++str;
	} 
	if (!str) return result;

START_FRACTIONAL:

	// ɨ�赽����Ϊֹ
	while (str && !(*str >= '0' && *str <= '9' || *str>='A' && *str<='Z' || *str>='a' && *str<='z')) {
		++str;
	} 
	if (!str) return result;

	// С���������
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
	// ʹ��std�����
	sprintf(string, "%f", val);
	return string;

	//// ����ͬftoa�����޸�С����Χ
	//// ����������С�����ַ��룬�������������棬С�����ֶ���
	//
	//// ��������
	//int integer = int(val);
	//char* ip = itoa(integer, string, radix);

	//// �ó�С�����ֵĸ����������ж��Ƿ���С������
	//double f_frac = val - integer;
	//if (f_frac < 0.000001)
	//{
	//	return string;
	//}

	//// ����С����
	//while (*ip) ++ip;
	//*ip++ = '.';
	//
	//// ����С������0
	//while (f_frac < 1 / (double)radix)
	//{
	//	*ip++ = '0';
	//	f_frac *= radix;
	//}

	//// С�����֣���������Ϊ30��
	//char buf[60];
	//int fractional = int(1000000000.0F * f_frac);
	//char* fp = itoa(fractional, buf, radix);
	//
	//// ����������������С������
	//BasicString::strcat(ip, fp);
	//
	//return string;
}



UINT __Mathematician::seeds = 1054026448;

__Mathematician::__Mathematician()
{}



/*
 * @brief	α���������
 * @param	minNum ��С����
			maxNum ���������������
 * @notice	���������Сʱ������Ҳ�϶̣��������һ����΢����������������
 */
UINT		
__Mathematician::GenerateRandomNum(UINT minNum,UINT maxNum)
{
	seeds = seeds * 29 + 37;
	return (seeds % (maxNum-minNum)) + minNum;
}



