
#ifndef _TIMERIOENABLER_H_
#define _TIMERIOENABLER_H_


namespace adxM3
{

	class TimerIOEnabler
	{
	public:


		/*
		 * @brief	脉宽调制器输出使能/禁止
		 *			并配置好输入输出相关模式设置
		 * @retval	成功开启/关闭输出返回1，否则返回0
		 */
		virtual int EnableOutput() = 0;
		virtual int DisableOutput() = 0;



		virtual ~TimerIOEnabler() {}
	};

}


#endif  /*_TIMERIOENABLER_H_*/
