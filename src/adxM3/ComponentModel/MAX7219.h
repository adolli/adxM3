
#ifndef _ADX_CM_MAX7219_H_
#define _ADX_CM_MAX7219_H_

#include "../HadwareAbstraction/Communicator/SynchronousCommunicator.h"

namespace adxM3
{

	class MAX7219
	{
	public:


		/*
		 * @brief	命令寄存器地址
		 */
		enum Cmd
		{
			NOP				= 0x00,				// 空指令（级联时发送控制令跳过区域）
			DecodeMode		= 0x09,				// 译码方式
			Intensity		= 0x0a,				// 亮度，取值0x0~0xf
			ScanLimit		= 0x0b,				// 扫描范围（数码管位数，取值0~7）
			PowerMode		= 0x0c,				// 掉电模式
			DspyTest		= 0x0f				// 试灯
		};



		/*
		 * @brief	位数段显示寄存器地址
		 *			e.g. WriteReg(D0, 1);  第0位显示1
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
			NoDecode		= 0x00,				// 不译码
			D0_BCD			= 0x01,				// 最低位BCD码，其余不译码
			D3_0_BCD		= 0x0f,				// 低4位BCD码，其余不译码
			All_BCD			= 0xff				// 全部BCD译码
		};



		/*
		 * @brief	PowerMode
		 */
		enum PM
		{
			PowerDown		= 0x00,				// 掉电模式
			PowerOn			= 0x01
		};




		/*
		 * @brief	MAX7219标准构造函数
		 *			自动配置SyncC外设所需的参数
		 *			MAX7219上电后清屏
		 *			默认无译码、全扫描、低亮度
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
			
			// 执行清屏
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
		 * @brief	设置译码模式
		 * @param	mode 译码模式
		 */
		void SetDecodeMode(DCM mode)
		{
			WriteReg(MAX7219::DecodeMode, mode);
		}



		/*
		 * @brief	设置电源模式
		 * @param	mode 电源模式
		 */
		void SetPowerMode(PM mode)
		{
			WriteReg(MAX7219::PowerMode, mode);
		}



		/*
		 * @brief	设置亮度
		 * @param	intens 亮度取值[0,0xf]
		 */
		void SetIntensity(UINT8 intens)
		{
			WriteReg(MAX7219::Intensity, intens);
		}



		/*
		 * @brief	设置扫描范围
		 * @param	range 对于数码管来说是显示位数，对于点阵液晶屏来说是行数，
		 *				取值范围 [0,0xf]
		 */
		void SetScanLimit(UINT8 range)
		{
			WriteReg(MAX7219::ScanLimit, range);
		}



		/*
		 * @brief	试灯
		 * @param	sta 当sta为ENABLE时为试灯模式，否则为正常显示模式 
		 */
		void DisplayTest(FunctionalState sta)
		{
			WriteReg(MAX7219::DspyTest, sta);
		}





		/*
		 * @brief	设置显示值
		 * @param	digit 点阵液晶屏的行或者数码管的位
		 *			val 显示的值
		 */
		void SetValue(DigitBits digit, BYTE val)
		{
			WriteReg(digit, val);
		}


	protected:

		SynchronousCommunicator& SyncC;


	private:

		/*
		 * @brief	写寄存器
		 */
		void WriteReg(BYTE reg, BYTE val)
		{
			UINT16 tx = ((UINT16)reg << 8) | val;
			SyncC.SynchronouslyTranceive(tx);
		}

	};
}

#endif  /*_ADX_CM_MAX7219_H_*/
