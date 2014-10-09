
#ifndef _MultiChannelVOLTMETER_H_
#define _MultiChannelVOLTMETER_H_

#include "Sensor.h"

namespace adxM3
{

	/*
	 * @brief	��ͨ����ѹ����������
	 *			
	 */
	class MultiChannelVoltMeter : public Sensor
	{
	public:


		/*
		 * @brief	����������ͨ�����ؿ���
		 * @param	channel[] ����ͨ���б�
		 *				���������е�ͨ��˳�����ò���˳��
		 *			channels ͨ������
		 *			sta ʹ��״̬
		 *				@arg ENABLE
		 *				@arg DISABLE
		 * @retval	�ɹ�������رշ���1�����򷵻�0
		 * @ovride	����������Ʋ���ͨ���Ŀ�����رգ�������1��Ӧ������������
		 */
		virtual int MultiChannelCommand(const int channel[], const unsigned int channels, FunctionalState sta) = 0;



		/*
		 * @brief	����������
		 * @param	result[] ������������λ��
		 *				�����������õĲ������б���
		 * @retval	���β����ɹ��򷵻�1�����򷵻�0
		 * @ovride	������ֻ��ʵ�ֲ�������
		 */
		virtual int MultiSampling(int result[]) = 0;


	};

}

#endif  /*_MultiChannelVOLTMETER_H_*/
