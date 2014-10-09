
#ifndef _GENERALPURPOSEINPUTOUTPUT_H_
#define _GENERALPURPOSEINPUTOUTPUT_H_

#include "../../common/defs.h"

namespace adxM3
{



	/*
	 * @brief	AFIO类型选择
	 */
	enum AFT
	{
		NotAFMode	= 0x0000,			// 非复用模式
		AF_0,
		AF_1,
		AF_2,
		AF_3,
		AF_4,
		AF_5,
		AF_6,
		AF_7,
		AF_PushPull,					// 复用推挽输出
		AF_OpenDrain					// 复用开漏输出
	};




	/*
	 * @brief	通用目的输入输出硬件抽象
	 *			派生类需根据接口说明严格实现接口功能
	 *			部分虚函数已调用接口方法实现其功能，若派生类中有更高效的建议override该方法
	 */
	class GeneralPurposeInputOutput
	{
	public:



		/*
		 * @brief	通用目的输入输出引脚输入输出方向选项
		 */
		enum IODIR
		{
			IN,
			OUT
		};




		/*
		 * @brief	输入滤波器选项
		 */
		enum IFT
		{
			Disable,				// 关闭滤波器
			Lowpass,				// 低通滤波器
			Bandpass,				// 带通滤波器
			Highpass				// 高通滤波器
		};



		/*
		 * @brief	输入模式类型选择
		 */
		enum IT
		{
			Floating	= 0x0100,
			PullUp		= 0x0200,
			PullDown	= 0x0400
		};



		/*
		 * @brief	输出模式类型选择
		 */
		enum OT
		{
			PushPull	= 0x0800,
			OpenDrain	= 0x1000
		};



		/*
		 * @brief	输出模式速度选择
		 */
		enum OSP
		{
			NotOutputMode,
			UltraLowSpeed,
			LowSpeed,
			MediumSpeed,
			HighSpeed,
			UltraHighSpeed
		};



		/*
		 * @brief	输出模式驱动能力选择
		 */
		enum ODS
		{
			LowStrength,
			HighStrength
		};



		/*
		 * @brief	通用目的输入输出设置输入输出模式，输入输出方向
		 * @param	input_type 输入模式配置，可以是IT枚举中的任何一个值
		 *				@arg Floating
		 *				@arg PullUp	
		 *				@arg PullDown
		 *			output_type 输出模式配置，可以是OT枚举中的任何一个值
		 *				@arg PushPull
		 *				@arg OpenDrain
		 *			speed 输出模式的速度值
		 *				@arg UltraLowSpeed
		 *				@arg LowSpeed
		 *				...
		 *				@arg UltraHighSpeed
		 * @ovride	SetOutputSpeed 值在输出模式下使用，输入模式下禁止使用
		 *			派生类中需要对每一种模式和每一种配置进行实现
		 *			若不支持某种模式，需要在函数返回时返回0，否则返回1
		 */
		virtual void InputConfig(const IT input_type) = 0;
		virtual void OutputConfig(const OT output_type, const OSP speed = HighSpeed) = 0;
		
		



		/*
		 * @brief	通用目的输入输出引脚设置输入输出方向
		 * @param	dir 方向
		 *				@arg IN
		 *				@arg OUT
		 * @retval	引脚方向
		 */
		virtual void SetDirection(const IODIR dir) = 0;
		virtual IODIR GetDirection() const = 0;


		
		
		
		/*
		 * @brief	通用目的输入输出获取输出速度
		 * @retval	输出模式速度，为以下几种值之一
		 *				@arg UltraLowSpeed
		 *				@arg LowSpeed
		 *				...
		 *				@arg UltraHighSpeed
		 * @notice	GetOutputSpeed只在输出模式下使用，输入模式下使用无意义
		 *			若有必要，当前模式为非输出模式时可返回NotOutputMode
		 */
		virtual void SetOutputSpeed(OSP speed) = 0;
		virtual OSP GetOutputSpeed() const = 0;
		




		/*
		 * @brief	通用目的输入输出获取当前端口号
		 * @retval	端口号
		 * @notice	仅是端口在系统中的编号(0~n)
		 */
		virtual int GetPortSource() const = 0;



	protected:


		virtual ~GeneralPurposeInputOutput()
		{
		}


	};

}


#endif  /*_GENERALPURPOSEINPUTOUTPUT_H_*/
