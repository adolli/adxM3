
#ifndef _ADX_DIGITALFILTER_H_
#define _ADX_DIGITALFILTER_H_

namespace adxM3
{
	
	namespace FLT
	{


		/*
		 * @brief	数字滤波器抽象
		 *			要求输入信号在连续FilterWidth个采样周期内保持才输出该信号
		 * @tparam	INPUT 输入信号类型
		 *			FilterWidth 滤波宽度，即连续采样周期个数
		 * @notice	滤波器初始状态无输出
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

						 // 达到滤波脉宽后更新输出值
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
			 * @breif	检测边缘是否跳变，上升沿或下降沿
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

			INPUT lastInput;		// 上次输入的值
			OUTPUT currentOutput;	// 当前滤波器输出值
			int filterCount;		// 滤波计数
			EdgeStatus edgeStatus;	// 跳变边沿状态
			

		};

	}

}

#endif  /*_ADX_DIGITALFILTER_H_*/
