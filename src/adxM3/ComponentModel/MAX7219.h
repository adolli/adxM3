
#ifndef _ADX_CM_MAX7219_H_
#define _ADX_CM_MAX7219_H_

#include "../HadwareAbstraction/Communicator/SynchronousCommunicator.h"

namespace adxM3
{

	class MAX7219
	{
	public:


		/*
		 * @brief	����Ĵ�����ַ
		 */
		enum Cmd
		{
			NOP				= 0x00,				// ��ָ�����ʱ���Ϳ�������������
			DecodeMode		= 0x09,				// ���뷽ʽ
			Intensity		= 0x0a,				// ���ȣ�ȡֵ0x0~0xf
			ScanLimit		= 0x0b,				// ɨ�跶Χ�������λ����ȡֵ0~7��
			PowerMode		= 0x0c,				// ����ģʽ
			DspyTest		= 0x0f				// �Ե�
		};



		/*
		 * @brief	λ������ʾ�Ĵ�����ַ
		 *			e.g. WriteReg(D0, 1);  ��0λ��ʾ1
		 */
		enum DigitBits
		{
			D0				= 0x01,				
			D1				= 0x02,				
			D2				= 0x03,				
			D3				= 0x04,				
			D4				= 0x05,				
			D5				= 0x06,				
			D6				= 0x07,				
			D7				= 0x08				
		};



		/*
		 * @brief	DecodeMode
		 */
		enum DCM
		{
			NoDecode		= 0x00,				// ������
			D0_BCD			= 0x01,				// ���λBCD�룬���಻����
			D3_0_BCD		= 0x0f,				// ��4λBCD�룬���಻����
			All_BCD			= 0xff				// ȫ��BCD����
		};



		/*
		 * @brief	PowerMode
		 */
		enum PM
		{
			PowerDown		= 0x00,				// ����ģʽ
			PowerOn			= 0x01
		};




		/*
		 * @brief	MAX7219��׼���캯��
		 *			�Զ�����SyncC��������Ĳ���
		 *			MAX7219�ϵ������
		 *			Ĭ�������롢ȫɨ�衢������
		 */
		MAX7219(SynchronousCommunicator& _SyncC)
			: SyncC(_SyncC)
		{
			SyncC.SetMode(ConfigurableCommunicator::MASTER);
			SyncC.SetBaudRate(1125000);		// 11.25MHz
			SyncC.SetDataBits(16);
			SyncC.SetClockPhase(SynchronousCommunicator::CPHA_1st_EDGE);
			SyncC.SetClockPolarity(SynchronousCommunicator::CPOL_LOW);
			SyncC.SetFirstBit(ConfigurableCommunicator::MSB);

			SetPowerMode(PowerOn);
			
			// ִ������
			for (int i = D0; i <= D7; ++i)
			{
				SetValue(DigitBits(i), 0);
			}

			DisplayTest(DISABLE);
			SetScanLimit(7);
			SetDecodeMode(NoDecode);
			SetIntensity(0);
		}



		~MAX7219()
		{
			SetPowerMode(PowerDown);
		}




		
		/*
		 * @brief	��������ģʽ
		 * @param	mode ����ģʽ
		 */
		void SetDecodeMode(DCM mode)
		{
			WriteReg(MAX7219::DecodeMode, mode);
		}



		/*
		 * @brief	���õ�Դģʽ
		 * @param	mode ��Դģʽ
		 */
		void SetPowerMode(PM mode)
		{
			WriteReg(MAX7219::PowerMode, mode);
		}



		/*
		 * @brief	��������
		 * @param	intens ����ȡֵ[0,0xf]
		 */
		void SetIntensity(UINT8 intens)
		{
			WriteReg(MAX7219::Intensity, intens);
		}



		/*
		 * @brief	����ɨ�跶Χ
		 * @param	range �����������˵����ʾλ�������ڵ���Һ������˵��������
		 *				ȡֵ��Χ [0,0xf]
		 */
		void SetScanLimit(UINT8 range)
		{
			WriteReg(MAX7219::ScanLimit, range);
		}



		/*
		 * @brief	�Ե�
		 * @param	sta ��staΪENABLEʱΪ�Ե�ģʽ������Ϊ������ʾģʽ 
		 */
		void DisplayTest(FunctionalState sta)
		{
			WriteReg(MAX7219::DspyTest, sta);
		}





		/*
		 * @brief	������ʾֵ
		 * @param	digit ����Һ�������л�������ܵ�λ
		 *			val ��ʾ��ֵ
		 */
		void SetValue(DigitBits digit, BYTE val)
		{
			WriteReg(digit, val);
		}


	protected:

		SynchronousCommunicator& SyncC;


	private:

		/*
		 * @brief	д�Ĵ���
		 */
		void WriteReg(BYTE reg, BYTE val)
		{
			UINT16 tx = ((UINT16)reg << 8) | val;
			SyncC.SynchronouslyTranceive(tx);
		}

	};
}

#endif  /*_ADX_CM_MAX7219_H_*/
