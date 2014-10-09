
#ifndef _GENERALPURPOSEINPUTOUTPUTPIN_H_
#define _GENERALPURPOSEINPUTOUTPUTPIN_H_

#include "GeneralPurposeInputOutput.h"
#include "MultiplexedPin.h"

namespace adxM3
{


	/*
	 * @brief	通用目的输入输出硬件抽象
	 *			引脚抽象模型
	 *			派生类需根据接口说明严格实现接口功能
	 *			部分虚函数已调用接口方法实现其功能，若派生类中有更高效的建议override该方法
	 */
	class GeneralPurposeInputOutputPin : public GeneralPurposeInputOutput
	{
	public:





		/*
		 * @brief	通用目的输入输出引脚置位
		 * @notice	输出模式下为设置该引脚电平
		 *			输入模式下不使用该方法
		 */
		virtual void Set() = 0;
		


		/*
		 * @brief	通用目的输入输出引脚复位
		 * @notice	输出模式下为设置该引脚电平
		 *			输入模式下不使用该方法
		 */
		virtual void Reset() = 0;
		


		/*
		 * @brief	通用目的输入输出引脚电平设置
		 * @param	level 要设置的电平，true或1表示高电平，false或0表示低电平
		 * @notice	输出模式下为设置该引脚电平
		 *			输入模式下不使用该方法
		 */
		virtual void SetLevel(const bool level) = 0;



		/*
		 * @brief	通用目的输入输出读取引脚电平
		 * @retval	引脚的电平，true或1表示高电平，false或0表示低电平
		 * @notice	输出模式下为输出的电平
		 *			输入模式下为输入的电平
		 */
		virtual bool GetLevel() const = 0;



		/*
		 * @brief	通用目的输入输出引脚电平翻转
		 * @notice	输出模式下将电平翻转
		 *			输入模式下不使用该方法
		 */
		virtual void Toggle()
		{
			SetLevel(!GetLevel());
		}



		/*
		 * @brief	通用目的输入输出获取引脚电平取反值
		 * @retval	返回引脚电平取反的值
		 */
		virtual bool operator!()
		{
			return !GetLevel();
		}



		/*
		 * @brief	通用目的输入输出引脚电平设置
		 *			重载 赋值= 运算符，功能同SetLevel方法
		 * @param	level 要设置的电平，true或1表示高电平，false或0表示低电平
		 *			(level为int类型是为了匹配立即数类型)
		 * @retval	返回对象自身引用
		 */
		virtual GeneralPurposeInputOutputPin& operator=(const int level)
		{
			SetLevel(level);
			return *this;
		}
		virtual GeneralPurposeInputOutputPin& operator=(const GeneralPurposeInputOutputPin& pin)
		{
			SetLevel(pin.GetLevel());
			return *this;
		}
		


		/*
		 * @brief	通用目的输入输出比较引脚电平值
		 * @param	level 引脚电平与该值作比较(level为int类型是为了匹配立即数类型)
		 *			pin 引脚电平与pin的电平作比较
		 * @retval	比较结果相同时返回true，否则返回false
		 */
		virtual bool operator==(const int level)
		{
			return level == GetLevel();
		}
		virtual bool operator==(const GeneralPurposeInputOutputPin& pin)
		{
			return GetLevel() == pin.GetLevel();
		}



		/*
		 * @brief	通用目的输入输出隐式转换读取引脚电平值（功能同GetLevel方法）
		 * @retval	引脚电平值
		 */
		virtual operator bool()
		{
			return GetLevel();
		}



		/*
		 * @brief	通用目的输入输出获取引脚编号
		 * @notice	引脚编号为端口中的索引号
		 */
		virtual PinIndex GetPinSource() const = 0;


	};

}


#endif  /*_GENERALPURPOSEINPUTOUTPUTPIN_H_*/
