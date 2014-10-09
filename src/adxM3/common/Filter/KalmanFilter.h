
#ifndef _ADX_KALMANFILTER_H_
#define _ADX_KALMANFILTER_H_


#include "../defs.h"


namespace adxM3
{


	namespace FLT
	{


		/*
		 * @brief	定增益卡尔曼滤波器
		 * @tparam	INPUT 采样输入量的类型
		 *			KG 卡尔曼增益
		 */
		template< class INPUT, float KG >
		class KalmanFilter
		{
		public:


			// 涉及到浮点运算，要求输出为浮点型
			typedef float OUTPUT;
			typedef float DeltaOUTPUT;



			virtual OUTPUT operator()(INPUT z_measure)
			{
				outputValue += KG * ((float)z_measure - outputValue);
				return outputValue;
			}


			KalmanFilter()
				: outputValue(0)
			{
			}


			void Reset()
			{
				outputValue = 0;
			}


		protected:

			OUTPUT outputValue;

		};




		/*
		 * @brief	不对称卡尔曼滤波器
		 * @tparam	INPUT 输入数据类型
		 *			KGi 输入增大时卡尔曼增益
		 *			KGd 输入减小时卡尔曼增益
		 */
		template< class INPUT, float KGi, float KGd >
		class AsymmetricKalmanFilter
			: public KalmanFilter<INPUT, KGi>
		{
		public:

			typedef typename KalmanFilter<INPUT, KGi>::OUTPUT OUTPUT;

			virtual OUTPUT operator()(INPUT z_measure)
			{
				float dValue = (float)z_measure - outputValue;
				if (dValue > 0)
				{
					outputValue += KGi * dValue;
				}
				else
				{
					outputValue += KGd * dValue;
				}
				return outputValue;
			}


		protected:

			using KalmanFilter<INPUT, KGi>::outputValue;

		};



	}

}


#endif  /*_ADX_KALMANFILTER_H_*/
