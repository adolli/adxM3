
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



	// 外设寄存器位带操作
	// 请注意，仅限于外设寄存器！不能用于内存
	#define BITBAND(Reg,Bit) (*((UINT32 volatile*)(0x42000000u + (32u*((UINT32)(Reg) - (UINT32)0x40000000u)) + (4u*((UINT32)(Bit))))))
	

	/*
	 * @brief	外设寄存器位类型
	 * @tparam	REG 在arm32位架构中为UINT32类型的寄存器地址
	 *			BIT_INDEX 该寄存器的位索引号
	 */
	template< AddressType REG, unsigned int BIT_INDEX > 
	class RegBitType
	{
	public:
		static const UINT32 RegAddress = REG;
		static const unsigned int BitIndex = BIT_INDEX;
		

		/*
		 * @brief	寄存器位的基本操作，置位、复位、翻转
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
	 * @brief	基于寄存器位的门控类型（用于使能某外设门控信号）
	 * @tparam	REG 在arm32位架构中为UINT32类型的寄存器地址
	 *			BIT_INDEX 该寄存器的位索引号
	 */
	template< UINT32 REG, unsigned int BIT_INDEX > 
	class GateType
		: public RegBitType<REG, BIT_INDEX>
	{
	public:


		/*
		 * @brief	外设门控信号开关
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
	 * @brief	空门控类型（用于满足外设动作但不产生任何效果）
	 *				编译需要ON和OFF方法才能通过
	 * @tparam	REG 在arm32位架构中为UINT32类型的寄存器地址
	 *			BIT_INDEX 该寄存器的位索引号
	 */
	template< UINT32 REG = 0, unsigned int BIT_INDEX = 0 > 
	class EmptyGateType
	{
	public:


		/*
		 * @brief	外设门控信号开关
		 *			不做任何事情
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
