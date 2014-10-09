
#ifndef _ADX_DATABUFFER_H_
#define _ADX_DATABUFFER_H_

#include "../assert.h"
#include "../TypeTraits.h"

#include <cmath>
using namespace std;

namespace adxM3
{


	template< class ValueType, int BufferSize >
	class DataBufferQueen
	{
	public:


		DataBufferQueen()
			: currentIndex(0)
		{
			STATIC_ASSERT(BufferSize >= 1, buffersize_should_be_greater_equal_than_one);
		}



		/*
		 * @brief	数据入队
		 * @param	input 入队的数据
		 * @retval	返回出队数据
		 */
		ValueType EnQueen(ValueType input)
		{
			ValueType ret;
			if (currentIndex != BufferSize - 1)
			{
				++currentIndex;
			}
			else
			{
				currentIndex = 0;
			}
			ret = dataQueen[currentIndex];
			dataQueen[currentIndex] = input;
			return ret;
		}
		ValueType operator()(ValueType input)
		{
			return EnQueen(input);
		}




		/*
		 * @brief	获取最后入队的值
		 */
		ValueType GetBack() const
		{
			return dataQueen[currentIndex];
		}



		/*
		 * @brief	计算数据趋势斜率
		 * @retval	返回数据队列中最新一个数据减去最旧一个数据除以队列大小
		 */
		float GetTrendSlope() const
		{
			int prevIndex = currentIndex != BufferSize - 1 ? currentIndex + 1 : 0;
			return ((float)dataQueen[currentIndex] - (float)dataQueen[prevIndex]) / (float)BufferSize;
		}



		/*
		 * @brief	获取平均值
		 * @retval	队列中所有数据算术平均
		 */
		float GetAverageValue() const
		{
			typename ArithTraits<ValueType>::MaxBitsSigned sum = 0;
			for (int i = 0; i < BufferSize; ++i)
			{
				sum += dataQueen[i];
			}
			return sum / BufferSize;
		}




	protected:

		ValueType dataQueen[BufferSize];
		int currentIndex;

	};





	/*
	 * @brief	单侧正态分布权值数据缓冲队列
	 */ 
	template< class ValueType, int BufferSize, float Sigma >
	class GaussianDistributedDataBufferQueen
		: public DataBufferQueen<ValueType, BufferSize>
	{
	public:


		GaussianDistributedDataBufferQueen()
		{
			float weightSum = 0;
			for (int x = 0; x < BufferSize; ++x)
			{
				weightTable[x] = exp(-(x * x) / (2 * Sigma * Sigma));
				weightSum += weightTable[x];
			}

			// 使权值总和为1
			for (int x = 0; x < BufferSize; ++x)
			{
				weightTable[x] /= weightSum;
			}
		}




		/*
		 * @brief	获取正态平均值，越新的值权重越大
		 */
		float GetNormalAverageValue() const
		{
			float sum = 0;
			int currentIdx = currentIndex;
			for (int i = 0; i < BufferSize; ++i)
			{
				sum += dataQueen[currentIdx] * weightTable[i];
				--currentIdx;
				if (currentIdx < 0)
				{
					currentIdx = BufferSize - 1;
				}
			}
			return sum;
		}



	protected:

		
		using DataBufferQueen<ValueType, BufferSize>::currentIndex;
		using DataBufferQueen<ValueType, BufferSize>::dataQueen;

		float weightTable[BufferSize];

	};





}


#endif  /*_ADX_DATABUFFER_H_*/
