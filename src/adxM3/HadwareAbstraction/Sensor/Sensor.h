
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
		 * @brief	传感器通道定义，从0开始
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
		 * @brief	传感器采样通道开关控制
		 * @param	channel 采样通道
		 *			sta 使能状态
		 *				@arg ENABLE
		 *				@arg DISABLE
		 * @retval	成功开启或关闭返回1，否则返回0
		 * @ovride	派生类需控制采样通道的开启与关闭，当返回1后应可以立即采样
		 */
		virtual int ChannelCommand(const int channel, FunctionalState sta) = 0;



		/*
		 * @brief	传感器采样
		 * @param	result 采样结果保存的位置
		 * @retval	本次采样成功则返回1，否则返回0
		 * @ovride	派生类只需实现采样即可
		 */
		virtual int Sampling(int& result) = 0;




		/*
		 * @brief	传感器设置精度
		 * @param	accbits 精度位数
		 * @retval	成功设置1，否则返回0
		 */
		virtual int SetAccuracy(const BYTE accbits) = 0;



		/*
		 * @brief	传感器测试转换完成
		 * @retval	当前转换已完成返回true，否则返回false
		 */
		virtual bool ConvertComplete() const = 0;




		/*
		 * @brief	采样时间选项
		 */
		enum SMPT
		{
			VeryShort,					// 非常短
			Short,						// 短
			Normal,						// 普通
			Long,						// 长
			VeryLong,					// 很长
			ExtremelyLong,				// 非常长
			MosquitoesAreAsleep,		// 蚊子都睡着了
		};



		/*
		 * @brief	传感器采样时间配置
		 * @param	sample_time 采样时间，可以是SMPT枚举中的任何一个
		 * @retval	成功设置返回1，否则返回0
		 */
		virtual int SetSampleTime(const SMPT sample_time) = 0;



	protected:

		virtual ~Sensor() {}
	};


}

#endif  /*_SENSOR_H_*/
