
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
		 * @brief	ADC�Զ�У׼
		 *			ADC�����ʼ��ʱ��ִ����У׼�ֲ����ݶԲ�����Ӱ��
		 */
		static void AutoCalibrate()
		{
			// У׼ǰ��Ҫʹ����͵�ADCʱ��Ƶ�ʣ����Ӳ��ƽ����������
			EnableHardwareAverageCalculation(Every_32_Samples);
			
			// Ƶ����ΪBusClock����2
			MICS input_clock = GetModuleInputClockSource();
			SetModuleInputClockSource(BusClock_DIV2);

			// ִ��У׼
			PeripheralInfo::BaseAccess()->SC3.CAL = SET;
			while (PeripheralInfo::BaseAccess()->SC3.CAL == SET);

			// plus side ����У׼���
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

			// minus side ����У׼���
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

			// �ָ�ģ��ʱ��Ƶ��
			SetModuleInputClockSource(input_clock);
			DisableHardwareAverageCalculation();
		}



		/*
		 * @brief	ADC���У׼�Ƿ�ʧ��
		 * @retval	����true���У׼ʧ�ܣ����򷵻�false
		 */
		static bool CalibrateFail()
		{
			return PeripheralInfo::BaseAccess()->SC3.CALF;
		}



		/*
		 * @brief	ADC����Դѡ��
		 */
		enum TRGS
		{
			Software,
			Hardware
		};




		/*
		 * @brief	ADC����ת������Դ
		 * @param	source ����Դ
		 *				@arg Software �������
		 *				@arg Hardware Ӳ������
		 */
		static void SetTriggerSource(const TRGS source)
		{
			STATIC_ASSERT(Software == 0 && Hardware == 1,
				enumerate_value_is_not_campacted_with_this_configuration);
			PeripheralInfo::BaseAccess()->SC2.ADTRG = bool(source);
		}








		/*
		 * @brief	ģ��ʱ�ӷ�Ƶѡ��
		 */
		enum MCKD
		{
			DIV_1,
			DIV_2,
			DIV_4,
			DIV_8
		};



		/*
		 * @brief	����ADCģ��ʱ�ӷ�Ƶ
		 * @param	div ��Ƶ���ӣ�������MCKDö���е��κ�һ��
		 */
		static void SetModuleClockDivision(const MCKD div)
		{
			STATIC_ASSERT(DIV_1 == 0 && DIV_2 == 1 && DIV_4 == 2 && DIV_8 == 3,
				enumerate_value_is_not_campacted_with_this_configuration);
			PeripheralInfo::BaseAccess()->CFG1.ADIV = div;
		}



		/*
		 * @brief	ADCģ������ʱ��Դѡ��
		 */
		enum MICS
		{
			BusClock_DIV1,
			BusClock_DIV2,
			AlternateClock,
			AsynchronousClock,
		};



		/*
		 * @brief	����ADCģ������ʱ��Դ
		 * @param	source ʱ��Դ��������MICSö���е��κ�һ��
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
		 * @brief	��ȡADCģ������ʱ��Դ
		 * @retval	ʱ��Դ��������MICSö���е��κ�һ��
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
		 * @brief	Ӳ��ƽ�������������
		 */
		enum SMPAVG
		{
			Every_4_Samples,
			Every_8_Samples,
			Every_16_Samples,
			Every_32_Samples,
		};



		/*
		 * @brief	ADCʹ��/��ֹӲ��ƽ��������
		 * @param	div ��Ƶ���ӣ�������MCKDö���е��κ�һ��
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
		 * @brief	ADC����ͨ����ģ��
		 * @tparam	CHANNEL_INDEX ͨ����
		 */
		template< UINT32 CHANNEL_INDEX >
		class CH
			: public adxM3::Sensor
		{

			typedef typename TL::TypeAt<typename PeripheralInfo::InputChannels, CHANNEL_INDEX>
				::Result InputChannelSignal;
			
			
			// InputChannelSignal�Ǹ��ź���������
			// �乹��ʱ���Զ����ö�·����������������Ҫ��ʱ��
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
			 * @brief	ADC����ͨ�����ؿ���
			 * @param	channel ����ͨ��
			 *			sta ʹ��״̬
			 *				@arg ENABLE
			 *				@arg DISABLE
			 * @retval	�ɹ�������رշ���1�����򷵻�0
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
			 * @brief	ADC����
			 * @param	result ������������λ��
			 * @retval	���β����ɹ��򷵻�1�����򷵻�0
			 */
			virtual int Sampling(int& result)
			{
				// ��д��üĴ���ͨ��λ��ADCH��ʱת���Զ���ʼ
				PeripheralInfo::BaseAccess()->wSC1[0] = CHANNEL_INDEX;

				// ��ʱ����
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
			 * @brief	ADC���þ���
			 * @param	accbits ����λ��
			 * @retval	�ɹ�����1�����򷵻�0
			 */
			virtual int SetAccuracy(const BYTE accbits)
			{
				return StaticSetAccuracyImpl(accbits);
			}



			/*
			 * @brief	ADC����ת�����
			 * @retval	��ǰת������ɷ���true�����򷵻�false
			 */
			virtual bool ConvertComplete() const
			{
				return PeripheralInfo::BaseAccess()->SC1[0].COCO;
			}
	



			/*
			 * @brief	����������ʱ������
			 * @param	sample_time ����ʱ�䣬������SMPTö���е��κ�һ��
			 * @retval	�ɹ����÷���1�����򷵻�0
			 */
			virtual int SetSampleTime(const SMPT sample_time)
			{
				return StaticSetSampleTimeImpl(sample_time);
			}


			
			/*
			 * @brief	ADC�ο���ѹѡ��
			 */
			enum VRS
			{
				ExternalVoltageReference,
				AlternatelVoltageReference
			};



			/*
			 * @brief	ADC���òο���ѹ
			 * @param	vref �ο���ѹԴ
			 *				@arg ExternalVoltageReference �ⲿ��ѹ�ο�
			 *				@arg AlternatelVoltageReference ���õ�ѹ�ο�������K60��˵���ڲ��ο���
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
			 * @brief	ADC���ͨ������ͨ�����ؿ���
			 * @param	channel ����ͨ��
			 *			sta ʹ��״̬
			 *				@arg ENABLE
			 *				@arg DISABLE
			 * @retval	�ɹ�������رշ���1�����򷵻�0
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
			 * @brief	ADC���ͨ������
			 * @param	result ������������λ��
			 * @retval	���β����ɹ��򷵻�1�����򷵻�0
			 */
			virtual int Sampling(int& result)
			{
				UINT32 regSC1 = (SET << ADC_BitBandType::SC1_DIFF) | DIFF_CHANNEL_INDEX;
				
				// ��д��üĴ���ͨ��λ��ADCH��ʱת���Զ���ʼ
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
			 * @brief	ADC���ͨ�����þ���
			 * @param	accbits ����λ��
			 * @retval	�ɹ�����1�����򷵻�0
			 */
			virtual int SetAccuracy(const BYTE accbits)
			{
				return StaticSetAccuracyImpl(accbits);
			}



			/*
			 * @brief	ADC���ͨ������ת�����
			 * @retval	��ǰת������ɷ���true�����򷵻�false
			 */
			virtual bool ConvertComplete() const
			{
				return PeripheralInfo::BaseAccess()->SC1[0].COCO;
			}




			/*
			 * @brief	ADC���ͨ������ʱ������
			 * @param	sample_time ����ʱ�䣬������SMPTö���е��κ�һ��
			 * @retval	�ɹ����÷���1�����򷵻�0
			 */
			virtual int SetSampleTime(const SMPT sample_time)
			{
				return StaticSetSampleTimeImpl(sample_time);
			}


		};


	private:

		// ��У׼��ǣ�������ͨ������ʱ�ظ�ִ��У׼
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
