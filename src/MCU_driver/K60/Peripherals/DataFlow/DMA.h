
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
	 * @breif	直接内存访问控制器外设
	 *			名词：
	 *				可用通道/空闲通道：DMA中某个通道没有配置传输任务
	 *			外设模型如下：			
	 *				外设构造时在全部通道中搜索可用通道，直到找到一个空闲通道分配给用户
	 *				当用户调用TransferRequest方法时才启动DMA时钟（如果尚未开启），直到DMA
	 *				收到传输请求信号时（对于内存到内存拷贝则自动开始传输，外设寄存器与
	 *				内存之间的传输则会等待外设请求信号，在此期间CPU可以做别的事），自动
	 *				开始传输，在传输完成后发出事件通知，如果用户监听了传输完成事件，则
	 *				可以为此事件注册事件响应方法。当用户事件响应完成后，该外设模型会自动
	 *				释放通道并且自动关闭DMA时钟（如果当前已经没有任何一个正在传输的通道）。
	 *
	 *
	 * @impl	外设内部实现如下：
	 *				1. DMA控制器构造时循环搜索可用通道，直到找到一个空闲的通道，并在数据成员
	 *				CurrentChannel中记录下通道号，通道对应使用TCD[CurrentChannel]，和DMAMUX
	 *				中的CHCFG[CurrentChannel]。
	 *				2. 用户可监听传输完成事件并为此注册响应方法，如果开启了事件通知，则对应通道屏蔽位
	 *				ChannelTCEventMask会被RESET。
	 *				3. 用户调用TransferRequest方法对传输进行配置时，会根据配置的源与目的参数自动
	 *				设置好TCD，设置好后自动使能通道。传输完成中断是被强制开启的，在传输完成中断中，
	 *				如果用户开启了事件通知则会回调用户注册的响应方法，用户注册的响应方法被调用完
	 *				后立刻注销，并重新置位该通道屏蔽位，因为一个传输只对应一次事件，此时外设对象
	 *				可被析构。真正的关闭外设时钟会推迟到传输完成的中断中（根据智能时钟管理系统提
	 *				供的功能，会对时钟开启进行计数，当全部申请时钟开启的对象都释放时钟后才真正关
	 *				闭时钟）。
	 *
	 * @typical	典型单次传输耗时：87ns
	 *			配置传输请求耗时：4us
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

			// 开启外设时钟
			MultiplexerClockSource::ClockOn();
			ClockSource::ClockOn();
		}


		virtual ~Peripheral()
		{
			// 如果用户不小心构造了一个实例但未调用TransferRequest方法
			// 在此应关闭时钟，因为不会进入到中断服务里面
			if (HasnotSubmitTransferRequest)
			{
				MultiplexerClockSource::ClockOff();
				ClockSource::ClockOff();
				PeripheralInfo::ChannelIdleFlagStatus |= _BV(CurrentChannel);
			}
		}




		/*
		 * @brief	获取当前DMA控制器占用的通道号
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
		 * @brief	DMA通道传输配置（需要带上模版参数）
		 * @tparam	DstPtrType 目标指针类型
		 *			SrcPtrType 源指针类型
		 *				当以上类型如T* const时，传输地址不自动递增
		 *				当以上类型如T* 时，传输地址自动递增
		 *				每次递增字节数由sizeof(T)决定
		 * @param	dst 目标地址指针
		 *			src 源地址指针
		 *			buf_size 传输量（根据源和目的的类型决定是字数还是字节数等）
		 *			isCircular 循环传输模式
		 *				@arg true 为循环传输模式（请先测试！）
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


			
			// 不使用MinorLoop映射
			PeripheralInfo::BaseAccess()->CR.EMLM = RESET;


			// 条件：不在ROM区域内且触发源为外设且源数据是一个缓冲区则复制到DMA专用传输缓冲区
			if (	trgSource < DMAMUX_Always0 
				&&	!(	(UINT32)src > MemoryMapping::ROM_BASE 
					&&	(UINT32)src < MemoryMapping::ROM_BASE + MemoryMapping::ROM_SIZE)
				&&	!adxM3::PointerTraits<SrcPtrType>::IsConstPointer)
			{
				// 外设与内存之间的传输先将内容复制到缓冲区
				PeripheralInfo::TransferSourceBuffer[CurrentChannel] = new SrcValueType[buf_zise];
				std::memcpy(PeripheralInfo::TransferSourceBuffer[CurrentChannel],
					src, buf_zise * sizeof(SrcValueType));

				// 设置源地址和目的地址
				PeripheralInfo::BaseAccess()->TCD[CurrentChannel].SADDR 
					= (UINT32)PeripheralInfo::TransferSourceBuffer[CurrentChannel];
			}
			else
			{
				// 内存之间传输，不需要使用专用缓冲区
				PeripheralInfo::BaseAccess()->TCD[CurrentChannel].SADDR = (UINT32)src;
			}
			
			PeripheralInfo::BaseAccess()->TCD[CurrentChannel].DADDR = (UINT32)dst;

			// 设置源地址与目的地址每次传输偏移量（传输增量）
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

			// 配置传输地址模数
			PeripheralInfo::BaseAccess()->TCD[CurrentChannel].ATTR.DMOD = 0;
			PeripheralInfo::BaseAccess()->TCD[CurrentChannel].ATTR.SMOD = 0;
			
			// 配置目的地址每次传输宽度
			// 从bytes 编码到log(2, bytes)的数组
			//							bytes:	0  1  2  3  4
			static const UINT8 SizeEncode[] = { 0, 0, 1, 0, 2 };
			PeripheralInfo::BaseAccess()->TCD[CurrentChannel].ATTR.DSIZE = SizeEncode[sizeof(DstValueType)];
			PeripheralInfo::BaseAccess()->TCD[CurrentChannel].ATTR.SSIZE = SizeEncode[sizeof(SrcValueType)];
			

			// 通道单次传输字节数（由源数据类型决定）
			PeripheralInfo::BaseAccess()->TCD[CurrentChannel].NBYTES_MLNO = sizeof(SrcValueType);
			

			// 传输总量（buf_size的单位由每次传输的字节数决定）
			PeripheralInfo::BaseAccess()->TCD[CurrentChannel].CITER_ELINKNO.CITER = buf_zise;
			PeripheralInfo::BaseAccess()->TCD[CurrentChannel].BITER_ELINKNO.BITER = buf_zise;


			// 传输完成后恢复原地址
			PeripheralInfo::BaseAccess()->TCD[CurrentChannel].SLAST = 0;
			if (adxM3::PointerTraits<DstPtrType>::IsConstPointer)
			{
				// 退回初始地址
				PeripheralInfo::BaseAccess()->TCD[CurrentChannel].DLAST_SGA = -(buf_zise * sizeof(DstValueType));
			}
			else
			{
				PeripheralInfo::BaseAccess()->TCD[CurrentChannel].DLAST_SGA = 0;
			}

			// DMA传输间歇配置（带宽控制）
			PeripheralInfo::BaseAccess()->TCD[CurrentChannel].CSR.BWC = 3;

			// 主传输循环不链接
			PeripheralInfo::BaseAccess()->TCD[CurrentChannel].CSR.MAJORELINK = RESET;		

			// 循环模式配置（即是否传输完成后自动清除请求标志位）
			PeripheralInfo::BaseAccess()->TCD[CurrentChannel].CSR.DREQ = !isCircular;


			// DMA触发源配置
			PeripheralInfo::MultiplexerAccess()->CHCFG[CurrentChannel] = 
				DMAMUX_CHCFG_ENBL_MASK | trgSource;


			// 使能传输完成中断
			PeripheralInfo::BaseAccess()->TCD[CurrentChannel].CSR.INTMAJOR = SET;
			PeripheralInfo::BaseAccess()->TCD[CurrentChannel].CSR.INTHALF = RESET;

			if (startWhen == StartImmediately)
			{
				HasnotSubmitTransferRequest = false;

				// 通道传输使能，等待请求信号
				// memory to memory transfer会直接开始，跟外设有关的传输则需等待外设请求信号
				PeripheralInfo::BitBandAccess()->ERQ[CurrentChannel] = SET;
			}
		}



		/*
		 * @brief	启动传输
		 */
		void StartTransfer()
		{
			HasnotSubmitTransferRequest = false;
			PeripheralInfo::BitBandAccess()->ERQ[CurrentChannel] = SET;
		}




		/*
		 * @brief	检测DMA通道传输完成
		 * @retval	传输完成则返回true，否则返回false
		 */
		bool TransferComplete()
		{
			return PeripheralInfo::ChannelIdleFlagStatus & _BV(CurrentChannel);
		}



		/*
		 * @brief	DMA事件列表
		 */
		enum EVT
		{
			TRANSFER_COMPLETE
		};



		/*
		 * @brief	事件通知开关控制
		 * @param	evt 需要开启或关闭通知的事件类型
		 * @retval	成功使能/禁止某个事件通知则返回1，否则返回0
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
		 * @brief	添加事件监听器，监听事件对象的某个事件
		 * @param	evt 对象的事件
		 *			FnPtr 触发该事件时的处理函数，由派生类在实现时转换到所需的类型
		 * @retval	成功注册某个事件监听器则返回1，否则返回0
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
		 * @brief	通道传输链接配置（调用此方法请保证时钟处于开启状态）
		 * @param	from 从这个通道
		 *			linkedTo 链接到这个通道
		 * @retval	如果链接成功则返回1，否则返回0
		 */
		static int ChannelChainingConfig(INT8 from, INT8 linkedTo)
		{
			PeripheralInfo::BaseAccess()->TCD[from].CSR.MAJORLINKCH = linkedTo;
			PeripheralInfo::BaseAccess()->TCD[from].CSR.MAJORELINK = SET;
			return PeripheralInfo::BaseAccess()->TCD[from].CSR.MAJORELINK;
		}



	private:
		
		
		// 构造后保存当前分配到的通道号
		// DMA通道在声明周期内可获得唯一一个可独占的通道号
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
