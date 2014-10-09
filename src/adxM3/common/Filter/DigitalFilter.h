
#ifndef _ADX_DIGITALFILTER_H_
#define _ADX_DIGITALFILTER_H_

namespace adxM3
{
	
	namespace FLT
	{


		/*
		 * @brief	�����˲�������
		 *			Ҫ�������ź�������FilterWidth�����������ڱ��ֲ�������ź�
		 * @tparam	INPUT �����ź�����
		 *			FilterWidth �˲���ȣ��������������ڸ���
		 * @notice	�˲�����ʼ״̬�����
		 */
		template< class INPUT, int FilterWidth >
		class DigitalFilter
		{
		public:

			typedef INPUT OUTPUT;


			DigitalFilter()
				: lastInput(INPUT(0)), currentOutput(OUTPUT(0)), filterCount(0), edgeStatus(NO_SKIPPING)
			{
			}


			OUTPUT operator()(INPUT input)
			{
				 if (input == lastInput)
				 {
					 ++filterCount;
					 if (filterCount >= FilterWidth)
					 {
						 filterCount = 0;

						 // �ﵽ�˲������������ֵ
						 if (currentOutput > input)
						 {
							 edgeStatus = FALLING_EDGE;
						 }
						 else if (currentOutput < input)
						 {
							 edgeStatus = RISING_EDGE;
						 }
						 else
						 {
							 edgeStatus = NO_SKIPPING;
						 }
						 currentOutput = input;
					 }
				 }
				 else
				 {
					 lastInput = input;
					 filterCount = 0;
				 }

				 return currentOutput;
			}




			/*
			 * @breif	����Ե�Ƿ����䣬�����ػ��½���
			 */
			bool EdgeFalling() const
			{
				return edgeStatus == FALLING_EDGE;
			}
			bool EdgeRising() const
			{
				return edgeStatus == RISING_EDGE;
			}



			enum EdgeStatus
			{
				NO_SKIPPING,
				FALLING_EDGE,
				RISING_EDGE
			};

		protected:

			INPUT lastInput;		// �ϴ������ֵ
			OUTPUT currentOutput;	// ��ǰ�˲������ֵ
			int filterCount;		// �˲�����
			EdgeStatus edgeStatus;	// �������״̬
			

		};

	}

}

#endif  /*_ADX_DIGITALFILTER_H_*/
