
#ifndef _ADX_K60_GPIO_H_
#define _ADX_K60_GPIO_H_

#include "../PeripheralInfo.h"
#include "../PeripheralBase.h"
#include "GlobalPort.h"

namespace K60
{
	


	/*
	 * @brief	通用目的输入输出外设
	 *			继承于全局端口，作为GPIO使用时请切换模式到GPIO（Pin::SetMode）
	 * @tparam	BASE GPIO外设的基址
	 *				@arg PTA_BASE
	 *				@arg PTB_BASE
	 *				...
	 *				@arg PTE_BASE
	 */
	template< UINT32 BASE >
	class Peripheral< GPIO_INFO<BASE> >
		: protected Peripheral< typename GPIO_INFO<BASE>::GlobalPortInfo >
	{
	public:

		typedef typename GPIO_INFO<BASE>::GlobalPortInfo	GlobalPortInfo;
		typedef GPIO_INFO<BASE>								PeripheralInfo;
		typedef Peripheral<GlobalPortInfo>					GlobalPort;
		


		/*
		 * @brief	标准构造函数
		 */
		Peripheral()
		{
			// GPIO继承于GlobalPort
			// 时钟由GlobalPort控制
		}

		
	


		/*
		 * @brief	GPIO端口引脚类
		 *			【此类暂时不使用，请使用更高效的ConstPin】
		 *			【但为了灵活性，此类暂时保留】
		 */
		class Pin
			: public GlobalPort::Pin
			, public adxM3::GeneralPurposeInputOutputPin
		{
		public:
			
			using GlobalPort::Pin::index;
			using adxM3::GeneralPurposeInputOutputPin::operator=;
		
			Pin(unsigned int _index)
				: GlobalPort::Pin(_index)
			{
			}
		
		


			/*
			 * @brief	可复用引脚复用配置与获取当前配置
			 * @param	alt 引脚复用类型或通道号
			 */
			virtual int MultiplexConfig(unsigned int alt)
			{
				GlobalPort::Pin::MultiplexConfig(alt);
				return 1;
			}
			virtual unsigned int GetMultiplexing() const
			{
				return 0;
			}




			/*
			 * @brief	设置引脚输出驱动能力
			 * @param	speed 输出速度，可以是以下任何一个值
			 *				@arg adxM3::GeneralPurposeInputOutput::LowStrength 低驱动力
			 *				@arg adxM3::GeneralPurposeInputOutput::HighStrength 高驱动力
			 * @notice	仅在数字输出模式下有效
			 */
			void SetDriveStrength(const adxM3::GeneralPurposeInputOutput::ODS strength)
			{
				STATIC_ASSERT(
						adxM3::GeneralPurposeInputOutput::LowStrength == 0
					&&	adxM3::GeneralPurposeInputOutput::HighStrength == 1,
					enumerate_value_is_not_campacted_with_this_configuration);
				
				GlobalPort::PeripheralInfo::BitBandAccess()
						->PCR[index][PORT_BitBandType::PCR_DSE] = strength;
			}



			/*
			 * @brief	设置引脚输出速度（slew rate）
			 * @param	speed 输出速度，可以是以下任何一个值
			 *				@arg adxM3::GeneralPurposeInputOutput::UltraHighSpeed
			 *				@arg adxM3::GeneralPurposeInputOutput::HighSpeed
			 *				@arg adxM3::GeneralPurposeInputOutput::MediumSpeed
			 *				@arg adxM3::GeneralPurposeInputOutput::LowSpeed
			 *				@arg adxM3::GeneralPurposeInputOutput::UltraLowSpeed
			 *				当大于等于MediumSpeed时切换到fast slew rate，否则为slow slew rate
			 * @notice	仅在数字输出模式下有效
			 */
			virtual void SetOutputSpeed(const OSP speed)
			{
				switch (speed)
				{
				case adxM3::GeneralPurposeInputOutput::UltraHighSpeed :
				case adxM3::GeneralPurposeInputOutput::HighSpeed :
				case adxM3::GeneralPurposeInputOutput::MediumSpeed :
					GlobalPort::PeripheralInfo::BitBandAccess()
						->PCR[index][PORT_BitBandType::PCR_SRE] = RESET;
					break;

				case adxM3::GeneralPurposeInputOutput::LowSpeed :
				case adxM3::GeneralPurposeInputOutput::UltraLowSpeed :
					// SET : slow slew rate
					GlobalPort::PeripheralInfo::BitBandAccess()
						->PCR[index][PORT_BitBandType::PCR_SRE] = SET;
					break;

				default:;
				}
			}
			virtual OSP GetOutputSpeed() const
			{
				if (GlobalPort::PeripheralInfo::BitBandAccess()
						->PCR[index][PORT_BitBandType::PCR_SRE] == SET)
				{
					return LowSpeed;
				}
				else
				{
					return HighSpeed;
				}
			}




			/*
			 * @brief	设置引脚输入滤波器
			 * @param	filter 输入滤波，可以是以下任何一个值
			 *				@arg adxM3::GeneralPurposeInputOutput::Disable
			 *				@arg adxM3::GeneralPurposeInputOutput::Lowpass
			 *				@arg adxM3::GeneralPurposeInputOutput::Bandpass
			 *				@arg adxM3::GeneralPurposeInputOutput::Highpass
			 * @notice	仅在数字输入模式下有效
			 *			K60仅有低通滤波功能，当输入频率大于2MHz时请关闭低通滤波器
			 */
			void SetInputFilter(const adxM3::GeneralPurposeInputOutput::IFT filter)
			{
				switch (filter)
				{
				case adxM3::GeneralPurposeInputOutput::Lowpass :
					GlobalPort::PeripheralInfo::BitBandAccess()
						->PCR[index][PORT_BitBandType::PCR_PFE] = SET;
					break;

				default:
					GlobalPort::PeripheralInfo::BitBandAccess()
						->PCR[index][PORT_BitBandType::PCR_PFE] = RESET;
					break;
				}
			}




			/*
			 * @brief	引脚输出模式配置
			 * @param	output_type 输出模式，可以是以下任何一个值
			 *				@arg adxM3::GeneralPurposeInputOutput::OpenDrain
			 *				@arg adxM3::GeneralPurposeInputOutput::PushPull
			 * @notice	仅在数字输出模式下有效
			 */
			virtual void OutputConfig(const OT output_type, const OSP speed = HighSpeed)
			{
				SetDirection(OUT);
				if (output_type == adxM3::GeneralPurposeInputOutput::OpenDrain)
				{
					GlobalPort::PeripheralInfo::BitBandAccess()
						->PCR[index][PORT_BitBandType::PCR_ODE] = SET;
				}
				else
				{
					GlobalPort::PeripheralInfo::BitBandAccess()
						->PCR[index][PORT_BitBandType::PCR_ODE] = RESET;
				}
				SetOutputSpeed(speed);
			}
			


			/*
			 * @brief	配置引脚内部上下拉电阻
			 * @param	input_type 输入模式选项
			 *				@arg adxM3::GeneralPurposeInputOutput:::Floating 浮空
			 *				@arg adxM3::GeneralPurposeInputOutput:::PullUp 上拉电阻
			 *				@arg adxM3::GeneralPurposeInputOutput:::PullDown 下拉电阻
			 * @notice	仅在数字输入模式下有效	
			 */
			virtual void InputConfig(const IT input_type)
			{
				switch (input_type)
				{
				case adxM3::GeneralPurposeInputOutput::Floating :
					GlobalPort::PeripheralInfo::BitBandAccess()->PCR[index][PORT_BitBandType::PCR_PE] = RESET;
					break;

				case adxM3::GeneralPurposeInputOutput::PullUp :
					GlobalPort::PeripheralInfo::BitBandAccess()->PCR[index][PORT_BitBandType::PCR_PS] = SET;
					GlobalPort::PeripheralInfo::BitBandAccess()->PCR[index][PORT_BitBandType::PCR_PE] = SET;
					break;

				case adxM3::GeneralPurposeInputOutput::PullDown :
					GlobalPort::PeripheralInfo::BitBandAccess()->PCR[index][PORT_BitBandType::PCR_PS] = RESET;
					GlobalPort::PeripheralInfo::BitBandAccess()->PCR[index][PORT_BitBandType::PCR_PE] = SET;
					break;

				default:;
				}
			}



			/*
			 * @brief	设置引脚输入输出方向
			 * @param	dir 方向
			 *				@arg GeneralPurposeInputOutput::IN
			 *				@arg GeneralPurposeInputOutput::OUT
			 */
			virtual void SetDirection(const adxM3::GeneralPurposeInputOutput::IODIR dir)
			{
				// 检查OUT和IN的值是否满足K60寄存器的值定义要求
				STATIC_ASSERT(	adxM3::GeneralPurposeInputOutput::OUT == 1
							&&	adxM3::GeneralPurposeInputOutput::IN == 0,
							enumerate_value_is_not_campacted_with_this_configuration );
				PeripheralInfo::BitBandAccess()->PDDR[index] = bool(dir);
			}
			virtual IODIR GetDirection() const
			{
				STATIC_ASSERT(	adxM3::GeneralPurposeInputOutput::OUT == 1
							&&	adxM3::GeneralPurposeInputOutput::IN == 0,
							enumerate_value_is_not_campacted_with_this_configuration );
				return IODIR(PeripheralInfo::BitBandAccess()->PDDR[index]);
			}
			
			

			/*
			 * @brief	GPIO Pin置位
			 * @notice	输出模式下为设置该引脚电平
			 *			输入模式下不使用该方法
			 */
			virtual void Set()
			{
				PeripheralInfo::BitBandAccess()->PSOR[index] = SET;
			}
		


			/*
			 * @brief	GPIO Pin复位
			 * @notice	输出模式下为设置该引脚电平
			 *			输入模式下不使用该方法
			 */
			virtual void Reset()
			{
				PeripheralInfo::BitBandAccess()->PCOR[index] = SET;
			}
		


			/*
			 * @brief	GPIO Pin电平设置
			 * @param	level 要设置的电平，true或1表示高电平，false或0表示低电平
			 * @notice	输出模式下为设置该引脚电平
			 *			输入模式下不使用该方法
			 */
			virtual void SetLevel(const bool level)
			{
				PeripheralInfo::BitBandAccess()->PDOR[index] = level;
			}



			/*
			 * @brief	GPIO Pin读取引脚电平
			 * @retval	引脚的电平，true或1表示高电平，false或0表示低电平
			 * @notice	输出模式下为输出的电平
			 *			输入模式下为输入的电平
			 */
			virtual bool GetLevel() const
			{
				// 检查OUT和IN的值是否满足K60寄存器的值定义要求
				STATIC_ASSERT(	adxM3::GeneralPurposeInputOutput::OUT == 1
							&&	adxM3::GeneralPurposeInputOutput::IN == 0,
							enumerate_value_is_not_campacted_with_this_configuration);

				if (PeripheralInfo::BitBandAccess()->PDDR[index] == adxM3::GeneralPurposeInputOutput::OUT)
				{
					return PeripheralInfo::BitBandAccess()->PDOR[index];
				}
				else
				{
					return PeripheralInfo::BitBandAccess()->PDIR[index];
				}
			}



			/*
			 * @brief	GPIO Pin电平翻转
			 * @notice	输出模式下将电平翻转
			 *			输入模式下不使用该方法
			 */
			virtual void Toggle()
			{
				PeripheralInfo::BitBandAccess()->PTOR[index] = SET;
			}

			
			
			/*
			 * @brief	通用目的输入输出获取引脚编号
			 * @notice	引脚编号为端口中的索引号
			 */
			virtual PinIndex GetPinSource() const
			{
				return index;
			}
			
			
			/*
			 * @brief	通用目的输入输出获取当前端口号
			 * @retval	端口号
			 * @notice	仅是端口在系统中的编号(0~n)
			 */
			virtual int GetPortSource() const
			{
				switch (PeripheralInfo::BaseAddress)
				{
				case PTA_BASE :
					return 0;

				case PTB_BASE :
					return 1;

				case PTC_BASE :
					return 2;

				case PTD_BASE :
					return 3;

				case PTE_BASE :
					return 4;

				default:
					return -1;
				}
			}

		};
	


		/*
		 * @brief	索引引脚
		 * @param	index 引脚号（PTA31则index为31）
		 * @retval	返回该类中Pin类型的临时对象
		 */
		Pin operator[](const unsigned int index) const
		{
			return Pin(index);
		}





		/*
		 * @brief	GPIO常引脚类
		 */
		template< PinIndex INDEX >
		class ConstPin 
			: public GlobalPort::template ConstPin<INDEX>
			, public adxM3::GeneralPurposeInputOutputPin
		{
		public:

			using adxM3::GeneralPurposeInputOutputPin::operator=;


			ConstPin()
			{
				// 检查引脚是否超出该端口的引脚数
				STATIC_ASSERT(INDEX < GlobalPort::PeripheralInfo::PIN_MAX,
					this_port_does_not_have_so_much_pins);

				// GlobalPort::template ConstPin<INDEX>已经开启时钟

				// GPIO对象把引脚默认设为GPIO
				GlobalPort::template ConstPin<INDEX>::MultiplexConfig(PinMultiplexing::GPIO);
			}


			virtual ~ConstPin()
			{
				// GPIO对象把引脚释放
				GlobalPort::template ConstPin<INDEX>::MultiplexConfig(PinMultiplexing::ALT0);
			}





			/*
			 * @brief	设置引脚输出驱动能力
			 * @param	speed 输出驱动力，可以是以下任何一个值
			 *				@arg adxM3::GeneralPurposeInputOutput::LowStrength 低驱动力
			 *				@arg adxM3::GeneralPurposeInputOutput::HighStrength 高驱动力
			 * @notice	仅在数字输出模式下有效
			 */
			void SetDriveStrength(const adxM3::GeneralPurposeInputOutput::ODS strength)
			{
				STATIC_ASSERT(
						adxM3::GeneralPurposeInputOutput::LowStrength == 0
					&&	adxM3::GeneralPurposeInputOutput::HighStrength == 1,
					enumerate_value_is_not_campacted_with_this_configuration);
				
				GlobalPort::PeripheralInfo::BitBandAccess()
						->PCR[INDEX][PORT_BitBandType::PCR_DSE] = strength;
			}



			/*
			 * @brief	设置引脚输出速度（slew rate）
			 * @param	speed 输出速度，可以是以下任何一个值
			 *				@arg adxM3::GeneralPurposeInputOutput::UltraHighSpeed
			 *				@arg adxM3::GeneralPurposeInputOutput::HighSpeed
			 *				@arg adxM3::GeneralPurposeInputOutput::MediumSpeed
			 *				@arg adxM3::GeneralPurposeInputOutput::LowSpeed
			 *				@arg adxM3::GeneralPurposeInputOutput::UltraLowSpeed
			 *				当大于等于MediumSpeed时切换到fast slew rate，否则为slow slew rate
			 * @notice	仅在数字输出模式下有效
			 */
			virtual void SetOutputSpeed(const OSP speed)
			{
				switch (speed)
				{
				case adxM3::GeneralPurposeInputOutput::UltraHighSpeed :
				case adxM3::GeneralPurposeInputOutput::HighSpeed :
				case adxM3::GeneralPurposeInputOutput::MediumSpeed :
					GlobalPort::PeripheralInfo::BitBandAccess()
						->PCR[INDEX][PORT_BitBandType::PCR_SRE] = RESET;
					break;

				case adxM3::GeneralPurposeInputOutput::LowSpeed :
				case adxM3::GeneralPurposeInputOutput::UltraLowSpeed :
					// SET : slow slew rate
					GlobalPort::PeripheralInfo::BitBandAccess()
						->PCR[INDEX][PORT_BitBandType::PCR_SRE] = SET;
					break;

				default:;
				}
			}
			virtual OSP GetOutputSpeed() const
			{
				if (GlobalPort::PeripheralInfo::BitBandAccess()
						->PCR[INDEX][PORT_BitBandType::PCR_SRE] == SET)
				{
					return LowSpeed;
				}
				else
				{
					return HighSpeed;
				}
			}




			/*
			 * @brief	设置引脚输入滤波器
			 * @param	filter 输入滤波，可以是以下任何一个值
			 *				@arg adxM3::GeneralPurposeInputOutput::Disable
			 *				@arg adxM3::GeneralPurposeInputOutput::Lowpass
			 *				@arg adxM3::GeneralPurposeInputOutput::Bandpass
			 *				@arg adxM3::GeneralPurposeInputOutput::Highpass
			 * @notice	仅在数字输入模式下有效
			 *			K60仅有低通滤波功能，当输入频率大于2MHz时请关闭低通滤波器
			 */
			void SetInputFilter(const adxM3::GeneralPurposeInputOutput::IFT filter)
			{
				switch (filter)
				{
				case adxM3::GeneralPurposeInputOutput::Lowpass :
					GlobalPort::PeripheralInfo::BitBandAccess()
						->PCR[INDEX][PORT_BitBandType::PCR_PFE] = SET;
					break;

				default:
					GlobalPort::PeripheralInfo::BitBandAccess()
						->PCR[INDEX][PORT_BitBandType::PCR_PFE] = RESET;
					break;
				}
			}




			/*
			 * @brief	引脚输出模式配置
			 * @param	output_type 输出模式，可以是以下任何一个值
			 *				@arg adxM3::GeneralPurposeInputOutput::OpenDrain
			 *				@arg adxM3::GeneralPurposeInputOutput::PushPull
			 * @notice	仅在数字输出模式下有效
			 */
			virtual void OutputConfig(const OT output_type, const OSP speed = HighSpeed)
			{
				SetDirection(OUT);
				if (output_type == adxM3::GeneralPurposeInputOutput::OpenDrain)
				{
					GlobalPort::PeripheralInfo::BitBandAccess()
						->PCR[INDEX][PORT_BitBandType::PCR_ODE] = SET;
				}
				else
				{
					GlobalPort::PeripheralInfo::BitBandAccess()
						->PCR[INDEX][PORT_BitBandType::PCR_ODE] = RESET;
				}
				SetOutputSpeed(speed);
			}
			


			/*
			 * @brief	配置引脚内部上下拉电阻
			 * @param	input_type 输入模式选项
			 *				@arg adxM3::GeneralPurposeInputOutput:::Floating 浮空
			 *				@arg adxM3::GeneralPurposeInputOutput:::PullUp 上拉电阻
			 *				@arg adxM3::GeneralPurposeInputOutput:::PullDown 下拉电阻
			 * @notice	仅在数字输入模式下有效	
			 */
			virtual void InputConfig(const IT input_type)
			{
				switch (input_type)
				{
				case adxM3::GeneralPurposeInputOutput::Floating :
					GlobalPort::PeripheralInfo::BitBandAccess()->PCR[INDEX][PORT_BitBandType::PCR_PE] = RESET;
					break;

				case adxM3::GeneralPurposeInputOutput::PullUp :
					GlobalPort::PeripheralInfo::BitBandAccess()->PCR[INDEX][PORT_BitBandType::PCR_PS] = SET;
					GlobalPort::PeripheralInfo::BitBandAccess()->PCR[INDEX][PORT_BitBandType::PCR_PE] = SET;
					break;

				case adxM3::GeneralPurposeInputOutput::PullDown :
					GlobalPort::PeripheralInfo::BitBandAccess()->PCR[INDEX][PORT_BitBandType::PCR_PS] = RESET;
					GlobalPort::PeripheralInfo::BitBandAccess()->PCR[INDEX][PORT_BitBandType::PCR_PE] = SET;
					break;

				default:;
				}
			}



			/*
			 * @brief	设置引脚输入输出方向
			 * @param	dir 方向
			 *				@arg GeneralPurposeInputOutput::IN
			 *				@arg GeneralPurposeInputOutput::OUT
			 */
			virtual void SetDirection(const adxM3::GeneralPurposeInputOutput::IODIR dir)
			{
				// 检查OUT和IN的值是否满足K60寄存器的值定义要求
				STATIC_ASSERT(	adxM3::GeneralPurposeInputOutput::OUT == 1
							&&	adxM3::GeneralPurposeInputOutput::IN == 0,
							enumerate_value_is_not_campacted_with_this_configuration);
				PeripheralInfo::BitBandAccess()->PDDR[INDEX] = bool(dir);
			}
			virtual IODIR GetDirection() const
			{
				STATIC_ASSERT(	adxM3::GeneralPurposeInputOutput::OUT == 1
							&&	adxM3::GeneralPurposeInputOutput::IN == 0,
							enumerate_value_is_not_campacted_with_this_configuration);
				return IODIR(PeripheralInfo::BitBandAccess()->PDDR[INDEX]);
			}
			
			

			/*
			 * @brief	GPIO Pin置位
			 * @notice	输出模式下为设置该引脚电平
			 *			输入模式下不使用该方法
			 */
			virtual void Set()
			{
				PeripheralInfo::BitBandAccess()->PSOR[INDEX] = SET;
			}
		


			/*
			 * @brief	GPIO Pin复位
			 * @notice	输出模式下为设置该引脚电平
			 *			输入模式下不使用该方法
			 */
			virtual void Reset()
			{
				PeripheralInfo::BitBandAccess()->PCOR[INDEX] = SET;
			}
		


			/*
			 * @brief	GPIO Pin电平设置
			 * @param	level 要设置的电平，true或1表示高电平，false或0表示低电平
			 * @notice	输出模式下为设置该引脚电平
			 *			输入模式下不使用该方法
			 */
			virtual void SetLevel(const bool level)
			{
				PeripheralInfo::BitBandAccess()->PDOR[INDEX] = level;
			}



			/*
			 * @brief	GPIO Pin读取引脚电平
			 * @retval	引脚的电平，true或1表示高电平，false或0表示低电平
			 * @notice	输出模式下为输出的电平
			 *			输入模式下为输入的电平
			 */
			virtual bool GetLevel() const
			{
				// 检查OUT和IN的值是否满足K60寄存器的值定义要求
				STATIC_ASSERT(	adxM3::GeneralPurposeInputOutput::OUT == 1
							&&	adxM3::GeneralPurposeInputOutput::IN == 0,
							enumerate_value_is_not_campacted_with_this_configuration);

				if (PeripheralInfo::BitBandAccess()->PDDR[INDEX] == adxM3::GeneralPurposeInputOutput::OUT)
				{
					return PeripheralInfo::BitBandAccess()->PDOR[INDEX];
				}
				else
				{
					return PeripheralInfo::BitBandAccess()->PDIR[INDEX];
				}
			}



			/*
			 * @brief	GPIO Pin电平翻转
			 * @notice	输出模式下将电平翻转
			 *			输入模式下不使用该方法
			 */
			virtual void Toggle()
			{
				PeripheralInfo::BitBandAccess()->PTOR[INDEX] = SET;
			}

			
			
			/*
			 * @brief	通用目的输入输出获取引脚编号
			 * @notice	引脚编号为端口中的索引号
			 */
			virtual PinIndex GetPinSource() const
			{
				return INDEX;
			}
			
			

			/*
			 * @brief	通用目的输入输出获取当前端口号
			 * @retval	端口号
			 * @notice	仅是端口在系统中的编号(0~n)
			 */
			virtual int GetPortSource() const
			{
				switch (PeripheralInfo::BaseAddress)
				{
				case PTA_BASE :
					return 0;

				case PTB_BASE :
					return 1;

				case PTC_BASE :
					return 2;

				case PTD_BASE :
					return 3;

				case PTE_BASE :
					return 4;

				default:
					return -1;
				}
			}


		};





		/*
		 * @brief	GPIO端口类
		 * @tparam	BitWidth 端口位宽
		 * @tparam	StartBitShift 起始位相对于端口索引号，默认从0开始
		 */
		template< UINT32 BitWidth, PinIndex StartBitShift = 0 >
		class Port
			: public GlobalPort::template Port<BitWidth, StartBitShift>
			, public GeneralPurposeInputOutputPort
		{

		public:

			enum
			{
				FieldMask = _BF(StartBitShift + BitWidth - 1, StartBitShift)
			};


		public:

			using adxM3::GeneralPurposeInputOutputPort::operator=;


			typedef typename GeneralPurposeInputOutputPort::PortValueType PortValueType;



			Port()
				: currentIODir(IN)
			{
				// GlobalPort::template Port<BitWidth, StartBitShift>已经开启时钟

				// GPIO对象把引脚默认设为GPIO
				GlobalPort::template Port<BitWidth, StartBitShift>::MultiplexConfig(PinMultiplexing::GPIO);
				SetDirection(IN);
			}


			virtual ~Port()
			{
				// GPIO对象把引脚释放
				GlobalPort::template Port<BitWidth, StartBitShift>::MultiplexConfig(PinMultiplexing::ALT0);
			}








			/*
			 * @brief	通用目的输入输出设置端口值
			 * @param	value 端口的值
			 * @notice	输出模式下为设置端口输出值
			 *			输入模式下由派生类根据实际情况决定设置上拉下拉还是输入的值
			 */
			virtual void SetValue(const PortValueType value)
			{
				PortValueType dor = PeripheralInfo::BaseAccess()->PDOR;
				dor &= ~FieldMask;
				dor |= (value & _BF(BitWidth - 1, 0)) << StartBitShift;
				PeripheralInfo::BaseAccess()->PDOR = dor;
			}



			/*
			 * @brief	通用目的输入输出读取端口值
			 * @retval	端口的值
			 * @notice	两个方法分别读取输入的值和输出的值
			 */
			virtual PortValueType GetInputValue() const
			{
				return PortValueType(PeripheralInfo::BaseAccess()->PDIR & FieldMask) >> StartBitShift;
			}
			virtual PortValueType GetOutputValue() const
			{
				return PortValueType(PeripheralInfo::BaseAccess()->PDOR & FieldMask) >> StartBitShift;
			}
			PortValueType GetValue() const
			{
				if (currentIODir == OUT)
				{
					return GetOutputValue();
				}
				else
				{
					return GetInputValue();
				}
			}		





			/*
			 * @brief	通用目的输入输出端口值隐式转换
			 * @retval	端口值
			 * @notice	自动区分输入输出模式
			 */
			virtual operator PortValueType() const
			{
				return (UINT16)GetValue();
			}
			







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
			 * @notice	SetOutputSpeed 值在输出模式下使用，输入模式下禁止使用
			 */
			virtual void InputConfig(const IT input_type)
			{
				switch (input_type)
				{
				case adxM3::GeneralPurposeInputOutput::Floating :
					for (PinIndex i = StartBitShift; i < BitWidth + StartBitShift; ++i)
					{
						PeripheralInfo::BitBandAccess()->PDDR[i] = IN;
						GlobalPort::PeripheralInfo::BitBandAccess()->PCR[i][PORT_BitBandType::PCR_PE] = RESET;
					}
					break;

				case adxM3::GeneralPurposeInputOutput::PullUp :
					for (PinIndex i = StartBitShift; i < BitWidth + StartBitShift; ++i)
					{
						PeripheralInfo::BitBandAccess()->PDDR[i] = IN;
						GlobalPort::PeripheralInfo::BitBandAccess()->PCR[i][PORT_BitBandType::PCR_PS] = SET;
						GlobalPort::PeripheralInfo::BitBandAccess()->PCR[i][PORT_BitBandType::PCR_PE] = SET;
					}
					break;

				case adxM3::GeneralPurposeInputOutput::PullDown :
					for (PinIndex i = StartBitShift; i < BitWidth + StartBitShift; ++i)
					{
						PeripheralInfo::BitBandAccess()->PDDR[i] = IN;
						GlobalPort::PeripheralInfo::BitBandAccess()->PCR[i][PORT_BitBandType::PCR_PS] = RESET;
						GlobalPort::PeripheralInfo::BitBandAccess()->PCR[i][PORT_BitBandType::PCR_PE] = SET;
					}
					break;

				default:;
				}
			}
			virtual void OutputConfig(const OT output_type, const OSP speed = HighSpeed)
			{
				if (output_type == adxM3::GeneralPurposeInputOutput::OpenDrain)
				{
					for (PinIndex i = StartBitShift; i < BitWidth + StartBitShift; ++i)
					{
						PeripheralInfo::BitBandAccess()->PDDR[i] = OUT;
						GlobalPort::PeripheralInfo::BitBandAccess()
							->PCR[i][PORT_BitBandType::PCR_ODE] = SET;
					}
				}
				else
				{
					for (PinIndex i = StartBitShift; i < BitWidth + StartBitShift; ++i)
					{
						PeripheralInfo::BitBandAccess()->PDDR[i] = OUT;
						GlobalPort::PeripheralInfo::BitBandAccess()
							->PCR[i][PORT_BitBandType::PCR_ODE] = RESET;
					}
				}
				SetOutputSpeed(speed);
			}
		
		



			/*
			 * @brief	通用目的输入输出引脚设置输入输出方向
			 * @param	dir 方向
			 *				@arg IN
			 *				@arg OUT
			 * @retval	引脚方向
			 */
			virtual void SetDirection(const IODIR dir)
			{
				// 检查OUT和IN的值是否满足K60寄存器的值定义要求
				STATIC_ASSERT(	adxM3::GeneralPurposeInputOutput::OUT == 1
					&&	adxM3::GeneralPurposeInputOutput::IN == 0,
					enumerate_value_is_not_campacted_with_this_configuration);

				currentIODir = dir;
				for (PinIndex i = StartBitShift; i < BitWidth + StartBitShift; ++i)
				{
					PeripheralInfo::BitBandAccess()->PDDR[i] = bool(dir);
				}
			}
			virtual IODIR GetDirection() const
			{
				return currentIODir;
			}


		
		
		
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
			virtual void SetOutputSpeed(OSP speed)
			{
				switch (speed)
				{
				case adxM3::GeneralPurposeInputOutput::UltraHighSpeed :
				case adxM3::GeneralPurposeInputOutput::HighSpeed :
				case adxM3::GeneralPurposeInputOutput::MediumSpeed :
					for (PinIndex i = StartBitShift; i < BitWidth + StartBitShift; ++i)
					{
						GlobalPort::PeripheralInfo::BitBandAccess()
							->PCR[i][PORT_BitBandType::PCR_SRE] = RESET;
					}
					break;

				case adxM3::GeneralPurposeInputOutput::LowSpeed :
				case adxM3::GeneralPurposeInputOutput::UltraLowSpeed :
					// SET : slow slew rate
					for (PinIndex i = StartBitShift; i < BitWidth + StartBitShift; ++i)
					{
						GlobalPort::PeripheralInfo::BitBandAccess()
							->PCR[i][PORT_BitBandType::PCR_SRE] = SET;
					}
					break;

				default:;
				}
			}
			virtual OSP GetOutputSpeed() const
			{
				return OSP(GlobalPort::PeripheralInfo::BitBandAccess()->PCR[StartBitShift][PORT_BitBandType::PCR_SRE]);
			}
		




			/*
			 * @brief	通用目的输入输出获取当前端口号
			 * @retval	端口号
			 * @notice	仅是端口在系统中的编号(0~n)
			 */
			virtual int GetPortSource() const
			{
				switch (PeripheralInfo::BaseAddress)
				{
				case PTA_BASE :
					return 0;

				case PTB_BASE :
					return 1;

				case PTC_BASE :
					return 2;

				case PTD_BASE :
					return 3;

				case PTE_BASE :
					return 4;

				default:
					return -1;
				}
			}



		private:


			typename GeneralPurposeInputOutput::IODIR currentIODir;




		};

	};

}

#endif  /*_ADX_K60_GPIO_H_*/
