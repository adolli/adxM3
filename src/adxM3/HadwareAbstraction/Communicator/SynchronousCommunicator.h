
#ifndef _SYNCHRONOUSCOMMUNICATOR_H_
#define _SYNCHRONOUSCOMMUNICATOR_H_


#include "Communicator.h"


namespace adxM3
{


	/*
	 * @brief	ͬ��ͨѶ���ӿ��࣬�̳��ڻ���ͨѶ��
	 *			�ýӿ�ֻ�ṩ�����շ�������ʱ����λ�ͼ�����������ʹ��
	 *			ClockPolarityAndPhaseProgrammableSynchronousCommunicator
	 */
	class SynchronousCommunicator : public ConfigurableCommunicator
	{
	public:


		/*
		 * @brief	ͬ��ͨѶ���շ��ӿڷ���
		 * @param	out ���������ݵĵ�ַ
		 *			in ���յ����ݴ�����ĵ�ַ
		 *			_TxBuf ���������ݻ�����
		 *			_RxBuf ���ջ���������ʹ�������з��䣩
		 *			bufferSize ���ֽ��շ��ķ��ͻ�������С
		 *			����֧֡���ֽڡ����֡��� �������ݣ���ͨѶ�������ʵ�����ʵ��
		 *			�շ�ǰ��������ú�����λ
		 * @notice	��������ͬ��ͨѶ�������û�յ����ݿɷ���0
		 *			���ֽ��շ������е��η���һ���ֽڻ���һ��������������������ݵ�ǰ���þ���
		 *			����������override�÷���
		 * @retval	���ؽ��յ�������/������
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
		 * @brief	ʱ�Ӽ���ѡ��
		 */
		enum CPOL
		{
			CPOL_LOW,		// ʱ���źų�̬��
			CPOL_HIGH		// ʱ���źų�̬��
		};



		/*
		 * @brief	ͬ��ͨѶ������ʱ�Ӽ���
		 * @param	polarity ʱ�Ӽ���ö��
		 *				@arg CPOL_LOW
		 *				@arg CPOL_HIGH
		 * @retval	���óɹ��򷵻�1�����򷵻�0
		 */
		virtual int SetClockPolarity(const CPOL polarity) = 0;



		/*
		 * @brief	ͬ��ͨѶ����ȡʱ�Ӽ���
		 * @retval	polarity ʱ�Ӽ���ö��
		 *			���޷���ȡĬ�Ϸ���CPOL_LOW
		 *			�޷���ȡʱ�Ӽ���ʱ������̳��ڴ���
		 */
		virtual CPOL GetClockPolarity() const = 0;



		/*
		 * @brief	ʱ����λѡ�ѡ��ʱ���źŵĴ�������
		 */
		enum CPHA
		{
			CPHA_1st_EDGE,
			CPHA_2nd_EDGE
		};



		/*
		 * @brief	ͬ��ͨѶ������ʱ����λ
		 * @param	phase ʱ����λö��
		 *				@arg CPHA_1st_EDGE
		 *				@arg CPHA_2nd_EDGE
		 * @retval	���óɹ��򷵻�1�����򷵻�0
		 */
		virtual int SetClockPhase(const CPHA phase) = 0;



		/*
		 * @brief	ͬ��ͨѶ����ȡʱ����λ
		 * @retval	phase ʱ����λö��
		 *			���޷���ȡĬ�Ϸ���CPHA_1st_EDGE
		 *			�޷���ȡʱ����λʱ������̳��ڴ���
		 */
		virtual CPHA GetClockPhase() const = 0;





	};



}


#endif  /*_SYNCHRONOUSCOMMUNICATOR_H_*/
