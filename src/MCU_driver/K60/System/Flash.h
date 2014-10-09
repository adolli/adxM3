
#ifndef _ADX_K60_FLASH_H_
#define _ADX_K60_FLASH_H_

#include "adxM3.h"

namespace K60
{

	class Flash
	{
		//FTFL 命令
#define FCMD (FTFL->FCCOB0)        

#define FADDR2 (FTFL->FCCOB1)         //Flash address [23:16]
#define FADDR1 (FTFL->FCCOB2)         //Flash address [15:8]
#define FADDR0 (FTFL->FCCOB3)         //Flash address [7:0]
#define FDATA0 (FTFL->FCCOB4)         //Data Byte 0       //注意一点，4字节排序，FDATA3是最低位，FDATA0是最高位
#define FDATA1 (FTFL->FCCOB5)         //Data Byte 1
#define FDATA2 (FTFL->FCCOB6)         //Data Byte 2
#define FDATA3 (FTFL->FCCOB7)         //Data Byte 3
#define FDATA4 (FTFL->FCCOB8)         //Data Byte 4
#define FDATA5 (FTFL->FCCOB9)         //Data Byte 5
#define FDATA6 (FTFL->FCCOBA)         //Data Byte 6
#define FDATA7 (FTFL->FCCOBB)         //Data Byte 7



		enum
		{
			RD1BLK		= 0x00,   // 读整块Flash
			RD1SEC		= 0x01,   // 读整个扇区
			PGMCHK		= 0x02,   // 写入检查
			RDRSRC		= 0x03,   // 读目标数据(4字节)
			PGM4		= 0x06,   // 写入长字(4字节)
			ERSBLK		= 0x08,   // 擦除整块Flash
			ERSSCR		= 0x09,   // 擦除Flash扇区
			PGMSEC		= 0x0B,   // 写入扇区
			RD1ALL		= 0x40,   // 读所有的块
			RDONCE		= 0x41,   // 只读一次
			PGMONCE		= 0x43,   // 只写一次
			ERSALL		= 0x44,   // 擦除所有块
			VFYKEY		= 0x45,   // 验证后门访问钥匙
			PGMPART		= 0x80,   // 写入分区
			SETRAM		= 0x81,   // 设定FlexRAM功能
		};




		friend class SystemType;
	public:

		enum
		{
			FLASH_SECTOR_SIZE = 2 * 1024,				// 扇区大小为 2kB
			FLASH_SECTOR_MAX = 256,						// 扇区数
			FLASH_ALIGN_ADDR = 4,                       // 地址对齐整数倍
		};     
		
		
		/*
		 * @type	Flash默认读写类型
		 */
		typedef UINT32 FlashIOType;





		union WordStruct
		{
			BYTE Byte[4];
			HWORD HalfWord[2];
			WORD Word;
		};




		/*
		 * @brief	Flash擦除扇区
		 * @param	sectorIndex 扇区号，（范围？？）
		 */
		static int EraseSector(UINT16 sectorIndex)
		{
			UINT32 addr = sectorIndex * FLASH_SECTOR_SIZE;

			// 设置擦除命令
			FTFL->FCCOB0 = ERSSCR;

			// 设置目标地址
			FADDR2 = ((WordStruct*)&addr)->Byte[2];
			FADDR1 = ((WordStruct*)&addr)->Byte[1];
			FADDR0 = ((WordStruct*)&addr)->Byte[0];

			if(PrivateExcute() == 0)
			{
				return 0;
			}

			if(sectorIndex == 0)
			{
				return Write(sectorIndex, 0x00040C, 0xFFFFFFFE);
			}

			return 1;
		}




		/*
		 * @brief	Flash写入
		 * @param	sectorIndex 扇区号
		 *			offset 该扇区地址偏移 必须是4的倍数，并且不得大于0x800
		 *			data 写入的数据
		 */
		static int Write(UINT16 sectorIndex, UINT16 offset, FlashIOType data)
		{
			if (offset % FLASH_ALIGN_ADDR != 0 || offset > FLASH_SECTOR_SIZE)
			{
				return 0;
			}

			UINT32 addr = sectorIndex * FLASH_SECTOR_SIZE + offset;

			// 设置目标地址
			FADDR2 = ((WordStruct*)&addr)->Byte[2];
			FADDR1 = ((WordStruct*)&addr)->Byte[1];
			FADDR0 = ((WordStruct*)&addr)->Byte[0];

			// 设置写入数据
			FDATA0 = ((WordStruct*)&data)->Byte[3];
			FDATA1 = ((WordStruct*)&data)->Byte[2];
			FDATA2 = ((WordStruct*)&data)->Byte[1];
			FDATA3 = ((WordStruct*)&data)->Byte[0]; 

			// 设置擦除命令
			FCMD = PGM4;

			if(PrivateExcute() == 0)
			{
				return 0;
			}

			return 1;  //成功执行
		}




		/*
		 * @brief	Flash读
		 * @param	sectorIndex 扇区号
		 *			offset 该扇区地址偏移 必须是4的倍数，并且不得大于0x800
		 */
		template< class ReturnType >
		static ReturnType Read(UINT16 sectorIndex, UINT16 offset)
		{
			return *(ReturnType*)(sectorIndex * FLASH_SECTOR_SIZE + offset);
		}




		/*
		 * @brief	Flash连续读写
		 * @param	sectorIndex 扇区号
		 *			offset 该扇区地址偏移 必须是4的倍数，并且不得大于0x800
		 *			buf 写入的数据缓冲区
		 *			bufsize 缓冲区大小，单位：字数
		 */
		static int SequentialWrite(UINT16 sectorIndex, UINT16 offset, const void* _buf, UINT32 bufsize)
		{
			const FlashIOType* buf = static_cast<const FlashIOType*>(_buf);
			while (bufsize--)
			{
				if (Write(sectorIndex, offset, *buf++) == 0)
				{
					return 0;
				}
				offset += 4;
			}
			return 1;
		}




		/*
		 * @brief	Flash连续读取
		 * @tparam	ReturnType 读取数据的数据类型
		 *				要求ReturnType为一个指向常量的指针类型
		 */
		template< class ReturnType >
		static ReturnType SequentialRead(UINT16 sectorIndex, UINT16 offset)
		{
			return (ReturnType)(sectorIndex * FLASH_SECTOR_SIZE + offset);
		}


	private:



		/*
		 * @brief	Flash初始化准备
		 */
		static void _Init()
		{
			// 清除Flash预读取缓冲区
			FMC->PFB0CR |= FMC_PFB0CR_S_B_INV_MASK;
			FMC->PFB1CR |= FMC_PFB0CR_S_B_INV_MASK;

			while(!(FTFL->FSTAT & FTFL_FSTAT_CCIF_MASK));    // 等待命令完成

			FTFL->FSTAT =    (0
				|  FTFL_FSTAT_CCIF_MASK        // 指令完成标志(写1清0)
				|  FTFL_FSTAT_RDCOLERR_MASK    // 读冲突错误标志(写1清0)
				|  FTFL_FSTAT_ACCERR_MASK      // 访问错误标志位(写1清0)
				|  FTFL_FSTAT_FPVIOL_MASK      // 非法访问保护标志位(写1清0)
				);
		}



		/*
		 * @brief	Flash执行操作
		 */
		static int PrivateExcute()
		{
			FTFL->FSTAT =    (0
				|  FTFL_FSTAT_CCIF_MASK        // 指令完成标志(写1清0)
				|  FTFL_FSTAT_RDCOLERR_MASK    // 读冲突错误标志(写1清0)
				|  FTFL_FSTAT_ACCERR_MASK      // 访问错误标志位(写1清0)
				|  FTFL_FSTAT_FPVIOL_MASK      // 非法访问保护标志位(写1清0)
				);

			while(!(FTFL->FSTAT & FTFL_FSTAT_CCIF_MASK));    // 等待命令完成

			// 检查错误标志
			if( FTFL->FSTAT 
				& (FTFL_FSTAT_ACCERR_MASK | FTFL_FSTAT_RDCOLERR_MASK | FTFL_FSTAT_FPVIOL_MASK | FTFL_FSTAT_MGSTAT0_MASK))
			{
				return 0 ;                                  //执行命令出错
			}
			else
			{
				return 1;                                   //执行命令成功
			}
		}

	};
}

#endif  /*_ADX_K60_FLASH_H_*/
