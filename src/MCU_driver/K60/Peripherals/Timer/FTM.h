
#ifndef _ADX_K60_FTM_H_
#define _ADX_K60_FTM_H_

#include "adxM3.h"
#include "../../System/System.h"
#include "../PeripheralBase.h"
#include "../PeripheralInfo.h"
#include "../Port/GlobalPort.h"
#include "../Port/SignalPin.h"

namespace K60
{

	/*
	 * @brief	FlexTimer
	 *			FTM�����Ǹ�ʱ����Ԫ���̳в�ʵ��adxM3��ʱ��
	 *			
	 */
	template< UINT32 BASE >
	class Peripheral< FTM_INFO<BASE> >
		: public adxM3::Timer
	{

		typedef FTM_INFO<BASE> PeripheralInfo;
		typedef ClockGateController<typename PeripheralInfo::ClockSourceGate> ClockSource;

	public:



		/*
		 * @brief	��ʱ��ʱ��Դѡ��
		 */
		enum CLKSRC
		{
			NoClockSource			= 0,
			SystemClock				= 1,
			FixedFrequencyClock		= 2,
			ExternalClock			= 3
		};




		/*
		 * @brief	��׼���캯��
		 *			��ʼ��ʱĬ��ʱ��ԴΪϵͳʱ��
		 */
		Peripheral(UINT32 time_us = 0)
			: previousClockSource(SystemClock)
		{
			ClockSource::ClockOn();
			

			// ע��ȫ��ʵ��ָ��
			PeripheralInfo::InstancePtr = this;

			if (time_us > 0)
			{
				SetTimeOut(time_us);
			}

			// ����NVIC
			CM4::$NVIC::SetPriority(PeripheralInfo::PeripheralIRQ,
				PeripheralInfo::InteruptPriority);
			CM4::$NVIC::EnableIRQ(PeripheralInfo::PeripheralIRQ);

			
		}


		virtual ~Peripheral()
		{
			ClockSource::ClockOff();
		}




		/*
		 * @brief	������ʱ
		 */
		virtual void Start()
		{
			SetClockSource(previousClockSource);
		}





		/*
		 * @brief	ֹͣ��ʱ
		 *			�����������ع鵽��ʼֵ
		 */
		virtual void Stop()
		{
			// ���Ѿ�ֹͣʱ������ֹͣ�����ⶪʧ��ǰ��ʱ��Դ��Ϣ
			CLKSRC clksrc = GetClockSource();
			if (clksrc != NoClockSource)
			{
				previousClockSource = clksrc;
				SetClockSource(NoClockSource);

				// д������ֵ��CNT�Ĵ����Գ�ʼ����ֵΪCNTIN��ֵ
				PeripheralInfo::BaseAccess()->CNT = 0;
			}
		}



		/*
		 * @brief	��ͣ��ʱ����ʱֵ������
		 *			�������Continue�������ʹ��
		 */
		virtual void Pause()
		{
			// ��ʱ��Դ�г�
			CLKSRC clksrc = GetClockSource();
			if (clksrc != NoClockSource)
			{
				previousClockSource = clksrc;
				SetClockSource(NoClockSource);
			}
		}



		/*
		 * @brief	������ʱ����Pause���ʹ��
		 */
		virtual void Continue()
		{
			//	�ָ�ʱ��Դ
			SetClockSource(previousClockSource);
		}




		enum
		{
			PulsePerUS = PeripheralInfo::ModuleClock::Frequency / 1000000,
			ModuleClockFrequencyInMHz = PulsePerUS,
			ModuleClockFrequencyInHz = PeripheralInfo::ModuleClock::Frequency
		};



		/*
		 * @brief	FTM����/��ȡ���Ƶ��ֵ
		 * @param	psc_value Ԥ��Ƶ��ֵ
		 *				@arg 1, 2, 4, 8, 16, 32, 64, 128
		 * @notice	�벻Ҫ���������г���ֵ�����ֵ
		 */
		void SetPrescaler(UINT8 psc_value)
		{
			StaticSetPrescalerImpl(psc_value);
		}
		UINT8 GetPrescaler() const
		{
			return StaticGetPrescalerImpl();
		}



		/*
		 * @brief	��ȡ��ǰ��ʱ����ʱʱ��ֵ
		 */
		virtual UINT32 GetTime() const
		{
			UINT8 psc = GetPrescaler();
			
			// ������Ѿ�������tick���������ڼ���ֵ��ȥ��ʼ����ֵ
			// ��ʹ���з��ŵļ��㣬���ת�����޷��ţ�
			UINT32 countedVal = (UINT32)((int)PeripheralInfo::BaseAccess()->CNT - (int)PeripheralInfo::BaseAccess()->CNTIN);
			return countedVal * psc / PulsePerUS;
		}



		/*
		 * @brief	���ü�ʱ����ʱʱ��
		 *			��λ us
		 * @notice	��ʱ���Ȳ�Ҫ���� 93,200 us
		 */
		virtual void SetTimeOut(UINT32 time_us)
		{
			StaticSetTimeOutImpl(time_us);
		}



		/*
		 * @brief	���ö�ʱ����ʱƵ��
		 * @notice	����ͬSetTimeOut�������õ���TimeOut�ĵ�������Ƶ��
		 *			Ƶ����СֵΪ11Hz��С�ڴ�ֵʱ�Զ�����
		 */
		virtual void SetFrequency(UINT32 freq)
		{
			StaticSetFrequencyImpl(freq);
		}



		/*
		 * @brief	��ȡ��ʱ����ʱʱ��
		 * @notice	����ʱʱ���ϳ�ʱ������10ms������ȡֵʱ���ܻ����һ�����
		 */
		virtual UINT32 GetTimeOut() const
		{
			return StaticGetTimeOutImpl();
		}



		/*
		 * @brief	��ȡ��ʱ����ʱƵ��
		 */
		virtual UINT32 GetFrequency() const
		{
			return StaticGetFrequencyImpl();
		}






		/*
		 * @brief	�ж϶�ʱ���Ƿ����ڼ�ʱ
		 * @retval	���ڼ�ʱ����true�����򷵻�false
		 */
		virtual bool TimerIsRunning() const
		{
			return (PeripheralInfo::BaseAccess()->SC.CLKS != 0);
		}





		/*
		 * @brief	�¼�֪ͨ���ؿ���
		 * @param	evt ��Ҫ������ر�֪ͨ���¼�����
		 * @retval	�ɹ�ʹ��/��ֹĳ���¼�֪ͨ�򷵻�1�����򷵻�0
		 */
		virtual int EnableEventNotify(EventType evt)
		{
			switch (evt)
			{
			case TIME_OUT :
				PeripheralInfo::BaseAccess()->SC.TOIE = SET;
				return 1;

			default:
				return 0;
			}
		}
		virtual int DisableEventNotify(EventType evt)
		{
			switch (evt)
			{
			case TIME_OUT :
				PeripheralInfo::BaseAccess()->SC.TOIE = RESET;
				return 1;

			default:
				return 0;
			}
		}



		/*
		 * @brief	����¼��������������¼������ĳ���¼�
		 * @param	evt ������¼�
		 *			FnPtr �������¼�ʱ�Ĵ�����������������ʵ��ʱת�������������
		 * @retval	�ɹ�ע��ĳ���¼��������򷵻�1�����򷵻�0
		 */
		virtual int AddEventListener(EventType evt, void* FnPtr)
		{
			switch (evt)
			{
			case TIME_OUT :
				timeOutISR = FnPtr;
				PeripheralInfo::BaseAccess()->SC.TOIE = SET;
				return 1;

			default:
				return 0;
			}
		}





		/*
		 * @brief	FTMѡ��ʱ��Դ
		 * @param	clksrc ʱ��Դ��������CLKSRCö���е��κ�һ��
		 */
		void SetClockSource(const CLKSRC clksrc)
		{
			if (PeripheralInfo::BaseAccess()->MODE.WPDIS == 0)
			{
				PeripheralInfo::BaseAccess()->MODE.WPDIS = SET;
			}	
			PeripheralInfo::BaseAccess()->SC.CLKS = clksrc;
		}



		/*
		 * @brief	FTM��ȡ��ǰʱ��Դ
		 * @retval	ʱ��Դ��������CLKSRCö���е��κ�һ��
		 */
		CLKSRC GetClockSource() const
		{
			return CLKSRC(PeripheralInfo::BaseAccess()->SC.CLKS);
		}




		/*
		 * @brief	��������������ѡ��
		 */
		enum CNTDIR
		{
			CountUp,
			CenterAligned
		};



		/*
		 * @brief	FTM����/��ȡ��������������
		 * @param	dir �������򣬿�����CNTDIRö���е��κ�һ��
		 * @retval	��������
		 * @notice	�÷������ڷ�д����״̬(MODE[WPDIS] == 1)����Ч
		 */
		void SetCounterDirection(const CNTDIR dir)
		{
			STATIC_ASSERT(CountUp == 0 && CenterAligned == 1,
				enumerate_value_is_not_campacted_with_this_configuration);
			PeripheralInfo::BaseAccess()->SC.CPWMS = dir;
		}
		CNTDIR GetCounterDirection() const
		{
			return CNTDIR(PeripheralInfo::BaseAccess()->SC.CPWMS);
		}



		// ��ʱ������¼���Ӧ����
		adxM3::Function< void(void*) > timeOutISR;


	private:

		CLKSRC previousClockSource;


	public:


		/*
		 * @brief	FTM���벶���������Ƚ�ͨ��
		 */
		template< UINT32 CHANNEL_INDEX >
		class CH
		{
		public:

			CH()
			{
				STATIC_ASSERT(CHANNEL_INDEX < PeripheralInfo::Channel_MAX,
					FTM_does_not_have_so_much_capture_or_compare_channels);

				ClockSource::ClockOn();

				// ����NVIC
				CM4::$NVIC::SetPriority(PeripheralInfo::PeripheralIRQ,
					PeripheralInfo::InteruptPriority);
				CM4::$NVIC::EnableIRQ(PeripheralInfo::PeripheralIRQ);
			}

			~CH()
			{
				ClockSource::ClockOff();
			}



			/*
			 * @brief	����/�Ƚ�ģʽѡ��
			 */
			enum CCMODE
			{
				CaptureOnRisingEdge		= 0x1,
				CaptureOnFallingEdge	= 0x2,
				CaptureOnBothEdge		= 0x3,
				ToggleOutputOnMatch		= 0x5,
				ClearOutputOnMatch		= 0x6,
				SetOutputOnMatch		= 0x7,
			};



			/*
			 * @brief	����/�Ƚ�ģʽ����
			 * @param	mode ����ģʽ���߱Ƚ�ģʽ��������CCMODEö���е��κ�һ��
			 *			comparaVal ����Ƚ�ֵ����������Ƚ�ģʽ�¸ò���������
			 */
			void CaptureCompareConfig(const CCMODE mode, const UINT16 comparaVal = 0)
			{
				STATIC_ASSERT(
					CaptureOnRisingEdge		== 0x1 &&
					CaptureOnFallingEdge	== 0x2 &&
					CaptureOnBothEdge		== 0x3 &&
					ToggleOutputOnMatch		== 0x5 &&
					ClearOutputOnMatch		== 0x6 &&
					SetOutputOnMatch		== 0x7,
					enumerate_value_is_not_campacted_with_this_configuration);


				PeripheralInfo::BaseAccess()
					->CONTROLS[CHANNEL_INDEX].CnSC.MS = (mode & 0xC) >> 2;
				PeripheralInfo::BaseAccess()
					->CONTROLS[CHANNEL_INDEX].CnSC.ELS = mode & 0x3;

				// ���ñȽ�ֵ��ֻ������Ƚ�ģʽ����Ч��
				PeripheralInfo::BaseAccess()->CONTROLS[CHANNEL_INDEX].CnV = comparaVal;
			}






		protected:

			typedef typename PeripheralInfo::Channels Channels;
			typedef typename TL::TypeAt<Channels, CHANNEL_INDEX>::Result ChannelSignal;
			
			SignalPin<ChannelSignal> ChannelPin;

		};



		/*
		 * @brief	FTM PWM���ͨ��
		 */
		template< UINT32 CHANNEL_INDEX >
		class PWM
			: protected CH<CHANNEL_INDEX>
			, public adxM3::PulseWidthModulator
			, public adxM3::TimerIOEnabler
		{
		public:


			typedef Peripheral<PeripheralInfo> PeripheralFTM;



			/*
			 * @brief	PWMͨ����׼���캯��
			 *			����ʱĬ������ΪHighTruePulsePWM
			 *			����ʱ���ı䶨ʱ�����ڣ���ע�������úö�ʱ���ٹ���PWMͨ��
			 * @param	duty ռ�ձȣ���Χ[0,1]
			 *			period PWM�����ڣ�����Ϊ0ʱ��ʾ���ı�ԭ����������
			 *			align PWM����ģʽ
			 *				@arg EdgeAligned ���ض���
			 *				@arg CenterAligned �������
			 */
			PWM(float duty = 0, const UINT32 period = 0, const PWMALIGN align = EdgeAligned)
			{
				STATIC_ASSERT(CHANNEL_INDEX < PeripheralInfo::Channel_MAX,
					FTM_does_not_have_so_much_PWM_channels);

				// ����ΪHighTruePulseģʽ
				PeripheralInfo::BaseAccess()->CONTROLS[CHANNEL_INDEX].CnSC.MS = 3;
				PeripheralInfo::BaseAccess()->CONTROLS[CHANNEL_INDEX].CnSC.ELS = 2;

				// ���ñ��밴�����µ�˳��

				// ���ö���ģʽ
				SetAlignment(align);

				// ����PWM������
				if (period != 0)
				{
					SetPulsePeriod(period);
				}

				// ����PWMռ�ձ�
				SetDutyCycle(duty);

				// ����ʱ��Դ
				PeripheralInfo::BaseAccess()->SC.CLKS = 1;

				// �Ƚ�ֵ����װ��
				PeripheralInfo::BaseAccess()->PWMLOAD.LDOK = SET;
				PeripheralInfo::BitBandAccess()->PWMLOAD[CHANNEL_INDEX] = SET;
			}

			virtual ~PWM()
			{
			}




			/*
			 * @brief	������������ʹ��/��ֹ
			 *			�����ú�����������ģʽ����
			 * @retval	�ɹ�����/�ر��������1�����򷵻�0
			 */
			virtual int EnableOutput()
			{
				PeripheralInfo::BitBandAccess()->OUTMASK[CHANNEL_INDEX] = RESET;
				return 1;
			}
			virtual int DisableOutput()
			{
				PeripheralInfo::BitBandAccess()->OUTMASK[CHANNEL_INDEX] = SET;
				return 1;
			}




			/*
			 * @brief	�������������ռ�ձȣ��ٷֱȣ���ȷ��1%��
			 * @param	duty ռ�ձȣ�ȡֵ��Χ[0,1]���˷���ֻȡ��С�����֣������û���Ҫ����ȡֵ��Χ��
			 * @param	duty_percent ռ�ձȣ�ȡֵ��Χ[0,100]��
			 *				����һ���Ǹ���İ汾��ʵ��ʱ�벻Ҫ�漰��������
			 */
			virtual void SetDutyCycle(float duty)
			{
				INT16 cntin = PeripheralInfo::BaseAccess()->CNTIN;
				INT16 mod = PeripheralInfo::BaseAccess()->MOD;

				if (GetAlignment() == EdgeAligned)
				{
					PeripheralInfo::BaseAccess()->CONTROLS[CHANNEL_INDEX].CnV 
						= duty * (mod - cntin + 1) + cntin;
				}
				else // Center Aligned
				{
					PeripheralInfo::BaseAccess()->CONTROLS[CHANNEL_INDEX].CnV 
						= duty * (mod - cntin) + cntin;
				}
			}
			virtual void SetDutyCyclePercent(const UINT8 duty_percent)
			{
				INT16 cntin = PeripheralInfo::BaseAccess()->CNTIN;
				INT16 mod = PeripheralInfo::BaseAccess()->MOD;
				
				if (GetAlignment() == EdgeAligned)
				{
					PeripheralInfo::BaseAccess()->CONTROLS[CHANNEL_INDEX].CnV 
						= duty_percent * (mod - cntin + 1) / 100 + cntin;
				}
				else // Center Aligned
				{
					PeripheralInfo::BaseAccess()->CONTROLS[CHANNEL_INDEX].CnV 
						= duty_percent * (mod - cntin) / 100 + cntin;
				}
			}



			/*
			 * @brief	�����������ȡռ�ձȣ��ٷֱȣ���ȷ��1%��
			 * @retval	ռ�ձȣ�ȡֵ��Χ[0,1]
			 */
			virtual float GetDutyCycle() const
			{
				INT16 cntin = PeripheralInfo::BaseAccess()->CNTIN;
				INT16 mod = PeripheralInfo::BaseAccess()->MOD;
				INT16 cnv = PeripheralInfo::BaseAccess()->CONTROLS[CHANNEL_INDEX].CnV;

				if (GetAlignment() == EdgeAligned)
				{
					return (float)(cnv - cntin) / (mod - cntin + 1);
				}
				else
				{
					return (float)(cnv - cntin) / (mod - cntin);
				}
			}
			virtual UINT8 GetDutyCyclePercent() const
			{
				INT16 cntin = PeripheralInfo::BaseAccess()->CNTIN;
				INT16 mod = PeripheralInfo::BaseAccess()->MOD;
				INT16 cnv = PeripheralInfo::BaseAccess()->CONTROLS[CHANNEL_INDEX].CnV;

				if (GetAlignment() == EdgeAligned)
				{
					return (cnv - cntin) * 100 / (mod - cntin + 1);
				}
				else
				{
					return (cnv - cntin) * 100 / (mod - cntin);
				}
			}



			/*
			 * @brief	���������������������
			 *			�����������ڲ���ı�ռ�ձ�
			 * @param	period_us �������ڣ���λus
			 */
			virtual void SetPulsePeriod(const UINT32 period_us)
			{
				StaticSetTimeOutImpl(period_us);
			}





			/*
			 * @brief	���������������������
			 * @retval	�������ڣ���λus
			 */
			virtual UINT32 GetPulsePeriod() const
			{
				return PeripheralFTM::StaticGetTimeOutImpl();
			}





			/*
			 * @brief	������������÷���Ƶ��
			 * @param	freq ���壨���������ڣ���λHz�����ٴ���22Hz
			 * @retval	���壨���������ڣ���λHz
			 */
			virtual void SetFrequency(const UINT32 freq)
			{
				StaticSetFrequencyImpl(freq);
			}
			virtual UINT32 GetFrequency() const
			{
				return StaticGetFrequencyImpl();
			}




			/*
			 * @brief	������������øߵ�ƽ����
			 * @param	width �����ȣ���λus����ֵ�벻Ҫ������������
			 * @retval	�����ȣ���λus
			 */
			virtual void SetPulseWidth(const UINT32 width)
			{
				INT16 cntin = PeripheralInfo::BaseAccess()->CNTIN;
				UINT8 psc = StaticGetPrescalerImpl();

				PeripheralInfo::BaseAccess()->CONTROLS[CHANNEL_INDEX].CnV 
					= width * PulsePerUS / psc + cntin;
			}
			virtual UINT32 GetPulseWidth() const
			{
				INT16 cntin = PeripheralInfo::BaseAccess()->CNTIN;
				INT16 cnv = PeripheralInfo::BaseAccess()->CONTROLS[CHANNEL_INDEX].CnV;
				UINT8 psc = StaticGetPrescalerImpl();
				return UINT32(cnv - cntin) * psc / PulsePerUS;
			}




			/*
			 * @brief	PWM��������
			 * @param	pol ���ԣ�������PWMPOLö�����κ�һ��
			 *				@arg ActiveLow �ɹ��Ƚ�ʱ�͵�ƽ(LowTruePulse)
			 *				@arg ActiveHigh �ɹ��Ƚ�ʱ�ߵ�ƽ(HighTruePulse)
			 */
			virtual void SetPolarity(const PWMPOL pol)
			{
				STATIC_ASSERT(ActiveLow == 0 && ActiveHigh == 1,
					enumerate_value_is_not_campacted_with_this_configuration);
				PeripheralInfo::BitBandAccess()->POL[CHANNEL_INDEX] = !pol;
			}




			/*
			 * @brief	PWM��������/��ȡ
			 * @param	align PWM����ģʽ��������PWMALIGNö���е��κ�һ��
			 */
			void SetAlignment(const PWMALIGN align)
			{
				STATIC_ASSERT(EdgeAligned == 0 && CenterAligned == 1,
					enumerate_value_is_not_campacted_with_this_configuration);
				
				PeripheralInfo::BaseAccess()->SC.CPWMS = align;
			}
			PWMALIGN GetAlignment() const 
			{
				STATIC_ASSERT(EdgeAligned == 0 && CenterAligned == 1,
					enumerate_value_is_not_campacted_with_this_configuration);

				return PWMALIGN(PeripheralInfo::BaseAccess()->SC.CPWMS);
			}





		protected:

			typedef typename PeripheralInfo::Channels Channels;
			typedef typename TL::TypeAt<Channels, CHANNEL_INDEX>::Result ChannelSignal;
			
			// PWMͨ������
			SignalPin<ChannelSignal> ChannelPin;

		};



		/*
		 * @brief	FTM ����PWM���ͨ��
		 *			ֻ��ΪEdgeAligned���뷽ʽ
		 */
		template< UINT32 PAIR_CHANNEL_INDEX >
		class ComplementaryPWM
			: public PWM<PAIR_CHANNEL_INDEX * 2>
			, public PWM<PAIR_CHANNEL_INDEX * 2 + 1>
		{

			/*
			 * @brief	˫·PWMͨ����
			 */
			enum 
			{
				PAIR_CH	= PAIR_CHANNEL_INDEX,
				CHA		= PAIR_CHANNEL_INDEX * 2,
				CHB		= PAIR_CHANNEL_INDEX * 2 + 1
			};


		public:


			using PWM<CHA>::GetAlignment;


			ComplementaryPWM(float duty = 0, const UINT32 period = 0)
				: PWM<CHA>(0, period), PWM<CHB>(duty)
			{
				PeripheralInfo::BaseAccess()->MODE.WPDIS = SET;
				PeripheralInfo::BaseAccess()->MODE.FTMEN = SET;


				// ���ģʽ����
				PeripheralInfo::BitBandAccess()
				   ->COMBINE[PAIR_CH][FTM_BitBandType::COMBINE_COMBINE] = SET;
				PeripheralInfo::BitBandAccess()
				   ->COMBINE[PAIR_CH][FTM_BitBandType::COMBINE_DECAP] = RESET;
				PeripheralInfo::BitBandAccess()
				   ->COMBINE[PAIR_CH][FTM_BitBandType::COMBINE_SYNCEN] = SET;		// ͬ����ʹ��

				
				// �������
				PeripheralInfo::BitBandAccess()
				   ->COMBINE[PAIR_CH][FTM_BitBandType::COMBINE_COMP] = SET;			// ����ģʽ

				PeripheralInfo::BaseAccess()->QDCTRL.QUADEN = RESET;
				PeripheralInfo::BaseAccess()->SC.CPWMS = RESET;

				// PWMװ��ʹ��
				PeripheralInfo::BitBandAccess()->PWMLOAD[CHA] = SET;
				PeripheralInfo::BitBandAccess()->PWMLOAD[CHB] = SET;
				PeripheralInfo::BaseAccess()->PWMLOAD.LDOK = SET;

				// ͬ������
				PeripheralInfo::BaseAccess()->SYNC.SYNCHOM = SET;					// OutMask ͬ���ܿ���
				PeripheralInfo::BaseAccess()->SYNCONF.SYNCMODE = SET;				// Enhanced PWM synchronization
				PeripheralInfo::BaseAccess()->SYNCONF.SWWRBUF = SET;				// MOD, CNTIN, CV ͬ��
				PeripheralInfo::BaseAccess()->SYNCONF.SWRSTCNT = SET;				// �������ܼĴ���ͬ��ʹ��
				PeripheralInfo::BaseAccess()->SYNCONF.SWOM = SET;					// OutMask ͬ��
				PeripheralInfo::BaseAccess()->SYNCONF.SWINVC = SET;					// Inverting ͬ��
				PeripheralInfo::BaseAccess()->SYNCONF.INVC = SET;					// Inverting ����PWMͬ��
				PeripheralInfo::BaseAccess()->SYNCONF.CNTINC = SET;					// CNTIN ����PWMͬ��

				// ͬ��װ�ص�����
				PeripheralInfo::BaseAccess()->SYNC.CNTMAX = SET;
				PeripheralInfo::BaseAccess()->SYNC.CNTMIN = SET;

				// ��ʼʱ��ʹ��
				DisableOutput();
			}





			/*
			 * @brief	ʹ����������
			 */
			void EnableDeadTimeInsertion()
			{
				PeripheralInfo::BitBandAccess()->COMBINE[PAIR_CH][FTM_BitBandType::COMBINE_DTEN] = SET;
				PeripheralInfo::BaseAccess()->DEADTIME.DTPS = 2;
				PeripheralInfo::BaseAccess()->DEADTIME.DTVAL = 48;
			}
			void DisableDeadTimeInsertion()
			{
				PeripheralInfo::BitBandAccess()->COMBINE[PAIR_CH][FTM_BitBandType::COMBINE_DTEN] = RESET;
			}




			/*
			 * @brief	����PWM����ʹ�����ֹ
			 */
			void EnableInverting()
			{
				PeripheralInfo::BitBandAccess()->INVCTRL[PAIR_CH] = SET;
				PeripheralInfo::BaseAccess()->SYNC.SWSYNC = SET;
			}
			void DisableInverting()
			{
				PeripheralInfo::BitBandAccess()->INVCTRL[PAIR_CH] = RESET;
				PeripheralInfo::BaseAccess()->SYNC.SWSYNC = SET;
			}




			/*
			 * @brief	������������ʹ��/��ֹ
			 *			�����ú�����������ģʽ����
			 * @retval	�ɹ�����/�ر��������1�����򷵻�0
			 */
			virtual int EnableOutput()
			{
				PeripheralInfo::BaseAccess()->OUTMASK &= ~(UINT8)(_BV(CHA) | _BV(CHB));
				PeripheralInfo::BaseAccess()->SYNC.SWSYNC = SET;
				return 1;
			}
			virtual int DisableOutput()
			{
				PeripheralInfo::BaseAccess()->OUTMASK |= (UINT8)(_BV(CHA) | _BV(CHB));
				PeripheralInfo::BaseAccess()->SYNC.SWSYNC = SET;
				return 1;
			}





			/*
			 * @brief	�������������ռ�ձȣ��ٷֱȣ���ȷ��1%��
			 * @param	duty ռ�ձȣ�ȡֵ��Χ[0,1]���˷���ֻȡ��С�����֣������û���Ҫ����ȡֵ��Χ��
			 * @param	duty_percent ռ�ձȣ�ȡֵ��Χ[0,100]��
			 *				����һ���Ǹ���İ汾��ʵ��ʱ�벻Ҫ�漰��������
			 */
			virtual void SetDutyCycle(float duty)
			{
				INT16 cntin = PeripheralInfo::BaseAccess()->CNTIN;
				INT16 mod = PeripheralInfo::BaseAccess()->MOD;

				UINT16 regval = duty * (mod - cntin + 1) + cntin;
				PeripheralInfo::BaseAccess()->CONTROLS[CHA].CnV = 0;
				PeripheralInfo::BaseAccess()->CONTROLS[CHB].CnV = regval;
				PeripheralInfo::BaseAccess()->SYNC.SWSYNC = SET;
			}
			virtual void SetDutyCyclePercent(const UINT8 duty_percent)
			{
				INT16 cntin = PeripheralInfo::BaseAccess()->CNTIN;
				INT16 mod = PeripheralInfo::BaseAccess()->MOD;
				
				UINT16 regval = duty_percent * (mod - cntin + 1) / 100 + cntin;
				PeripheralInfo::BaseAccess()->CONTROLS[CHA].CnV = 0;
				PeripheralInfo::BaseAccess()->CONTROLS[CHB].CnV = regval;
				PeripheralInfo::BaseAccess()->SYNC.SWSYNC = SET;
			}




			/*
			 * @brief	�����������ȡռ�ձȣ��ٷֱȣ���ȷ��1%��
			 * @retval	ռ�ձȣ�ȡֵ��Χ[0,1]
			 */
			virtual float GetDutyCycle() const
			{
				INT16 cntin = PeripheralInfo::BaseAccess()->CNTIN;
				INT16 mod = PeripheralInfo::BaseAccess()->MOD;
				INT16 cav = PeripheralInfo::BaseAccess()->CONTROLS[CHA].CnV;
				INT16 cbv = PeripheralInfo::BaseAccess()->CONTROLS[CHB].CnV;

				return (float)abs(cav - cbv) / (mod - cntin + 1);
			}
			virtual UINT8 GetDutyCyclePercent() const
			{
				INT16 cntin = PeripheralInfo::BaseAccess()->CNTIN;
				INT16 mod = PeripheralInfo::BaseAccess()->MOD;
				INT16 cav = PeripheralInfo::BaseAccess()->CONTROLS[CHA].CnV;
				INT16 cbv = PeripheralInfo::BaseAccess()->CONTROLS[CHB].CnV;

				return abs(cav - cbv) * 100 / (mod - cntin + 1);
			}




			/*
			 * @brief	PWM��������
			 * @param	pol ���ԣ�������PWMPOLö�����κ�һ��
			 *				@arg ActiveLow �ɹ��Ƚ�ʱ�͵�ƽ(LowTruePulse)
			 *				@arg ActiveHigh �ɹ��Ƚ�ʱ�ߵ�ƽ(HighTruePulse)
			 */
			virtual void SetPolarity(const PulseWidthModulator::PWMPOL pol)
			{
				STATIC_ASSERT(	PulseWidthModulator::ActiveLow == 0 
							&&  PulseWidthModulator::ActiveHigh == 1,
					enumerate_value_is_not_campacted_with_this_configuration);
				PeripheralInfo::BitBandAccess()->POL[CHA] = !pol;
				PeripheralInfo::BitBandAccess()->POL[CHB] = !pol;
				PeripheralInfo::BaseAccess()->SYNC.SWSYNC = SET;
			}



		private:


			using PWM<CHA>::SetAlignment;


		};




		/*
		 * @brief	FTM ��������������ͨ��
		 */
		class QuadratureDecoder
		{
		public:



			// ������������
			typedef INT16 CounterValueType;



			/*
			 * @brief	 ���������뷽ʽѡ��
			 */
			enum
			{
				PHA_PHB_Encoding,
				CNT_DIR_Encoding
			};




			QuadratureDecoder()
			{
				STATIC_ASSERT(
						PeripheralInfo::BaseAddress == FTM1_BASE
					||	PeripheralInfo::BaseAddress == FTM2_BASE,
					this_FTM_peripheral_does_not_support_quadrature_decode_functions);

				ClockSource::ClockOn();


				// QUAD ģʽҪ��ʹ��FTM
				WriteProtectDisable();
				PeripheralInfo::BaseAccess()->MODE.FTMEN = SET;

				// QUAD �������ģʽ
				PeripheralInfo::BaseAccess()->QDCTRL.QUADMODE = PHA_PHB_Encoding;


				PeripheralInfo::BaseAccess()->CNTIN = 0;
				PeripheralInfo::BaseAccess()->MOD = 0xFFFF;
				PeripheralInfo::BaseAccess()->CNT = 0;


				// QUAD ģʽʹ��
				PeripheralInfo::BaseAccess()->QDCTRL.QUADEN = SET;
			}




			~QuadratureDecoder()
			{
				ClockSource::ClockOff();
			}




			/*
			 * @brief	��ȡ��ǰ�������ֵ
			 */
			INT16 GetCounter() const
			{
				return (INT16)PeripheralInfo::BaseAccess()->CNT;
			}



			/*
			 * @brief	��λ�������������ֵ
			 */
			void ResetCounter()
			{
				PeripheralInfo::BaseAccess()->CNT = 0;
			}




			/*
			 * @brief	��������������ѡ��
			 *			�������ֶ���������������
			 *			�Ա������������ָ��Ϊ��������������ָ��ת����Ϊ��ת��forward��
			 */
			enum QCNTDIR
			{
				ForwardUpCounting,			// ��תʱ���ϼ���
				ForwardDownCounting			// ��ת�����¼���
			};


			/*
			 * @brief	
			 */
			void SetCountingDirection(QCNTDIR dir)
			{
				STATIC_ASSERT(ForwardUpCounting == 0 && ForwardDownCounting == 1,
					enumerate_value_is_not_campacted_with_this_configuration);
				PeripheralInfo::BaseAccess()->QDCTRL.PHAPOL = dir;
				PeripheralInfo::BaseAccess()->QDCTRL.PHBPOL = dir;
			}




			
			enum FLWT
			{
				UnderFlow,
				OverFlow,
			};


			FLWT GetFlowDirection() const
			{
				return FLWT(PeripheralInfo::BaseAccess()->QDCTRL.TOFDIR);
			}




			enum DIR
			{
				CountDown,
				CountUp,
			};

			DIR GetCounterDirection() const
			{
				return DIR(PeripheralInfo::BaseAccess()->QDCTRL.QUADIR);
			}



		private:

			typedef typename PeripheralInfo::QuadratureInputs::PHA QPHA;
			typedef typename PeripheralInfo::QuadratureInputs::PHB QPHB;

			// �����������ź�����
			SignalPin<QPHA> QuadratureInputPhaseA;
			SignalPin<QPHB> QuadratureInputPhaseB;

		};




	protected:

		/*
		 * @brief	����/��ȡ��ʱ����ʱ���ڣ���̬ʵ��
		 */
		static inline void StaticSetTimeOutImpl(const UINT32 time_us)
		{
			// Ϊ�˱�֤FTM��ʹ��ʱ�����������ô���
			// ����PWMģʽ��MOD�Ĵ�������ֵ��Χ[0,7FFF]����
			// FTM��ʱһ�ɰ�����MOD��Χ���ã������Ƕ�ʱ���ȼ��룬�ͼ��������ȼ���

			// ������ʱ������ֱ�ӷ���
			if (time_us > 46604) return;

			// Ĭ�������ʹ��ʼֵΪ0���޸�ģֵ����ֵֹ����ȷ����ʱʱ��
			PeripheralInfo::BaseAccess()->CNTIN = 0;

			// FTM�õ�ʱ��Դ��SystemClock��Ƶ�ʽϸߣ���ʱʱ���϶�
			// ��PSC�ֶδ�����С��ʼ�֣�ֱ���ҵ�һ��PSCֵʹ�ü������������ʱ�����ڶ�ʱʱ��
			UINT8 psc = 1;
			//					����Ϊ0x7FFF������MOD�Ĵ���ֵ[0,7FFF]
			while (time_us > psc * 0x7FFF / ModuleClockFrequencyInMHz && psc < 128)
			{
				psc <<= 1;
			}
			StaticSetPrescalerImpl(psc);
			PeripheralInfo::BaseAccess()->MOD = time_us * PulsePerUS / psc - 1;
		}
		static inline UINT32 StaticGetTimeOutImpl()
		{
			UINT8 psc = StaticGetPrescalerImpl();
			
			// ������ɼ��������tick������ģֵ��ȥ��ʼֵ
			// ��ʹ���з��ŵļ��㣬���ת�����޷��ţ�
			UINT32 maxCount = (UINT32)((int)PeripheralInfo::BaseAccess()->MOD - (int)PeripheralInfo::BaseAccess()->CNTIN);
			return (maxCount + 1) * psc / PulsePerUS;
		}



		/*
		 * @brief	����/��ȡ��ʱ�����Ƶ�ʣ���̬ʵ��
		 */
		static inline void StaticSetFrequencyImpl(const UINT32 freq)
		{
			if (freq < 22) return;

			// Ĭ�������ʹ��ʼֵΪ0���޸�ģֵ����ֵֹ����ȷ����ʱʱ����Ƶ�ʣ�
			PeripheralInfo::BaseAccess()->CNTIN = 0;

			// FTM�õ�ʱ��Դ��SystemClock��Ƶ�ʽϸߣ���ʱʱ���϶�
			// ��PSC�ֶδ����Ӵ�ʼ�֣�ֱ���ҵ�һ��PSCֵʹ�ü����������Ƶ�ʴ��ڶ�ʱƵ��
			UINT8 psc = 128;
			//									����Ϊ0x7FFF������MOD�Ĵ���ֵ[0,7FFF]
			while (freq > ModuleClockFrequencyInHz / 0x7FFF / psc && psc > 0)
			{
				psc >>= 1;
			}

			if (psc == 0)
			{
				psc = 1;
			}
			else
			{
				psc <<= 1;
			}
			StaticSetPrescalerImpl(psc);
			PeripheralInfo::BaseAccess()->MOD = ModuleClockFrequencyInHz / freq / psc - 1;
		}
		static inline UINT32 StaticGetFrequencyImpl()
		{
			UINT8 psc = StaticGetPrescalerImpl();

			// ������ɼ��������tick������ģֵ��ȥ��ʼֵ
			// ��ʹ���з��ŵļ��㣬���ת�����޷��ţ�
			UINT32 maxCount = (UINT32)((int)PeripheralInfo::BaseAccess()->MOD - (int)PeripheralInfo::BaseAccess()->CNTIN);
			return ModuleClockFrequencyInHz / (maxCount + 1) / psc;
		}



		/*
		 * @brief	����/��ȡ��ʱ��Ԥ��Ƶ������̬ʵ��
		 */
		static inline void StaticSetPrescalerImpl(const UINT8 psc_value)
		{
			// regval Ϊд��Ĵ���SC_PS���ֵ������Ԥ��Ƶϵ�������¹�ϵ
			// psc_value = _BV(regval);
			UINT8 regval = 0;
			while (_BV(regval) != psc_value && regval < 7)
			{
				++regval;
			}

			PeripheralInfo::BaseAccess()->SC.PS = regval;
		}
		static inline UINT8 StaticGetPrescalerImpl()
		{
			return _BV(PeripheralInfo::BaseAccess()->SC.PS);
		}



		/*
		 * @brief	��ֹд����
		 */
		static inline void WriteProtectDisable()
		{
			// �����������ΪRM�ֲ�����˵����ؼ��WPDIS�Ƿ�RESET����SET
			if (PeripheralInfo::BaseAccess()->MODE.WPDIS == RESET)
			{
				PeripheralInfo::BaseAccess()->MODE.WPDIS = SET;
			}
		}



	private:



		/*
		 * @brief	������ʱ��ʱ�䵽����ֹͣ��ֻ��ʱһ�Σ�
		 * @notice	FTM�޷�ʵ��ֻ��ʱһ�Σ����ֶ�������¼���ֹͣ�ö�ʱ��
		 */
		virtual void StartOnce()
		{
			// ��ʱ�޷�ʵ�ֶ�ʱһ��
		}

	};
	
}



extern "C"
{
	void FTM0_IRQHandler(void);
	void FTM1_IRQHandler(void);
	void FTM2_IRQHandler(void);
};


#endif  /*_ADX_K60_FTM_H_*/
