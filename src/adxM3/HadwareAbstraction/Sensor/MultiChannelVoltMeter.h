
#ifndef _MultiChannelVOLTMETER_H_
#define _MultiChannelVOLTMETER_H_

#include "Sensor.h"

namespace adxM3
{

	/*
	 * @brief	多通道电压传感器抽象
	 *			
	 */
	class MultiChannelVoltMeter : public Sensor
	{
	public:


		/*
		 * @brief	传感器采样通道开关控制
		 * @param	channel[] 采样通道列表
		 *				按照数据中的通道顺序配置采样顺序
		 *			channels 通道总数
		 *			sta 使能状态
		 *				@arg ENABLE
		 *				@arg DISABLE
		 * @retval	成功开启或关闭返回1，否则返回0
		 * @ovride	派生类需控制采样通道的开启与关闭，当返回1后应可以立即采样
		 */
		virtual int MultiChannelCommand(const int channel[], const unsigned int channels, FunctionalState sta) = 0;



		/*
		 * @brief	传感器采样
		 * @param	result[] 采样结果保存的位置
		 *				按照事先配置的采样序列保存
		 * @retval	本次采样成功则返回1，否则返回0
		 * @ovride	派生类只需实现采样即可
		 */
		virtual int MultiSampling(int result[]) = 0;


	};

}

#endif  /*_MultiChannelVOLTMETER_H_*/
