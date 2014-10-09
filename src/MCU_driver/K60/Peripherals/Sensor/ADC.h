
#ifndef _ADX_K60_ADC_H_
#define _ADX_K60_ADC_H_

#include "adxM3.h"
#include "../../System/System.h"
#include "../PeripheralBase.h"
#include "../PeripheralInfo.h"
#include "../Port/GlobalPort.h"
#include "../Port/SignalPin.h"

namespace K60
{

	template< UINT32 BASE >
	class Peripheral< ADC_INFO<BASE> >
	{
	public:

		typedef ADC_INFO<BASE> PeripheralInfo;
		typedef ClockGateController<typename PeripheralInfo::ClockSourceGate> ClockSource;



		Peripheral()
		{
			ClockSource::ClockOn();
			SetTriggerSource(Software);
			EnableHardwareAverageCalculation();

			SetModuleClockDivision(DIV_4);
			
			if (!HasCalibrated)
			{
				AutoCalibrate();
				HasCalibrated = true;
			}
			
		}

		~Peripheral()
		{
			ClockSource::ClockOff();
		}




		/*
		 * @brief	ADC自动校准
		 *			ADC外设初始化时需执行以校准分布电容对采样的影响
		 */
		static void AutoCalibrate()
		{
			// 校准前需要使用最低的ADC时钟频率，最高硬件平均采样次数
			EnableHardwareAverageCalculation(Every_32_Samples);
			
			// 频率设为BusClock除以2
			MICS input_clock = GetModuleInputClockSource();
			SetModuleInputClockSource(BusClock_DIV2);

			// 执行校准
			PeripheralInfo::BaseAccess()->SC3.CAL = SET;
			while (PeripheralInfo::BaseAccess()->SC3.CAL == SET);

			// plus side 计算校准结果
			UINT16 calibration_result = 0;
			calibration_result += PeripheralInfo::BaseAccess()->CLP0;
			calibration_result += PeripheralInfo::BaseAccess()->CLP1;
			calibration_result += PeripheralInfo::BaseAccess()->CLP2;
			calibration_result += PeripheralInfo::BaseAccess()->CLP3;
			calibration_result += PeripheralInfo::BaseAccess()->CLP4;
			//calibration_result += PeripheralInfo::BaseAccess()->CLPD;	// for diff mode?
			calibration_result += PeripheralInfo::BaseAccess()->CLPS;
			calibration_result >>= 1;				// div 2
			calibration_result |= _BV(15);			// set MSB
			PeripheralInfo::BaseAccess()->PG = calibration_result;

			// minus side 计算校准结果
			calibration_result = 0;
			calibration_result += PeripheralInfo::BaseAccess()->CLM0;
			calibration_result += PeripheralInfo::BaseAccess()->CLM1;
			calibration_result += PeripheralInfo::BaseAccess()->CLM2;
			calibration_result += PeripheralInfo::BaseAccess()->CLM3;
			calibration_result += PeripheralInfo::BaseAccess()->CLM4;
			//calibration_result += PeripheralInfo::BaseAccess()->CLMD;	// for diff mode?
			calibration_result += PeripheralInfo::BaseAccess()->CLMS;
			calibration_result >>= 1;				// div 2
			calibration_result |= _BV(15);			// set MSB
			PeripheralInfo::BaseAccess()->MG = calibration_result;

			// 恢复模块时钟频率
			SetModuleInputClockSource(input_clock);
			DisableHardwareAverageCalculation();
		}



		/*
		 * @brief	ADC检测校准是否失败
		 * @retval	返回true如果校准失败，否则返回false
		 */
		static bool CalibrateFail()
		{
			return PeripheralInfo::BaseAccess()->SC3.CALF;
		}



		/*
		 * @brief	ADC触发源选项
		 */
		enum TRGS
		{
			Software,
			Hardware
		};




		/*
		 * @brief	ADC设置转换触发源
		 * @param	source 触发源
		 *				@arg Software 软件触发
		 *				@arg Hardware 硬件触发
		 */
		static void SetTriggerSource(const TRGS source)
		{
			STATIC_ASSERT(Software == 0 && Hardware == 1,
				enumerate_value_is_not_campacted_with_this_configuration);
			PeripheralInfo::BaseAccess()->SC2.ADTRG = bool(source);
		}








		/*
		 * @brief	模块时钟分频选项
		 */
		enum MCKD
		{
			DIV_1,
			DIV_2,
			DIV_4,
			DIV_8
		};



		/*
		 * @brief	设置ADC模块时钟分频
		 * @param	div 分频因子，可以是MCKD枚举中的任何一个
		 */
		static void SetModuleClockDivision(const MCKD div)
		{
			STATIC_ASSERT(DIV_1 == 0 && DIV_2 == 1 && DIV_4 == 2 && DIV_8 == 3,
				enumerate_value_is_not_campacted_with_this_configuration);
			PeripheralInfo::BaseAccess()->CFG1.ADIV = div;
		}



		/*
		 * @brief	ADC模块输入时钟源选项
		 */
		enum MICS
		{
			BusClock_DIV1,
			BusClock_DIV2,
			AlternateClock,
			AsynchronousClock,
		};



		/*
		 * @brief	设置ADC模块输入时钟源
		 * @param	source 时钟源，可以是MICS枚举中的任何一个
		 */
		static void SetModuleInputClockSource(const MICS source)
		{
			STATIC_ASSERT( BusClock_DIV1 == 0 
						&& BusClock_DIV2 == 1 
						&& AlternateClock == 2 
						&& AsynchronousClock == 3,
				enumerate_value_is_not_campacted_with_this_configuration);
			PeripheralInfo::BaseAccess()->CFG1.ADICLK = source;
		}



		/*
		 * @brief	获取ADC模块输入时钟源
		 * @retval	时钟源，可以是MICS枚举中的任何一个
		 */
		static MICS GetModuleInputClockSource()
		{
			STATIC_ASSERT( BusClock_DIV1 == 0 
						&& BusClock_DIV2 == 1 
						&& AlternateClock == 2 
						&& AsynchronousClock == 3,
				enumerate_value_is_not_campacted_with_this_configuration);
			return MICS(PeripheralInfo::BaseAccess()->CFG1.ADICLK);
		}




		/*
		 * @brief	硬件平均数计算采样数
		 */
		enum SMPAVG
		{
			Every_4_Samples,
			Every_8_Samples,
			Every_16_Samples,
			Every_32_Samples,
		};



		/*
		 * @brief	ADC使能/禁止硬件平均数计算
		 * @param	div 分频因子，可以是MCKD枚举中的任何一个
		 */
		static void EnableHardwareAverageCalculation(const SMPAVG samples = Every_4_Samples)
		{
			STATIC_ASSERT( Every_4_Samples == 0 
						&& Every_8_Samples == 1 
						&& Every_16_Samples == 2 
						&& Every_32_Samples == 3,
				enumerate_value_is_not_campacted_with_this_configuration);
			PeripheralInfo::BaseAccess()->SC3.AVGE = SET;
			PeripheralInfo::BaseAccess()->SC3.AVGS = samples;
		}
		static void DisableHardwareAverageCalculation()
		{
			PeripheralInfo::BaseAccess()->SC3.AVGE = RESET;
		}




		/*
		 * @brief	ADC采样通道类模版
		 * @tparam	CHANNEL_INDEX 通道号
		 */
		template< UINT32 CHANNEL_INDEX >
		class CH
			: public adxM3::Sensor
		{

			typedef typename TL::TypeAt<typename PeripheralInfo::InputChannels, CHANNEL_INDEX>
				::Result InputChannelSignal;
			
			
			// InputChannelSignal是个信号引脚类型
			// 其构造时会自动配置多路复用器，并开启必要的时钟
			SignalPin<InputChannelSignal> InputChannelPin;
		
		public:

			typedef UINT16 ValueType;

			CH()
			{
				STATIC_ASSERT(CHANNEL_INDEX < PeripheralInfo::InputChannel_MAX,
					ADC_does_not_have_so_much_sensor_channels);

				ClockSource::ClockOn();
				SetTriggerSource(Software);
				SetAccuracy(16);
				SetSampleTime(VeryShort);
				SetModuleClockDivision(DIV_4);

				if (!HasCalibrated)
				{
					AutoCalibrate();
					HasCalibrated = true;
				}

				DisableHardwareAverageCalculation();
				ChannelCommand(CHANNEL_INDEX, ENABLE);
			}

			virtual ~CH()
			{
				ChannelCommand(CHANNEL_INDEX, DISABLE);
				ClockSource::ClockOff();
			}





			/*
			 * @brief	ADC采样通道开关控制
			 * @param	channel 采样通道
			 *			sta 使能状态
			 *				@arg ENABLE
			 *				@arg DISABLE
			 * @retval	成功开启或关闭返回1，否则返回0
			 */
			virtual int ChannelCommand(const int channel, FunctionalState sta)
			{
				if (sta == ENABLE)
				{
					PeripheralInfo::BaseAccess()->SC1[0].DIFF = RESET;
				}
				return 1;
			}



			/*
			 * @brief	ADC采样
			 * @param	result 采样结果保存的位置
			 * @retval	本次采样成功则返回1，否则返回0
			 */
			virtual int Sampling(int& result)
			{
				// 当写入该寄存器通道位（ADCH）时转换自动开始
				PeripheralInfo::BaseAccess()->wSC1[0] = CHANNEL_INDEX;

				// 超时保障
				int timeOutCount = 0;
				while (!ConvertComplete())
				{
					++timeOutCount;
					if (timeOutCount > 200000)
					{
						break;
					}
				}

				result = PeripheralInfo::BaseAccess()->R[0];

				return 1;
			}



			/*
			 * @brief	ADC设置精度
			 * @param	accbits 精度位数
			 * @retval	成功设置1，否则返回0
			 */
			virtual int SetAccuracy(const BYTE accbits)
			{
				return StaticSetAccuracyImpl(accbits);
			}



			/*
			 * @brief	ADC测试转换完成
			 * @retval	当前转换已完成返回true，否则返回false
			 */
			virtual bool ConvertComplete() const
			{
				return PeripheralInfo::BaseAccess()->SC1[0].COCO;
			}
	



			/*
			 * @brief	传感器采样时间配置
			 * @param	sample_time 采样时间，可以是SMPT枚举中的任何一个
			 * @retval	成功设置返回1，否则返回0
			 */
			virtual int SetSampleTime(const SMPT sample_time)
			{
				return StaticSetSampleTimeImpl(sample_time);
			}


			
			/*
			 * @brief	ADC参考电压选项
			 */
			enum VRS
			{
				ExternalVoltageReference,
				AlternatelVoltageReference
			};



			/*
			 * @brief	ADC设置参考电压
			 * @param	vref 参考电压源
			 *				@arg ExternalVoltageReference 外部电压参考
			 *				@arg AlternatelVoltageReference 复用电压参考（对于K60来说是内部参考）
			 */
			static void SetVoltageReference(const VRS vref)
			{
				STATIC_ASSERT(ExternalVoltageReference == 0 && AlternatelVoltageReference == 1,
					enumerate_value_is_not_campacted_with_this_configuration);
				PeripheralInfo::BaseAccess()->SC2.REFSEL = vref;
			}



		};



		template< UINT32 DIFF_CHANNEL_INDEX >
		class DiffCH
			: public adxM3::Sensor
		{

			typedef typename TL::TypeAt<typename PeripheralInfo::DifferentialChannels, DIFF_CHANNEL_INDEX>
				::Result DifferentialChannelSignal;

			SignalPin<typename DifferentialChannelSignal::DP> DifferentialChannelPinP;
			SignalPin<typename DifferentialChannelSignal::DM> DifferentialChannelPinM;


		public:

			typedef INT16 ValueType;


			DiffCH()
			{
				STATIC_ASSERT(DIFF_CHANNEL_INDEX < PeripheralInfo::DifferentialChannel_MAX,
					ADC_does_not_have_so_much_differential_channels);

				ClockSource::ClockOn();
				SetTriggerSource(Software);
				SetAccuracy(16);
				SetSampleTime(VeryShort);
				SetModuleClockDivision(DIV_4);

				if (!HasCalibrated)
				{
					AutoCalibrate();
					HasCalibrated = true;
				}

				DisableHardwareAverageCalculation();
				ChannelCommand(DIFF_CHANNEL_INDEX, ENABLE);
			}


			virtual ~DiffCH()
			{
				ChannelCommand(DIFF_CHANNEL_INDEX, DISABLE);
				ClockSource::ClockOff();
			}




			/*
			 * @brief	ADC差分通道采样通道开关控制
			 * @param	channel 采样通道
			 *			sta 使能状态
			 *				@arg ENABLE
			 *				@arg DISABLE
			 * @retval	成功开启或关闭返回1，否则返回0
			 */
			virtual int ChannelCommand(const int channel, FunctionalState sta)
			{
				if (sta == ENABLE)
				{
					PeripheralInfo::BaseAccess()->SC1[0].DIFF = SET;
				}
				else
				{
					PeripheralInfo::BaseAccess()->SC1[0].DIFF = RESET;
				}
				return 1;
			}



			/*
			 * @brief	ADC差分通道采样
			 * @param	result 采样结果保存的位置
			 * @retval	本次采样成功则返回1，否则返回0
			 */
			virtual int Sampling(int& result)
			{
				UINT32 regSC1 = (SET << ADC_BitBandType::SC1_DIFF) | DIFF_CHANNEL_INDEX;
				
				// 当写入该寄存器通道位（ADCH）时转换自动开始
				PeripheralInfo::BaseAccess()->wSC1[0] = regSC1;

				int timeOutCount = 0;
				while (!ConvertComplete())
				{
					++timeOutCount;
					if (timeOutCount > 200000)
					{
						break;
					}
				}
				result = (INT16)PeripheralInfo::BaseAccess()->R[0];

				return 1;
			}




			/*
			 * @brief	ADC差分通道设置精度
			 * @param	accbits 精度位数
			 * @retval	成功设置1，否则返回0
			 */
			virtual int SetAccuracy(const BYTE accbits)
			{
				return StaticSetAccuracyImpl(accbits);
			}



			/*
			 * @brief	ADC差分通道测试转换完成
			 * @retval	当前转换已完成返回true，否则返回false
			 */
			virtual bool ConvertComplete() const
			{
				return PeripheralInfo::BaseAccess()->SC1[0].COCO;
			}




			/*
			 * @brief	ADC差分通道采样时间配置
			 * @param	sample_time 采样时间，可以是SMPT枚举中的任何一个
			 * @retval	成功设置返回1，否则返回0
			 */
			virtual int SetSampleTime(const SMPT sample_time)
			{
				return StaticSetSampleTimeImpl(sample_time);
			}


		};


	private:

		// 已校准标记，避免多个通道开启时重复执行校准
		static bool HasCalibrated;



		inline static int StaticSetAccuracyImpl(const BYTE accbits)
		{
			switch (accbits)
			{
			case 16 :
				PeripheralInfo::BaseAccess()->CFG1.MODE = 3;
				return 1;

			case 12 :
				PeripheralInfo::BaseAccess()->CFG1.MODE = 1;
				return 1;

			case 10 :
				PeripheralInfo::BaseAccess()->CFG1.MODE = 2;
				return 1;

			case 8 :
				PeripheralInfo::BaseAccess()->CFG1.MODE = 0;
				return 1;

			default:
				return 0;
			}
		}



		inline static int StaticSetSampleTimeImpl(const adxM3::Sensor::SMPT sample_time)
		{
			switch (sample_time)
			{
			case adxM3::Sensor::VeryShort :
				PeripheralInfo::BaseAccess()->CFG1.ADLSMP = RESET;
				return 1;

			case adxM3::Sensor::Short :
				PeripheralInfo::BaseAccess()->CFG1.ADLSMP = SET;
				PeripheralInfo::BaseAccess()->CFG2.ADLSTS = 3;
				return 1;

			case adxM3::Sensor::Normal :
				PeripheralInfo::BaseAccess()->CFG1.ADLSMP = SET;
				PeripheralInfo::BaseAccess()->CFG2.ADLSTS = 2;
				return 1;

			case adxM3::Sensor::Long :
				PeripheralInfo::BaseAccess()->CFG1.ADLSMP = SET;
				PeripheralInfo::BaseAccess()->CFG2.ADLSTS = 1;
				return 1;

			case adxM3::Sensor::VeryLong :
			case adxM3::Sensor::ExtremelyLong :
			case adxM3::Sensor::MosquitoesAreAsleep :
				PeripheralInfo::BaseAccess()->CFG1.ADLSMP = SET;
				PeripheralInfo::BaseAccess()->CFG2.ADLSTS = 0;
				return 1;

			default: 
				return 0;
			}
		}

	};


	template< UINT32 BASE > 
	bool Peripheral< ADC_INFO<BASE> >::HasCalibrated = false;

}

#endif  /*_ADX_K60_ADC_H_*/
