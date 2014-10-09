
#ifndef _ADX_GAUSSIANFILTER_H_
#define _ADX_GAUSSIANFILTER_H_

#include "../assert.h"

#include <cmath>
using namespace std;

namespace adxM3
{

	namespace FLT
	{


		/*
		 * @brief	零均值高斯滤波器
		 * @tparam	INPUT 输入类型，要求带有默认构造方法
		 *			Samples 滤波每一维单侧采样样本数，总共2 * Samples + 1个样本
		 *			sigma 正太分布标准差
		 */
		template< class INPUT, int Samples, float Sigma >
		class GaussianFilter
		{
		public:


			typedef INPUT OUTPUT;
			typedef INPUT DeltaOUTPUT;


			GaussianFilter()
			{
				STATIC_ASSERT(Samples > 0, parameter_error);

				float weightSum = 0;
				for (int x = -Samples; x <= Samples; ++x)
				{
					weightTable[x + Samples] = exp(-(x * x) / (2 * Sigma * Sigma));
					weightSum += weightTable[x + Samples];
				}

				// 使权值总和为1
				for (int x = -Samples; x <= Samples; ++x)
				{
					weightTable[x + Samples] /= weightSum;
				}
			}



			OUTPUT operator()(INPUT data)
			{
				OUTPUT output = 0;
				for (int x = 0; x < 2 * Samples; ++x)
				{
					sample[x] = sample[x + 1];
					output += weightTable[x] * sample[x];
				}
				sample[2 * Samples] = data;
				output += weightTable[2 * Samples] * sample[2 * Samples];

				return output;
			}


		protected:

			// 样本
			INPUT sample[2 * Samples + 1];
			
			// 权值表
			float weightTable[2 * Samples + 1];

		};

	}

}

#endif /*_ADX_GAUSSIANFILTER_H_*/
