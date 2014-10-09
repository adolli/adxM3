
#ifndef _ADX_KALMANFILTER_H_
#define _ADX_KALMANFILTER_H_


#include "../defs.h"


namespace adxM3
{


	namespace FLT
	{


		/*
		 * @brief	�����濨�����˲���
		 * @tparam	INPUT ����������������
		 *			KG ����������
		 */
		template< class INPUT, float KG >
		class KalmanFilter
		{
		public:


			// �漰���������㣬Ҫ�����Ϊ������
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
		 * @brief	���Գƿ������˲���
		 * @tparam	INPUT ������������
		 *			KGi ��������ʱ����������
		 *			KGd �����Сʱ����������
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
