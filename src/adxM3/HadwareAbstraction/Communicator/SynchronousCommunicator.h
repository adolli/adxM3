
#ifndef _SYNCHRONOUSCOMMUNICATOR_H_
#define _SYNCHRONOUSCOMMUNICATOR_H_


#include "Communicator.h"


namespace adxM3
{


	/*
	 * @brief	同步通讯器接口类，继承于基本通讯器
	 *			该接口只提供基本收发方法，时钟相位和极性需配置请使用
	 *			ClockPolarityAndPhaseProgrammableSynchronousCommunicator
	 */
	class SynchronousCommunicator : public ConfigurableCommunicator
	{
	public:


		/*
		 * @brief	同步通讯器收发接口方法
		 * @param	out 待发送数据的地址
		 *			in 接收的数据存放区的地址
		 *			_TxBuf 待发送数据缓冲区
		 *			_RxBuf 接收缓冲区（由使用者自行分配）
		 *			bufferSize 多字节收发的发送缓冲区大小
		 *			发送帧支持字节、半字、字 类型数据，由通讯外设根据实际情况实现
		 *			收发前请务必配置好数据位
		 * @notice	在派生类同步通讯器中如果没收到数据可返回0
		 *			多字节收发方法中单次发送一个字节还是一个半字由派生类外设根据当前配置决定
		 *			建议派生类override该方法
		 * @retval	返回接收到的数据/缓冲区
		 */
		virtual UINT8 SynchronouslyTranceive(const UINT8 out) = 0;
		virtual UINT16 SynchronouslyTranceive(const UINT16 out)
		{
			UINT16 ret = 0;
			SynchronouslyTranceive(&out, &ret, sizeof(out));
			return ret;
		}
		virtual UINT32 SynchronouslyTranceive(const UINT32 out)
		{
			UINT32 ret = 0;
			SynchronouslyTranceive(&out, &ret, sizeof(out));
			return ret;
		}
		virtual void* SynchronouslyTranceive(const void* _TxBuf, void* _RxBuf, size_type bufferSize)
		{
			const UINT8* TxBuf = static_cast<const UINT8*>(_TxBuf);
			UINT8* RxBuf = static_cast<UINT8*>(_RxBuf);
			while (bufferSize--)
			{
				*RxBuf++ = SynchronouslyTranceive(*TxBuf++);
			}
			while (!SendComplete());
			return _RxBuf;
		}



		/*
		 * @brief	时钟极性选项
		 */
		enum CPOL
		{
			CPOL_LOW,		// 时钟信号常态低
			CPOL_HIGH		// 时钟信号常态高
		};



		/*
		 * @brief	同步通讯器配置时钟极性
		 * @param	polarity 时钟极性枚举
		 *				@arg CPOL_LOW
		 *				@arg CPOL_HIGH
		 * @retval	设置成功则返回1，否则返回0
		 */
		virtual int SetClockPolarity(const CPOL polarity) = 0;



		/*
		 * @brief	同步通讯器获取时钟极性
		 * @retval	polarity 时钟极性枚举
		 *			若无法获取默认返回CPOL_LOW
		 *			无法获取时钟极性时不建议继承于此类
		 */
		virtual CPOL GetClockPolarity() const = 0;



		/*
		 * @brief	时钟相位选项，选择时钟信号的触发边沿
		 */
		enum CPHA
		{
			CPHA_1st_EDGE,
			CPHA_2nd_EDGE
		};



		/*
		 * @brief	同步通讯器配置时钟相位
		 * @param	phase 时钟相位枚举
		 *				@arg CPHA_1st_EDGE
		 *				@arg CPHA_2nd_EDGE
		 * @retval	设置成功则返回1，否则返回0
		 */
		virtual int SetClockPhase(const CPHA phase) = 0;



		/*
		 * @brief	同步通讯器获取时钟相位
		 * @retval	phase 时钟相位枚举
		 *			若无法获取默认返回CPHA_1st_EDGE
		 *			无法获取时钟相位时不建议继承于此类
		 */
		virtual CPHA GetClockPhase() const = 0;





	};



}


#endif  /*_SYNCHRONOUSCOMMUNICATOR_H_*/
