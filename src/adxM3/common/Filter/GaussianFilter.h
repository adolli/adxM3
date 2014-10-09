
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
		 * @brief	���ֵ��˹�˲���
		 * @tparam	INPUT �������ͣ�Ҫ�����Ĭ�Ϲ��췽��
		 *			Samples �˲�ÿһά����������������ܹ�2 * Samples + 1������
		 *			sigma ��̫�ֲ���׼��
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

				// ʹȨֵ�ܺ�Ϊ1
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

			// ����
			INPUT sample[2 * Samples + 1];
			
			// Ȩֵ��
			float weightTable[2 * Samples + 1];

		};

	}

}

#endif /*_ADX_GAUSSIANFILTER_H_*/
