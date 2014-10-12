
#ifndef _ADX_K60_PIT_H_
#define _ADX_K60_PIT_H_

#include "adxM3.h"
#include "../../System/System.h"
#include "../PeripheralBase.h"
#include "../PeripheralInfo.h"
#include "../Port/GlobalPort.h"


namespace K60
{


	/*
	 * @brief	�����ж϶�ʱ��
	 *			ֻ�����¼������ﵽ0���Զ���װ��
	 */
	template< UINT32 BASE >
	class Peripheral< PIT_INFO<BASE> >
	{
	public:

		typedef PIT_INFO<BASE> PeripheralInfo;
		typedef ClockGateController<typename PeripheralInfo::ClockSourceGate> ClockSource;



		Peripheral()
		{
			ClockSource::ClockOn();
			EnablePeripheral();
		}

		~Peripheral()
		{
			ClockSource::ClockOff();
		}


		

		/*
		 * @brief	PIT������ʱ��ʹ��/��ֹ
		 */
		void EnablePeripheral()
		{
			PeripheralInfo::BitBandAccess()->MCR[PIT_BitBandType::MCR_MDIS] = RESET;
		}
		void DisablePeripheral()
		{
			PeripheralInfo::BitBandAccess()->MCR[PIT_BitBandType::MCR_MDIS] = SET;
		}



		/*
		 * @brief	PIT��ʱͨ����ģ��
		 * @tparam	CHANNEL_INDEX ��ʱͨ���ţ���Χ[0,3]
		 */
		template< UINT32 CHANNEL_INDEX >
		class CH
			: public adxM3::Timer
		{
		public:


			CH(const UINT32 time_us = 0)
				: autoReloadValue(0), currentValue(0), paused(false)
			{
				STATIC_ASSERT(CHANNEL_INDEX < PeripheralInfo::TimerChannel_MAX,
					PIT_does_not_have_so_much_timer_channels);

				ClockSource::ClockOn();

				// ע��ȫ��ʵ��ָ��
				PeripheralInfo::InstancePtr[CHANNEL_INDEX] = this;

				// ���ö�ʱʱ��
				SetTimeOut(time_us);

				// ����NVIC
				CM4::$NVIC::SetPriority(PeripheralInfo::ChannelIQR[CHANNEL_INDEX],
					PeripheralInfo::InteruptPriority[CHANNEL_INDEX]);
				CM4::$NVIC::EnableIRQ(PeripheralInfo::ChannelIQR[CHANNEL_INDEX]);

				// ������ʱ��
				PeripheralInfo::BitBandAccess()->MCR[PIT_BitBandType::MCR_MDIS] = RESET;
			}

			virtual ~CH()
			{
				ClockSource::ClockOff();
			}



			/*
			 * @brief	������ʱ
			 */
			virtual void Start()
			{
				PeripheralInfo::BitBandAccess()
					->CHANNEL[CHANNEL_INDEX].TCTRL[PIT_BitBandType::TCTRL_TEN] = SET;
			}



			/*
			 * @brief	������ʱ��ʱ�䵽����ֹͣ��ֻ��ʱһ�Σ�
			 */
			virtual void StartOnce()
			{
				// ������ʱ��
				PeripheralInfo::BitBandAccess()
					->CHANNEL[CHANNEL_INDEX].TCTRL[PIT_BitBandType::TCTRL_TEN] = SET;
				
				// ����������Զ���װ��ֵ��ʵ��һ�ζ�ʱ
				PeripheralInfo::BaseAccess()->CHANNEL[CHANNEL_INDEX].LDVAL = 0;
			}



			/*
			 * @brief	ֹͣ��ʱ����ʱ�����㣩
			 */
			virtual void Stop()
			{
				PeripheralInfo::BitBandAccess()
					->CHANNEL[CHANNEL_INDEX].TCTRL[PIT_BitBandType::TCTRL_TEN] = RESET;
			}



			/*
			 * @brief	��ͣ��ʱ����ʱֵ������
			 *			�������Continue�������ʹ��
			 */
			virtual void Pause()
			{
				paused = true;

				// �ȱ��浱ǰ��ʱ������ֵ
				currentValue = PeripheralInfo::BaseAccess()->CHANNEL[CHANNEL_INDEX].CVAL;

				// ֹͣ��ʱ��
				PeripheralInfo::BitBandAccess()
					->CHANNEL[CHANNEL_INDEX].TCTRL[PIT_BitBandType::TCTRL_TEN] = RESET;
			}



			/*
			 * @brief	������ʱ����Pause���ʹ��
			 */
			virtual void Continue()
			{
				// ���ҽ���paused == trueʱ�÷�����Ч
				// ������Ϊ�˲�Ҫ��ֹͣ��ʱ�������һ������ȷ���ڵ��ж�
				if (!paused) return;

				// ���ڶ�ʱ��ÿ���������Զ����´�autoReloadValue��ʼ����
				// Ϊ�˼�����ǰ�ļ�ʱ�����趨��ʱֵΪ��ǰ�ļ���ֵ
				PeripheralInfo::BaseAccess()->CHANNEL[CHANNEL_INDEX].LDVAL = currentValue;

				// ������ʱ��
				PeripheralInfo::BitBandAccess()
					->CHANNEL[CHANNEL_INDEX].TCTRL[PIT_BitBandType::TCTRL_TEN] = SET;

				// �ٻָ�������autoReloadValue����һ���ڵ���������ʱ
				PeripheralInfo::BaseAccess()->CHANNEL[CHANNEL_INDEX].LDVAL = autoReloadValue;

				paused = false;
			}




			enum
			{
				PulsePerUS = PeripheralInfo::ModuleClock::Frequency / 1000000
			};



			/*
			 * @brief	��ȡ��ǰ��ʱ����ʱʱ��ֵ
			 */
			virtual UINT32 GetTime() const
			{
				return PeripheralInfo::BaseAccess()
					->CHANNEL[CHANNEL_INDEX].CVAL / PulsePerUS;
			}



			/*
			 * @brief	���ü�ʱ����ʱʱ��
			 *			��λ us
			 * @notice	��ʱ���Ȳ�Ҫ���� 29,000,000 us
			 */
			virtual void SetTimeOut(UINT32 time_us)
			{
				autoReloadValue = time_us * PulsePerUS;
				PeripheralInfo::BaseAccess()->CHANNEL[CHANNEL_INDEX].LDVAL = autoReloadValue;
			}



			/*
			 * @brief	���ö�ʱ����ʱƵ�ʣ�
			 * @notice	����ͬSetTimeOut�������õ���TimeOut�ĵ�������Ƶ��
			 */
			virtual void SetFrequency(UINT32 freq)
			{
				autoReloadValue = PulsePerUS / freq;
				PeripheralInfo::BaseAccess()->CHANNEL[CHANNEL_INDEX].LDVAL = autoReloadValue;
			}



			/*
			 * @brief	��ȡ��ʱ����ʱʱ��
			 */
			virtual UINT32 GetTimeOut() const
			{
				return autoReloadValue / PulsePerUS;
			}



			/*
			 * @brief	��ȡ��ʱ����ʱƵ��
			 */
			virtual UINT32 GetFrequency() const
			{
				return PulsePerUS / autoReloadValue;
			}




			/*
			 * @brief	�ж϶�ʱ���Ƿ����ڼ�ʱ
			 * @retval	���ڼ�ʱ����true�����򷵻�false
			 */
			virtual bool TimerIsRunning() const
			{
				return PeripheralInfo::BitBandAccess()->CHANNEL[CHANNEL_INDEX].TCTRL[PIT_BitBandType::TCTRL_TEN];
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
					PeripheralInfo::BitBandAccess()
						->CHANNEL[CHANNEL_INDEX].TCTRL[PIT_BitBandType::TCTRL_TIE] = SET;
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
					PeripheralInfo::BitBandAccess()
						->CHANNEL[CHANNEL_INDEX].TCTRL[PIT_BitBandType::TCTRL_TIE] = RESET;
					return 1;

				default:
					return 0;
				}
			}



			/*
			 * @brief	����¼��������������¼������ĳ���¼�
			 * @param	l ������������Ҫʵ�ָü�������Ӧ�ӿ�
			 */
			void SetEventListener(const TimerUpdateListener<Timer&>* l)
			{
				PeripheralInfo::BitBandAccess()
					->CHANNEL[CHANNEL_INDEX].TCTRL[PIT_BitBandType::TCTRL_TIE] = SET;
				updateListenerWrapper.SetEventListener(l);
			}
			



			/*
			 * @brief	�����Զ���װ�������е�ֵ
			 *			���ڵ��μ�ʱʱ�������¼����жϣ����ָ�����ʱֵ
			 *			�Ա��´��ٵ���Start����ʱ�������µ���TimeOut����;
			 */
			void UpdateAutoReloadValue()
			{
				PeripheralInfo::BaseAccess()->CHANNEL[CHANNEL_INDEX].LDVAL = autoReloadValue;
			}


		private:

			UINT32 autoReloadValue;
			UINT32 currentValue;
			bool paused;

		};


	};
	
	
}


extern "C"
{
	void PIT0_IRQHandler(void);
	void PIT1_IRQHandler(void);
	void PIT2_IRQHandler(void);
	void PIT3_IRQHandler(void);
};

#endif  /*_ADX_K60_PIT_H_*/
