
#ifndef _ADX_CM_LINEARCCD_H_
#define _ADX_CM_LINEARCCD_H_


namespace adxM3
{



	template
	<
		int _SampleCount,
		class CLKSignal,
		class SISignal,
		class AOSignal,
		class MCU
	>
	class LinearCCD
	{
	public:


		typedef typename AOSignal::ValueType ValueType;

		enum { SampleCount = _SampleCount };


		LinearCCD()
		{
			CLK.OutputConfig(CLKSignal::PushPull);
			SI.OutputConfig(SISignal::PushPull);
		}



		int Sampling(ValueType* data)
		{
			CLK = 1;
			SI = 0;
			MCU::StaticDelay_ns(500);
			
			SI = 1;
			CLK = 0;
			MCU::StaticDelay_ns(500);

			CLK = 1;
			SI = 0;
			MCU::StaticDelay_ns(500);

			for (int i = 0; i < SampleCount; ++i)
			{
				CLK = 0;
				MCU::StaticDelay_ns(500);
				int val = 0;
				AO.Sampling(val);
				data[i] = PixelFilter[i](val);
				CLK = 1;
				MCU::StaticDelay_ns(500);
			}

			return 1;
		}


		


	protected:


		CLKSignal CLK;
		SISignal SI;
		AOSignal AO;


		FLT::KalmanFilter<int, 0.60> PixelFilter[SampleCount];
	};

}


#endif  /*_ADX_CM_LINEARCCD_H_*/
