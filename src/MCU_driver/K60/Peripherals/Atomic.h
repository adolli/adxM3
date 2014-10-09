
#ifndef _ADX_K60_ATOMIC_H_
#define _ADX_K60_ATOMIC_H_

#include "adxM3.h"

namespace K60
{


	template< AddressType REG, unsigned int BIT_INDEX >
	struct BITBAND__
	{
		static void SET()
		{
			(*((BIT volatile*)(0x42000000u + (32u*((UINT32)(REG) - (UINT32)0x40000000u)) + (4u*((UINT32)(BIT_INDEX)))))) = 1;
		}
		static void RESET()
		{
			(*((BIT volatile*)(0x42000000u + (32u*((UINT32)(REG) - (UINT32)0x40000000u)) + (4u*((UINT32)(BIT_INDEX)))))) = 0;

		}
		static bool GET()
		{
			return bool(*((BIT volatile*)(0x42000000u + (32u*((UINT32)(REG) - (UINT32)0x40000000u)) + (4u*((UINT32)(BIT_INDEX))))));
		}
	};



	// ����Ĵ���λ������
	// ��ע�⣬����������Ĵ��������������ڴ�
	#define BITBAND(Reg,Bit) (*((UINT32 volatile*)(0x42000000u + (32u*((UINT32)(Reg) - (UINT32)0x40000000u)) + (4u*((UINT32)(Bit))))))
	

	/*
	 * @brief	����Ĵ���λ����
	 * @tparam	REG ��arm32λ�ܹ���ΪUINT32���͵ļĴ�����ַ
	 *			BIT_INDEX �üĴ�����λ������
	 */
	template< AddressType REG, unsigned int BIT_INDEX > 
	class RegBitType
	{
	public:
		static const UINT32 RegAddress = REG;
		static const unsigned int BitIndex = BIT_INDEX;
		

		/*
		 * @brief	�Ĵ���λ�Ļ�����������λ����λ����ת
		 */
		static void SET()
		{
			BITBAND(REG, BIT_INDEX) = 1;
		}
		static void CLR()
		{
			BITBAND(REG, BIT_INDEX) = 0;
		}
		static void TGL()
		{
			BITBAND(REG, BIT_INDEX) = !BITBAND(REG, BIT_INDEX);
		}
	};




	/*
	 * @brief	���ڼĴ���λ���ſ����ͣ�����ʹ��ĳ�����ſ��źţ�
	 * @tparam	REG ��arm32λ�ܹ���ΪUINT32���͵ļĴ�����ַ
	 *			BIT_INDEX �üĴ�����λ������
	 */
	template< UINT32 REG, unsigned int BIT_INDEX > 
	class GateType
		: public RegBitType<REG, BIT_INDEX>
	{
	public:


		/*
		 * @brief	�����ſ��źſ���
		 */
		static void ON()
		{
			RegBitType<REG, BIT_INDEX>::SET();
		}
		static void OFF()
		{
			RegBitType<REG, BIT_INDEX>::CLR();
		}
	};



	/*
	 * @brief	���ſ����ͣ������������趯�����������κ�Ч����
	 *				������ҪON��OFF��������ͨ��
	 * @tparam	REG ��arm32λ�ܹ���ΪUINT32���͵ļĴ�����ַ
	 *			BIT_INDEX �üĴ�����λ������
	 */
	template< UINT32 REG = 0, unsigned int BIT_INDEX = 0 > 
	class EmptyGateType
	{
	public:


		/*
		 * @brief	�����ſ��źſ���
		 *			�����κ�����
		 */
		static void ON()
		{
		}
		static void OFF()
		{
		}
	};

}

#endif /*_ADX_K60_ATOMIC_H_*/
