
#ifndef _ADX_APP_VOLTMETER_H_
#define _ADX_APP_VOLTMETER_H_


namespace adxM3
{


	/*
	 * @brief	��ѹ��
	 * @tparam	SensorChannel ������ͨ��
	 */
	template< class SensorChannel >
	class Voltmeter
	{
	public:

		typedef typename SensorChannel::ValueType ValueType;



		ValueType GetResult()
		{
			ValueType ret;
			Channel.Sampling((int&)ret);
			return ret;
		}



	private:

		SensorChannel Channel;
	};

}

#endif  /*_ADX_APP_VOLTMETER_H_*/
