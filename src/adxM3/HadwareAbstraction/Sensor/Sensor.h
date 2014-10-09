
#ifndef _SENSOR_H_
#define _SENSOR_H_

#include "../../common/defs.h"
#include "../Event/EventObject.h"

namespace adxM3
{

	class Sensor
	{
	public:


		/*
		 * @brief	������ͨ�����壬��0��ʼ
		 */
		enum CHANNEL
		{
			CH0,
			CH1,
			CH2,
			CH3,
			CH4,
			CH5,
			CH6,
			CH7,
			CH8,
			CH9,
			CH10,
			CH11,
			CH12,
			CH13,
			CH14,
			CH15,
			CH16,
			CH17,
			CH18,
			CH19,
			CH20,
			CH21,
			CH22,
			CH23,
			CH24,
			CH25,
			CH26,
			CH27,
			CH28,
			CH29,
			CH30,
			CH31
		};






		/*
		 * @brief	����������ͨ�����ؿ���
		 * @param	channel ����ͨ��
		 *			sta ʹ��״̬
		 *				@arg ENABLE
		 *				@arg DISABLE
		 * @retval	�ɹ�������رշ���1�����򷵻�0
		 * @ovride	����������Ʋ���ͨ���Ŀ�����رգ�������1��Ӧ������������
		 */
		virtual int ChannelCommand(const int channel, FunctionalState sta) = 0;



		/*
		 * @brief	����������
		 * @param	result ������������λ��
		 * @retval	���β����ɹ��򷵻�1�����򷵻�0
		 * @ovride	������ֻ��ʵ�ֲ�������
		 */
		virtual int Sampling(int& result) = 0;




		/*
		 * @brief	���������þ���
		 * @param	accbits ����λ��
		 * @retval	�ɹ�����1�����򷵻�0
		 */
		virtual int SetAccuracy(const BYTE accbits) = 0;



		/*
		 * @brief	����������ת�����
		 * @retval	��ǰת������ɷ���true�����򷵻�false
		 */
		virtual bool ConvertComplete() const = 0;




		/*
		 * @brief	����ʱ��ѡ��
		 */
		enum SMPT
		{
			VeryShort,					// �ǳ���
			Short,						// ��
			Normal,						// ��ͨ
			Long,						// ��
			VeryLong,					// �ܳ�
			ExtremelyLong,				// �ǳ���
			MosquitoesAreAsleep,		// ���Ӷ�˯����
		};



		/*
		 * @brief	����������ʱ������
		 * @param	sample_time ����ʱ�䣬������SMPTö���е��κ�һ��
		 * @retval	�ɹ����÷���1�����򷵻�0
		 */
		virtual int SetSampleTime(const SMPT sample_time) = 0;



	protected:

		virtual ~Sensor() {}
	};


}

#endif  /*_SENSOR_H_*/
