
#ifndef _GENERALPURPOSEINPUTOUTPUTPORT_H_
#define _GENERALPURPOSEINPUTOUTPUTPORT_H_

#include "GeneralPurposeInputOutput.h"

namespace adxM3
{


	/*
	 * @brief	通用目的输入输出硬件抽象
	 *			端口抽象模型
	 *			派生类需根据接口说明严格实现接口功能
	 *			部分虚函数已调用接口方法实现其功能，若派生类中有更高效的建议override该方法
	 */
	class GeneralPurposeInputOutputPort : public GeneralPurposeInputOutput
	{
	public:


		
		friend class GeneralPurposeInputOutputPin;



		/*
		 * @brief	默认32位端口，16位端口或8位端口也可兼容
		 */
		typedef UINT32 PortValueType;



		/*
		 * @brief	通用目的输入输出设置端口值
		 * @param	value 端口的值
		 * @notice	输出模式下为设置端口输出值
		 *			输入模式下由派生类根据实际情况决定设置上拉下拉还是输入的值
		 */
		virtual void SetValue(const PortValueType value) = 0;



		/*
		 * @brief	通用目的输入输出读取端口值
		 * @retval	端口的值
		 * @notice	两个方法分别读取输入的值和输出的值
		 */
		virtual PortValueType GetInputValue() const = 0;
		virtual PortValueType GetOutputValue() const = 0;






		/*
		 * @brief	通用目的输入输出设置端口值
		 *			重载 赋值= 运算符提高可读性，派生类可重写此方法
		 * @notice	未指定任何引脚时，对整个端口赋值
		 *			指定了部分引脚时，
		 *			若value等于0则所选引脚会被复位，若value不等于0则所选引脚会被置位
		 */
		virtual void operator=(const PortValueType value)
		{
			SetValue(value);
		}
		virtual void operator=(const int value)
		{
			SetValue(PortValueType(value));
		}



		/*
		 * @brief	通用目的输入输出测试端口的值与所给值是否相等
		 * @param	value 端口值与该值作比较
		 *			port 端口值与port的端口值作比较
		 * @retval	相等则返回true，否则返回false
		 * @notice	相等比较运算符只用于输入模式！
		 *			并且不考虑所选引脚，只比较整个端口的值
		 */
		virtual bool operator==(const PortValueType value) const
		{
			return value == GetInputValue();
		}
		virtual bool operator==(const int value) const
		{
			return PortValueType(value) == GetInputValue();
		}
		virtual bool operator==(const GeneralPurposeInputOutputPort& port) const
		{
			return GetInputValue() == port.GetInputValue();
		}



		/*
		 * @brief	通用目的输入输出端口值隐式转换（功能同GetInputValue方法）
		 *			e.g. PortValueType val = PortA;
		 * @retval	端口值
		 * @notice	只在输入模式下有效，输出模式请勿使用该方法
		 */
		virtual operator PortValueType() const
		{
			return GetInputValue();
		}




		/*
		 * @brief	通用目的输入输出端口标准构造函数
		 *			构造时不选择任何引脚
		 */
		GeneralPurposeInputOutputPort()
		{
		}


	};

	typedef GeneralPurposeInputOutputPort::PortValueType PortValueType;
}


#endif  /*_GENERALPURPOSEINPUTOUTPUTPORT_H_*/
