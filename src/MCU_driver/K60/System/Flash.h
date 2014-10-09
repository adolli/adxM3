
#ifndef _ADX_K60_FLASH_H_
#define _ADX_K60_FLASH_H_

#include "adxM3.h"

namespace K60
{

	class Flash
	{
		//FTFL ����
#define FCMD (FTFL->FCCOB0)        

#define FADDR2 (FTFL->FCCOB1)         //Flash address [23:16]
#define FADDR1 (FTFL->FCCOB2)         //Flash address [15:8]
#define FADDR0 (FTFL->FCCOB3)         //Flash address [7:0]
#define FDATA0 (FTFL->FCCOB4)         //Data Byte 0       //ע��һ�㣬4�ֽ�����FDATA3�����λ��FDATA0�����λ
#define FDATA1 (FTFL->FCCOB5)         //Data Byte 1
#define FDATA2 (FTFL->FCCOB6)         //Data Byte 2
#define FDATA3 (FTFL->FCCOB7)         //Data Byte 3
#define FDATA4 (FTFL->FCCOB8)         //Data Byte 4
#define FDATA5 (FTFL->FCCOB9)         //Data Byte 5
#define FDATA6 (FTFL->FCCOBA)         //Data Byte 6
#define FDATA7 (FTFL->FCCOBB)         //Data Byte 7



		enum
		{
			RD1BLK		= 0x00,   // ������Flash
			RD1SEC		= 0x01,   // ����������
			PGMCHK		= 0x02,   // д����
			RDRSRC		= 0x03,   // ��Ŀ������(4�ֽ�)
			PGM4		= 0x06,   // д�볤��(4�ֽ�)
			ERSBLK		= 0x08,   // ��������Flash
			ERSSCR		= 0x09,   // ����Flash����
			PGMSEC		= 0x0B,   // д������
			RD1ALL		= 0x40,   // �����еĿ�
			RDONCE		= 0x41,   // ֻ��һ��
			PGMONCE		= 0x43,   // ֻдһ��
			ERSALL		= 0x44,   // �������п�
			VFYKEY		= 0x45,   // ��֤���ŷ���Կ��
			PGMPART		= 0x80,   // д�����
			SETRAM		= 0x81,   // �趨FlexRAM����
		};




		friend class SystemType;
	public:

		enum
		{
			FLASH_SECTOR_SIZE = 2 * 1024,				// ������СΪ 2kB
			FLASH_SECTOR_MAX = 256,						// ������
			FLASH_ALIGN_ADDR = 4,                       // ��ַ����������
		};     
		
		
		/*
		 * @type	FlashĬ�϶�д����
		 */
		typedef UINT32 FlashIOType;





		union WordStruct
		{
			BYTE Byte[4];
			HWORD HalfWord[2];
			WORD Word;
		};




		/*
		 * @brief	Flash��������
		 * @param	sectorIndex �����ţ�����Χ������
		 */
		static int EraseSector(UINT16 sectorIndex)
		{
			UINT32 addr = sectorIndex * FLASH_SECTOR_SIZE;

			// ���ò�������
			FTFL->FCCOB0 = ERSSCR;

			// ����Ŀ���ַ
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
		 * @brief	Flashд��
		 * @param	sectorIndex ������
		 *			offset ��������ַƫ�� ������4�ı��������Ҳ��ô���0x800
		 *			data д�������
		 */
		static int Write(UINT16 sectorIndex, UINT16 offset, FlashIOType data)
		{
			if (offset % FLASH_ALIGN_ADDR != 0 || offset > FLASH_SECTOR_SIZE)
			{
				return 0;
			}

			UINT32 addr = sectorIndex * FLASH_SECTOR_SIZE + offset;

			// ����Ŀ���ַ
			FADDR2 = ((WordStruct*)&addr)->Byte[2];
			FADDR1 = ((WordStruct*)&addr)->Byte[1];
			FADDR0 = ((WordStruct*)&addr)->Byte[0];

			// ����д������
			FDATA0 = ((WordStruct*)&data)->Byte[3];
			FDATA1 = ((WordStruct*)&data)->Byte[2];
			FDATA2 = ((WordStruct*)&data)->Byte[1];
			FDATA3 = ((WordStruct*)&data)->Byte[0]; 

			// ���ò�������
			FCMD = PGM4;

			if(PrivateExcute() == 0)
			{
				return 0;
			}

			return 1;  //�ɹ�ִ��
		}




		/*
		 * @brief	Flash��
		 * @param	sectorIndex ������
		 *			offset ��������ַƫ�� ������4�ı��������Ҳ��ô���0x800
		 */
		template< class ReturnType >
		static ReturnType Read(UINT16 sectorIndex, UINT16 offset)
		{
			return *(ReturnType*)(sectorIndex * FLASH_SECTOR_SIZE + offset);
		}




		/*
		 * @brief	Flash������д
		 * @param	sectorIndex ������
		 *			offset ��������ַƫ�� ������4�ı��������Ҳ��ô���0x800
		 *			buf д������ݻ�����
		 *			bufsize ��������С����λ������
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
		 * @brief	Flash������ȡ
		 * @tparam	ReturnType ��ȡ���ݵ���������
		 *				Ҫ��ReturnTypeΪһ��ָ������ָ������
		 */
		template< class ReturnType >
		static ReturnType SequentialRead(UINT16 sectorIndex, UINT16 offset)
		{
			return (ReturnType)(sectorIndex * FLASH_SECTOR_SIZE + offset);
		}


	private:



		/*
		 * @brief	Flash��ʼ��׼��
		 */
		static void _Init()
		{
			// ���FlashԤ��ȡ������
			FMC->PFB0CR |= FMC_PFB0CR_S_B_INV_MASK;
			FMC->PFB1CR |= FMC_PFB0CR_S_B_INV_MASK;

			while(!(FTFL->FSTAT & FTFL_FSTAT_CCIF_MASK));    // �ȴ��������

			FTFL->FSTAT =    (0
				|  FTFL_FSTAT_CCIF_MASK        // ָ����ɱ�־(д1��0)
				|  FTFL_FSTAT_RDCOLERR_MASK    // ����ͻ�����־(д1��0)
				|  FTFL_FSTAT_ACCERR_MASK      // ���ʴ����־λ(д1��0)
				|  FTFL_FSTAT_FPVIOL_MASK      // �Ƿ����ʱ�����־λ(д1��0)
				);
		}



		/*
		 * @brief	Flashִ�в���
		 */
		static int PrivateExcute()
		{
			FTFL->FSTAT =    (0
				|  FTFL_FSTAT_CCIF_MASK        // ָ����ɱ�־(д1��0)
				|  FTFL_FSTAT_RDCOLERR_MASK    // ����ͻ�����־(д1��0)
				|  FTFL_FSTAT_ACCERR_MASK      // ���ʴ����־λ(д1��0)
				|  FTFL_FSTAT_FPVIOL_MASK      // �Ƿ����ʱ�����־λ(д1��0)
				);

			while(!(FTFL->FSTAT & FTFL_FSTAT_CCIF_MASK));    // �ȴ��������

			// �������־
			if( FTFL->FSTAT 
				& (FTFL_FSTAT_ACCERR_MASK | FTFL_FSTAT_RDCOLERR_MASK | FTFL_FSTAT_FPVIOL_MASK | FTFL_FSTAT_MGSTAT0_MASK))
			{
				return 0 ;                                  //ִ���������
			}
			else
			{
				return 1;                                   //ִ������ɹ�
			}
		}

	};
}

#endif  /*_ADX_K60_FLASH_H_*/
