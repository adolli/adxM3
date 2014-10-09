
#ifndef _ADX_K60_DMA_H_
#define _ADX_K60_DMA_H_

#include "adxM3.h"
#include "../../System/System.h"
#include "../PeripheralBase.h"
#include "../PeripheralInfo.h"
#include <cstring>


namespace K60
{


	/*
	 * @breif	ֱ���ڴ���ʿ���������
	 *			���ʣ�
	 *				����ͨ��/����ͨ����DMA��ĳ��ͨ��û�����ô�������
	 *			����ģ�����£�			
	 *				���蹹��ʱ��ȫ��ͨ������������ͨ����ֱ���ҵ�һ������ͨ��������û�
	 *				���û�����TransferRequest����ʱ������DMAʱ�ӣ������δ��������ֱ��DMA
	 *				�յ����������ź�ʱ�������ڴ浽�ڴ濽�����Զ���ʼ���䣬����Ĵ�����
	 *				�ڴ�֮��Ĵ������ȴ����������źţ��ڴ��ڼ�CPU����������£����Զ�
	 *				��ʼ���䣬�ڴ�����ɺ󷢳��¼�֪ͨ������û������˴�������¼�����
	 *				����Ϊ���¼�ע���¼���Ӧ���������û��¼���Ӧ��ɺ󣬸�����ģ�ͻ��Զ�
	 *				�ͷ�ͨ�������Զ��ر�DMAʱ�ӣ������ǰ�Ѿ�û���κ�һ�����ڴ����ͨ������
	 *
	 *
	 * @impl	�����ڲ�ʵ�����£�
	 *				1. DMA����������ʱѭ����������ͨ����ֱ���ҵ�һ�����е�ͨ�����������ݳ�Ա
	 *				CurrentChannel�м�¼��ͨ���ţ�ͨ����Ӧʹ��TCD[CurrentChannel]����DMAMUX
	 *				�е�CHCFG[CurrentChannel]��
	 *				2. �û��ɼ�����������¼���Ϊ��ע����Ӧ����������������¼�֪ͨ�����Ӧͨ������λ
	 *				ChannelTCEventMask�ᱻRESET��
	 *				3. �û�����TransferRequest�����Դ����������ʱ����������õ�Դ��Ŀ�Ĳ����Զ�
	 *				���ú�TCD�����úú��Զ�ʹ��ͨ������������ж��Ǳ�ǿ�ƿ����ģ��ڴ�������ж��У�
	 *				����û��������¼�֪ͨ���ص��û�ע�����Ӧ�������û�ע�����Ӧ������������
	 *				������ע������������λ��ͨ������λ����Ϊһ������ֻ��Ӧһ���¼�����ʱ�������
	 *				�ɱ������������Ĺر�����ʱ�ӻ��Ƴٵ�������ɵ��ж��У���������ʱ�ӹ���ϵͳ��
	 *				���Ĺ��ܣ����ʱ�ӿ������м�������ȫ������ʱ�ӿ����Ķ����ͷ�ʱ�Ӻ��������
	 *				��ʱ�ӣ���
	 *
	 * @typical	���͵��δ����ʱ��87ns
	 *			���ô��������ʱ��4us
	 *
	 */
	template< UINT32 BASE >
	class Peripheral< DMA_INFO<BASE> >
		: adxM3::EventObject
	{
	public:


		typedef DMA_INFO<BASE> PeripheralInfo;
		typedef ClockGateController<typename PeripheralInfo::ClockSourceGate> ClockSource;
		typedef ClockGateController<typename PeripheralInfo::MultiplexerClockSourceGate> MultiplexerClockSource;
		

		Peripheral()
			: HasnotSubmitTransferRequest(true)
		{
			bool success = false;
			while (!success)
			{
				for (INT8 i = 1; i < PeripheralInfo::CHANNEL_MAX; ++i)
				{
					if (PeripheralInfo::ChannelIdleFlagStatus & _BV(i))
					{
						PeripheralInfo::ChannelIdleFlagStatus &= ~_BV(i);
						CurrentChannel = i;
						
						// NVIC
						CM4::$NVIC::SetPriority(
							PeripheralInfo::TransferCompleteIRQ[i], 
							PeripheralInfo::DefaultIRQPriority);
						CM4::$NVIC::EnableIRQ(PeripheralInfo::TransferCompleteIRQ[i]);
						
						success = true;
						break;
					}
				}
				if (success)
				{
					break;
				}
			}

			// ��������ʱ��
			MultiplexerClockSource::ClockOn();
			ClockSource::ClockOn();
		}


		virtual ~Peripheral()
		{
			// ����û���С�Ĺ�����һ��ʵ����δ����TransferRequest����
			// �ڴ�Ӧ�ر�ʱ�ӣ���Ϊ������뵽�жϷ�������
			if (HasnotSubmitTransferRequest)
			{
				MultiplexerClockSource::ClockOff();
				ClockSource::ClockOff();
				PeripheralInfo::ChannelIdleFlagStatus |= _BV(CurrentChannel);
			}
		}




		/*
		 * @brief	��ȡ��ǰDMA������ռ�õ�ͨ����
		 */
		INT8 GetAllocatedChannelIndex() const
		{
			return CurrentChannel;
		}




		enum STARTM
		{
			StartNotImmediately,
			StartImmediately
		};




		/*
		 * @brief	DMAͨ���������ã���Ҫ����ģ�������
		 * @tparam	DstPtrType Ŀ��ָ������
		 *			SrcPtrType Դָ������
		 *				������������T* constʱ�������ַ���Զ�����
		 *				������������T* ʱ�������ַ�Զ�����
		 *				ÿ�ε����ֽ�����sizeof(T)����
		 * @param	dst Ŀ���ַָ��
		 *			src Դ��ַָ��
		 *			buf_size ������������Դ��Ŀ�ĵ����;��������������ֽ����ȣ�
		 *			isCircular ѭ������ģʽ
		 *				@arg true Ϊѭ������ģʽ�����Ȳ��ԣ���
		 */
		template< typename DstPtrType, typename SrcPtrType >
		void TransferRequest(void* dst, 
							 const void* src, 
							 UINT16 buf_zise,
							 DMARequestSource trgSource = DMAMUX_Always0, 
							 bool isCircular = false,
							 STARTM startWhen = StartImmediately)
		{
			typedef typename adxM3::PointerTraits<DstPtrType>::value_type DstValueType;
			typedef typename adxM3::PointerTraits<SrcPtrType>::value_type SrcValueType;

			STATIC_ASSERT(
				   sizeof(DstValueType) <= 4 
				&& sizeof(SrcValueType) <= 4,
				this_driver_of_dma_only_supports_less_than_4_bytes_per_transfer);


			
			// ��ʹ��MinorLoopӳ��
			PeripheralInfo::BaseAccess()->CR.EMLM = RESET;


			// ����������ROM�������Ҵ���ԴΪ������Դ������һ�����������Ƶ�DMAר�ô��仺����
			if (	trgSource < DMAMUX_Always0 
				&&	!(	(UINT32)src > MemoryMapping::ROM_BASE 
					&&	(UINT32)src < MemoryMapping::ROM_BASE + MemoryMapping::ROM_SIZE)
				&&	!adxM3::PointerTraits<SrcPtrType>::IsConstPointer)
			{
				// �������ڴ�֮��Ĵ����Ƚ����ݸ��Ƶ�������
				PeripheralInfo::TransferSourceBuffer[CurrentChannel] = new SrcValueType[buf_zise];
				std::memcpy(PeripheralInfo::TransferSourceBuffer[CurrentChannel],
					src, buf_zise * sizeof(SrcValueType));

				// ����Դ��ַ��Ŀ�ĵ�ַ
				PeripheralInfo::BaseAccess()->TCD[CurrentChannel].SADDR 
					= (UINT32)PeripheralInfo::TransferSourceBuffer[CurrentChannel];
			}
			else
			{
				// �ڴ�֮�䴫�䣬����Ҫʹ��ר�û�����
				PeripheralInfo::BaseAccess()->TCD[CurrentChannel].SADDR = (UINT32)src;
			}
			
			PeripheralInfo::BaseAccess()->TCD[CurrentChannel].DADDR = (UINT32)dst;

			// ����Դ��ַ��Ŀ�ĵ�ַÿ�δ���ƫ����������������
			if (adxM3::PointerTraits<DstPtrType>::IsConstPointer)
			{
				PeripheralInfo::BaseAccess()->TCD[CurrentChannel].DOFF = 0;
			}
			else
			{
				PeripheralInfo::BaseAccess()->TCD[CurrentChannel].DOFF = sizeof(DstValueType);
			}
			if (adxM3::PointerTraits<SrcPtrType>::IsConstPointer)
			{
				PeripheralInfo::BaseAccess()->TCD[CurrentChannel].SOFF = 0;
			}
			else
			{
				PeripheralInfo::BaseAccess()->TCD[CurrentChannel].SOFF = sizeof(SrcValueType);
			}

			// ���ô����ַģ��
			PeripheralInfo::BaseAccess()->TCD[CurrentChannel].ATTR.DMOD = 0;
			PeripheralInfo::BaseAccess()->TCD[CurrentChannel].ATTR.SMOD = 0;
			
			// ����Ŀ�ĵ�ַÿ�δ�����
			// ��bytes ���뵽log(2, bytes)������
			//							bytes:	0  1  2  3  4
			static const UINT8 SizeEncode[] = { 0, 0, 1, 0, 2 };
			PeripheralInfo::BaseAccess()->TCD[CurrentChannel].ATTR.DSIZE = SizeEncode[sizeof(DstValueType)];
			PeripheralInfo::BaseAccess()->TCD[CurrentChannel].ATTR.SSIZE = SizeEncode[sizeof(SrcValueType)];
			

			// ͨ�����δ����ֽ�������Դ�������;�����
			PeripheralInfo::BaseAccess()->TCD[CurrentChannel].NBYTES_MLNO = sizeof(SrcValueType);
			

			// ����������buf_size�ĵ�λ��ÿ�δ�����ֽ���������
			PeripheralInfo::BaseAccess()->TCD[CurrentChannel].CITER_ELINKNO.CITER = buf_zise;
			PeripheralInfo::BaseAccess()->TCD[CurrentChannel].BITER_ELINKNO.BITER = buf_zise;


			// ������ɺ�ָ�ԭ��ַ
			PeripheralInfo::BaseAccess()->TCD[CurrentChannel].SLAST = 0;
			if (adxM3::PointerTraits<DstPtrType>::IsConstPointer)
			{
				// �˻س�ʼ��ַ
				PeripheralInfo::BaseAccess()->TCD[CurrentChannel].DLAST_SGA = -(buf_zise * sizeof(DstValueType));
			}
			else
			{
				PeripheralInfo::BaseAccess()->TCD[CurrentChannel].DLAST_SGA = 0;
			}

			// DMA�����Ъ���ã�������ƣ�
			PeripheralInfo::BaseAccess()->TCD[CurrentChannel].CSR.BWC = 3;

			// ������ѭ��������
			PeripheralInfo::BaseAccess()->TCD[CurrentChannel].CSR.MAJORELINK = RESET;		

			// ѭ��ģʽ���ã����Ƿ�����ɺ��Զ���������־λ��
			PeripheralInfo::BaseAccess()->TCD[CurrentChannel].CSR.DREQ = !isCircular;


			// DMA����Դ����
			PeripheralInfo::MultiplexerAccess()->CHCFG[CurrentChannel] = 
				DMAMUX_CHCFG_ENBL_MASK | trgSource;


			// ʹ�ܴ�������ж�
			PeripheralInfo::BaseAccess()->TCD[CurrentChannel].CSR.INTMAJOR = SET;
			PeripheralInfo::BaseAccess()->TCD[CurrentChannel].CSR.INTHALF = RESET;

			if (startWhen == StartImmediately)
			{
				HasnotSubmitTransferRequest = false;

				// ͨ������ʹ�ܣ��ȴ������ź�
				// memory to memory transfer��ֱ�ӿ�ʼ���������йصĴ�������ȴ����������ź�
				PeripheralInfo::BitBandAccess()->ERQ[CurrentChannel] = SET;
			}
		}



		/*
		 * @brief	��������
		 */
		void StartTransfer()
		{
			HasnotSubmitTransferRequest = false;
			PeripheralInfo::BitBandAccess()->ERQ[CurrentChannel] = SET;
		}




		/*
		 * @brief	���DMAͨ���������
		 * @retval	��������򷵻�true�����򷵻�false
		 */
		bool TransferComplete()
		{
			return PeripheralInfo::ChannelIdleFlagStatus & _BV(CurrentChannel);
		}



		/*
		 * @brief	DMA�¼��б�
		 */
		enum EVT
		{
			TRANSFER_COMPLETE
		};



		/*
		 * @brief	�¼�֪ͨ���ؿ���
		 * @param	evt ��Ҫ������ر�֪ͨ���¼�����
		 * @retval	�ɹ�ʹ��/��ֹĳ���¼�֪ͨ�򷵻�1�����򷵻�0
		 */
		virtual int EnableEventNotify(EventType evt)
		{
			switch (evt)
			{
			case TRANSFER_COMPLETE :
				PeripheralInfo::ChannelTCEventMask &= ~_BV(CurrentChannel);
				return 1;

			default:
				return 0;
			}
		}
		virtual int DisableEventNotify(EventType evt)
		{
			switch (evt)
			{
			case TRANSFER_COMPLETE :
				PeripheralInfo::ChannelTCEventMask |= _BV(CurrentChannel);
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
			case TRANSFER_COMPLETE :
				PeripheralInfo::ChannelTCEventMask &= ~_BV(CurrentChannel);
				PeripheralInfo::transferCompleteCBK[CurrentChannel] = FnPtr;
				return 1;

			default:
				return 0;
			}
		}




		/*
		 * @brief	ͨ�������������ã����ô˷����뱣֤ʱ�Ӵ��ڿ���״̬��
		 * @param	from �����ͨ��
		 *			linkedTo ���ӵ����ͨ��
		 * @retval	������ӳɹ��򷵻�1�����򷵻�0
		 */
		static int ChannelChainingConfig(INT8 from, INT8 linkedTo)
		{
			PeripheralInfo::BaseAccess()->TCD[from].CSR.MAJORLINKCH = linkedTo;
			PeripheralInfo::BaseAccess()->TCD[from].CSR.MAJORELINK = SET;
			return PeripheralInfo::BaseAccess()->TCD[from].CSR.MAJORELINK;
		}



	private:
		
		
		// ����󱣴浱ǰ���䵽��ͨ����
		// DMAͨ�������������ڿɻ��Ψһһ���ɶ�ռ��ͨ����
		INT8 CurrentChannel;


		bool HasnotSubmitTransferRequest;

	};



}



extern "C"
{
	void DMA0_IRQHandler(void);
	void DMA1_IRQHandler(void);
	void DMA2_IRQHandler(void);
	void DMA3_IRQHandler(void);
	void DMA4_IRQHandler(void);
	void DMA5_IRQHandler(void);
	void DMA6_IRQHandler(void);
	void DMA7_IRQHandler(void);
	void DMA8_IRQHandler(void);
	void DMA9_IRQHandler(void);
	void DMA10_IRQHandler(void); 
	void DMA11_IRQHandler(void); 
	void DMA12_IRQHandler(void); 
	void DMA13_IRQHandler(void); 
	void DMA14_IRQHandler(void); 
	void DMA15_IRQHandler(void); 
	void DMA_Error_IRQHandler(void);
};

#endif  /*_ADX_K60_DMA_H_*/
