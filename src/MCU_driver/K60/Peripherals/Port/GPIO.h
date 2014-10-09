
#ifndef _ADX_K60_GPIO_H_
#define _ADX_K60_GPIO_H_

#include "../PeripheralInfo.h"
#include "../PeripheralBase.h"
#include "GlobalPort.h"

namespace K60
{
	


	/*
	 * @brief	ͨ��Ŀ�������������
	 *			�̳���ȫ�ֶ˿ڣ���ΪGPIOʹ��ʱ���л�ģʽ��GPIO��Pin::SetMode��
	 * @tparam	BASE GPIO����Ļ�ַ
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
		 * @brief	��׼���캯��
		 */
		Peripheral()
		{
			// GPIO�̳���GlobalPort
			// ʱ����GlobalPort����
		}

		
	


		/*
		 * @brief	GPIO�˿�������
		 *			��������ʱ��ʹ�ã���ʹ�ø���Ч��ConstPin��
		 *			����Ϊ������ԣ�������ʱ������
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
			 * @brief	�ɸ������Ÿ����������ȡ��ǰ����
			 * @param	alt ���Ÿ������ͻ�ͨ����
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
			 * @brief	�������������������
			 * @param	speed ����ٶȣ������������κ�һ��ֵ
			 *				@arg adxM3::GeneralPurposeInputOutput::LowStrength ��������
			 *				@arg adxM3::GeneralPurposeInputOutput::HighStrength ��������
			 * @notice	�����������ģʽ����Ч
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
			 * @brief	������������ٶȣ�slew rate��
			 * @param	speed ����ٶȣ������������κ�һ��ֵ
			 *				@arg adxM3::GeneralPurposeInputOutput::UltraHighSpeed
			 *				@arg adxM3::GeneralPurposeInputOutput::HighSpeed
			 *				@arg adxM3::GeneralPurposeInputOutput::MediumSpeed
			 *				@arg adxM3::GeneralPurposeInputOutput::LowSpeed
			 *				@arg adxM3::GeneralPurposeInputOutput::UltraLowSpeed
			 *				�����ڵ���MediumSpeedʱ�л���fast slew rate������Ϊslow slew rate
			 * @notice	�����������ģʽ����Ч
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
			 * @brief	�������������˲���
			 * @param	filter �����˲��������������κ�һ��ֵ
			 *				@arg adxM3::GeneralPurposeInputOutput::Disable
			 *				@arg adxM3::GeneralPurposeInputOutput::Lowpass
			 *				@arg adxM3::GeneralPurposeInputOutput::Bandpass
			 *				@arg adxM3::GeneralPurposeInputOutput::Highpass
			 * @notice	������������ģʽ����Ч
			 *			K60���е�ͨ�˲����ܣ�������Ƶ�ʴ���2MHzʱ��رյ�ͨ�˲���
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
			 * @brief	�������ģʽ����
			 * @param	output_type ���ģʽ�������������κ�һ��ֵ
			 *				@arg adxM3::GeneralPurposeInputOutput::OpenDrain
			 *				@arg adxM3::GeneralPurposeInputOutput::PushPull
			 * @notice	�����������ģʽ����Ч
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
			 * @brief	���������ڲ�����������
			 * @param	input_type ����ģʽѡ��
			 *				@arg adxM3::GeneralPurposeInputOutput:::Floating ����
			 *				@arg adxM3::GeneralPurposeInputOutput:::PullUp ��������
			 *				@arg adxM3::GeneralPurposeInputOutput:::PullDown ��������
			 * @notice	������������ģʽ����Ч	
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
			 * @brief	�������������������
			 * @param	dir ����
			 *				@arg GeneralPurposeInputOutput::IN
			 *				@arg GeneralPurposeInputOutput::OUT
			 */
			virtual void SetDirection(const adxM3::GeneralPurposeInputOutput::IODIR dir)
			{
				// ���OUT��IN��ֵ�Ƿ�����K60�Ĵ�����ֵ����Ҫ��
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
			 * @brief	GPIO Pin��λ
			 * @notice	���ģʽ��Ϊ���ø����ŵ�ƽ
			 *			����ģʽ�²�ʹ�ø÷���
			 */
			virtual void Set()
			{
				PeripheralInfo::BitBandAccess()->PSOR[index] = SET;
			}
		


			/*
			 * @brief	GPIO Pin��λ
			 * @notice	���ģʽ��Ϊ���ø����ŵ�ƽ
			 *			����ģʽ�²�ʹ�ø÷���
			 */
			virtual void Reset()
			{
				PeripheralInfo::BitBandAccess()->PCOR[index] = SET;
			}
		


			/*
			 * @brief	GPIO Pin��ƽ����
			 * @param	level Ҫ���õĵ�ƽ��true��1��ʾ�ߵ�ƽ��false��0��ʾ�͵�ƽ
			 * @notice	���ģʽ��Ϊ���ø����ŵ�ƽ
			 *			����ģʽ�²�ʹ�ø÷���
			 */
			virtual void SetLevel(const bool level)
			{
				PeripheralInfo::BitBandAccess()->PDOR[index] = level;
			}



			/*
			 * @brief	GPIO Pin��ȡ���ŵ�ƽ
			 * @retval	���ŵĵ�ƽ��true��1��ʾ�ߵ�ƽ��false��0��ʾ�͵�ƽ
			 * @notice	���ģʽ��Ϊ����ĵ�ƽ
			 *			����ģʽ��Ϊ����ĵ�ƽ
			 */
			virtual bool GetLevel() const
			{
				// ���OUT��IN��ֵ�Ƿ�����K60�Ĵ�����ֵ����Ҫ��
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
			 * @brief	GPIO Pin��ƽ��ת
			 * @notice	���ģʽ�½���ƽ��ת
			 *			����ģʽ�²�ʹ�ø÷���
			 */
			virtual void Toggle()
			{
				PeripheralInfo::BitBandAccess()->PTOR[index] = SET;
			}

			
			
			/*
			 * @brief	ͨ��Ŀ�����������ȡ���ű��
			 * @notice	���ű��Ϊ�˿��е�������
			 */
			virtual PinIndex GetPinSource() const
			{
				return index;
			}
			
			
			/*
			 * @brief	ͨ��Ŀ�����������ȡ��ǰ�˿ں�
			 * @retval	�˿ں�
			 * @notice	���Ƕ˿���ϵͳ�еı��(0~n)
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
		 * @brief	��������
		 * @param	index ���źţ�PTA31��indexΪ31��
		 * @retval	���ظ�����Pin���͵���ʱ����
		 */
		Pin operator[](const unsigned int index) const
		{
			return Pin(index);
		}





		/*
		 * @brief	GPIO��������
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
				// ��������Ƿ񳬳��ö˿ڵ�������
				STATIC_ASSERT(INDEX < GlobalPort::PeripheralInfo::PIN_MAX,
					this_port_does_not_have_so_much_pins);

				// GlobalPort::template ConstPin<INDEX>�Ѿ�����ʱ��

				// GPIO���������Ĭ����ΪGPIO
				GlobalPort::template ConstPin<INDEX>::MultiplexConfig(PinMultiplexing::GPIO);
			}


			virtual ~ConstPin()
			{
				// GPIO����������ͷ�
				GlobalPort::template ConstPin<INDEX>::MultiplexConfig(PinMultiplexing::ALT0);
			}





			/*
			 * @brief	�������������������
			 * @param	speed ����������������������κ�һ��ֵ
			 *				@arg adxM3::GeneralPurposeInputOutput::LowStrength ��������
			 *				@arg adxM3::GeneralPurposeInputOutput::HighStrength ��������
			 * @notice	�����������ģʽ����Ч
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
			 * @brief	������������ٶȣ�slew rate��
			 * @param	speed ����ٶȣ������������κ�һ��ֵ
			 *				@arg adxM3::GeneralPurposeInputOutput::UltraHighSpeed
			 *				@arg adxM3::GeneralPurposeInputOutput::HighSpeed
			 *				@arg adxM3::GeneralPurposeInputOutput::MediumSpeed
			 *				@arg adxM3::GeneralPurposeInputOutput::LowSpeed
			 *				@arg adxM3::GeneralPurposeInputOutput::UltraLowSpeed
			 *				�����ڵ���MediumSpeedʱ�л���fast slew rate������Ϊslow slew rate
			 * @notice	�����������ģʽ����Ч
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
			 * @brief	�������������˲���
			 * @param	filter �����˲��������������κ�һ��ֵ
			 *				@arg adxM3::GeneralPurposeInputOutput::Disable
			 *				@arg adxM3::GeneralPurposeInputOutput::Lowpass
			 *				@arg adxM3::GeneralPurposeInputOutput::Bandpass
			 *				@arg adxM3::GeneralPurposeInputOutput::Highpass
			 * @notice	������������ģʽ����Ч
			 *			K60���е�ͨ�˲����ܣ�������Ƶ�ʴ���2MHzʱ��رյ�ͨ�˲���
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
			 * @brief	�������ģʽ����
			 * @param	output_type ���ģʽ�������������κ�һ��ֵ
			 *				@arg adxM3::GeneralPurposeInputOutput::OpenDrain
			 *				@arg adxM3::GeneralPurposeInputOutput::PushPull
			 * @notice	�����������ģʽ����Ч
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
			 * @brief	���������ڲ�����������
			 * @param	input_type ����ģʽѡ��
			 *				@arg adxM3::GeneralPurposeInputOutput:::Floating ����
			 *				@arg adxM3::GeneralPurposeInputOutput:::PullUp ��������
			 *				@arg adxM3::GeneralPurposeInputOutput:::PullDown ��������
			 * @notice	������������ģʽ����Ч	
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
			 * @brief	�������������������
			 * @param	dir ����
			 *				@arg GeneralPurposeInputOutput::IN
			 *				@arg GeneralPurposeInputOutput::OUT
			 */
			virtual void SetDirection(const adxM3::GeneralPurposeInputOutput::IODIR dir)
			{
				// ���OUT��IN��ֵ�Ƿ�����K60�Ĵ�����ֵ����Ҫ��
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
			 * @brief	GPIO Pin��λ
			 * @notice	���ģʽ��Ϊ���ø����ŵ�ƽ
			 *			����ģʽ�²�ʹ�ø÷���
			 */
			virtual void Set()
			{
				PeripheralInfo::BitBandAccess()->PSOR[INDEX] = SET;
			}
		


			/*
			 * @brief	GPIO Pin��λ
			 * @notice	���ģʽ��Ϊ���ø����ŵ�ƽ
			 *			����ģʽ�²�ʹ�ø÷���
			 */
			virtual void Reset()
			{
				PeripheralInfo::BitBandAccess()->PCOR[INDEX] = SET;
			}
		


			/*
			 * @brief	GPIO Pin��ƽ����
			 * @param	level Ҫ���õĵ�ƽ��true��1��ʾ�ߵ�ƽ��false��0��ʾ�͵�ƽ
			 * @notice	���ģʽ��Ϊ���ø����ŵ�ƽ
			 *			����ģʽ�²�ʹ�ø÷���
			 */
			virtual void SetLevel(const bool level)
			{
				PeripheralInfo::BitBandAccess()->PDOR[INDEX] = level;
			}



			/*
			 * @brief	GPIO Pin��ȡ���ŵ�ƽ
			 * @retval	���ŵĵ�ƽ��true��1��ʾ�ߵ�ƽ��false��0��ʾ�͵�ƽ
			 * @notice	���ģʽ��Ϊ����ĵ�ƽ
			 *			����ģʽ��Ϊ����ĵ�ƽ
			 */
			virtual bool GetLevel() const
			{
				// ���OUT��IN��ֵ�Ƿ�����K60�Ĵ�����ֵ����Ҫ��
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
			 * @brief	GPIO Pin��ƽ��ת
			 * @notice	���ģʽ�½���ƽ��ת
			 *			����ģʽ�²�ʹ�ø÷���
			 */
			virtual void Toggle()
			{
				PeripheralInfo::BitBandAccess()->PTOR[INDEX] = SET;
			}

			
			
			/*
			 * @brief	ͨ��Ŀ�����������ȡ���ű��
			 * @notice	���ű��Ϊ�˿��е�������
			 */
			virtual PinIndex GetPinSource() const
			{
				return INDEX;
			}
			
			

			/*
			 * @brief	ͨ��Ŀ�����������ȡ��ǰ�˿ں�
			 * @retval	�˿ں�
			 * @notice	���Ƕ˿���ϵͳ�еı��(0~n)
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
		 * @brief	GPIO�˿���
		 * @tparam	BitWidth �˿�λ��
		 * @tparam	StartBitShift ��ʼλ����ڶ˿������ţ�Ĭ�ϴ�0��ʼ
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
				// GlobalPort::template Port<BitWidth, StartBitShift>�Ѿ�����ʱ��

				// GPIO���������Ĭ����ΪGPIO
				GlobalPort::template Port<BitWidth, StartBitShift>::MultiplexConfig(PinMultiplexing::GPIO);
				SetDirection(IN);
			}


			virtual ~Port()
			{
				// GPIO����������ͷ�
				GlobalPort::template Port<BitWidth, StartBitShift>::MultiplexConfig(PinMultiplexing::ALT0);
			}








			/*
			 * @brief	ͨ��Ŀ������������ö˿�ֵ
			 * @param	value �˿ڵ�ֵ
			 * @notice	���ģʽ��Ϊ���ö˿����ֵ
			 *			����ģʽ�������������ʵ������������������������������ֵ
			 */
			virtual void SetValue(const PortValueType value)
			{
				PortValueType dor = PeripheralInfo::BaseAccess()->PDOR;
				dor &= ~FieldMask;
				dor |= (value & _BF(BitWidth - 1, 0)) << StartBitShift;
				PeripheralInfo::BaseAccess()->PDOR = dor;
			}



			/*
			 * @brief	ͨ��Ŀ�����������ȡ�˿�ֵ
			 * @retval	�˿ڵ�ֵ
			 * @notice	���������ֱ��ȡ�����ֵ�������ֵ
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
			 * @brief	ͨ��Ŀ����������˿�ֵ��ʽת��
			 * @retval	�˿�ֵ
			 * @notice	�Զ������������ģʽ
			 */
			virtual operator PortValueType() const
			{
				return (UINT16)GetValue();
			}
			







			/*
			 * @brief	ͨ��Ŀ��������������������ģʽ�������������
			 * @param	input_type ����ģʽ���ã�������ITö���е��κ�һ��ֵ
			 *				@arg Floating
			 *				@arg PullUp	
			 *				@arg PullDown
			 *			output_type ���ģʽ���ã�������OTö���е��κ�һ��ֵ
			 *				@arg PushPull
			 *				@arg OpenDrain
			 *			speed ���ģʽ���ٶ�ֵ
			 *				@arg UltraLowSpeed
			 *				@arg LowSpeed
			 *				...
			 *				@arg UltraHighSpeed
			 * @notice	SetOutputSpeed ֵ�����ģʽ��ʹ�ã�����ģʽ�½�ֹʹ��
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
			 * @brief	ͨ��Ŀ����������������������������
			 * @param	dir ����
			 *				@arg IN
			 *				@arg OUT
			 * @retval	���ŷ���
			 */
			virtual void SetDirection(const IODIR dir)
			{
				// ���OUT��IN��ֵ�Ƿ�����K60�Ĵ�����ֵ����Ҫ��
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
			 * @brief	ͨ��Ŀ�����������ȡ����ٶ�
			 * @retval	���ģʽ�ٶȣ�Ϊ���¼���ֵ֮һ
			 *				@arg UltraLowSpeed
			 *				@arg LowSpeed
			 *				...
			 *				@arg UltraHighSpeed
			 * @notice	GetOutputSpeedֻ�����ģʽ��ʹ�ã�����ģʽ��ʹ��������
			 *			���б�Ҫ����ǰģʽΪ�����ģʽʱ�ɷ���NotOutputMode
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
			 * @brief	ͨ��Ŀ�����������ȡ��ǰ�˿ں�
			 * @retval	�˿ں�
			 * @notice	���Ƕ˿���ϵͳ�еı��(0~n)
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
