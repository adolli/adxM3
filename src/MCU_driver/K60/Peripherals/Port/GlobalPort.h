
#ifndef _ADX_K60_GLOBALPORT_H_
#define _ADX_K60_GLOBALPORT_H_

#include "../PeripheralBase.h"
#include "../PeripheralInfo.h"
#include "../K60_reg.h"

namespace K60
{

	
	

	/*
	 * @brief	ȫ�ֶ˿�����
	 * @tparam	BASE �����ַ
	 *				@arg PORTA_BASE
	 *				@arg PORTB_BASE
	 *				...
	 *				@arg PORTE_BASE
	 */
	template< UINT32 BASE >
	class Peripheral< PORT_INFO<BASE> >
	{
	public:

		typedef PORT_INFO<BASE> PeripheralInfo;
		typedef ClockGateController<typename PeripheralInfo::ClockSourceGate> ClockSource;


		/*
		 * @brief	��׼���캯��
		 *			����ȫ�ֶ˿�ʱ��
		 */
		Peripheral()
		{
			ClockSource::ClockOn();
		}



		/*
		 * @brief	��׼��������
		 */
		virtual ~Peripheral()
		{
			// ʹ���������������ü����󣬻������һ������ʵ������ʱ�ر�ʱ��
			ClockSource::ClockOff();
		}




		/*
		 * @brief	�˿�������
		 */
		class Pin
		{
		public:

			Pin(unsigned int _index)
				: index(_index)
			{
			}

			~Pin()
			{
			}
		


			/*
			 * @brief	�������Ŷ�·����
			 *			ѡͨĳ���ź����ӵ��ⲿ����
			 * @param	alt_type �˿��ź�ͨ����
			 */
			void MultiplexConfig(unsigned int alt_type)
			{
				PeripheralInfo::BaseAccess()->PCR[index].MUX = alt_type;
			}





		protected:

			// ����������
			const PinIndex index;
		};




		/*
		 * @brief	ģ�泣��������
		 */
		template< PinIndex INDEX >
		class ConstPin
		{
		public:

			ConstPin()
			{
				// ��������Ƿ񳬳��ö˿ڵ�������
				STATIC_ASSERT(INDEX < PeripheralInfo::PIN_MAX,
					this_port_does_not_have_so_much_pins);

				// �����������ڴ˿���ʱ��
				ClockSource::ClockOn();
			}


			~ConstPin()
			{
				// ͬ�������������Ҫ�ر�ʱ��
				ClockSource::ClockOff();
			}



			/*
			 * @brief	�������Ŷ�·����
			 *			ѡͨĳ���ź����ӵ��ⲿ����
			 * @param	alt_type �˿��ź�ͨ����
			 */
			static void MultiplexConfig(unsigned int alt)
			{
				PeripheralInfo::BaseAccess()->PCR[INDEX].MUX = alt;
			}
			static unsigned int GetMultiplexing()
			{
				return PeripheralInfo::BaseAccess()->PCR[INDEX].MUX;
			}

		};


	


		/*
		 * @brief	�˿���
		 * @tparam	BitWidth �˿�λ��
		 * @tparam	StartBitShift ��ʼλ����ڶ˿������ţ�Ĭ�ϴ�0��ʼ
		 */
		template< UINT32 BitWidth, PinIndex StartBitShift = 0 >
		class Port
		{
		public:




			Port()
			{
				// ��������Ƿ񳬳��ö˿ڵ�������
				STATIC_ASSERT(BitWidth + StartBitShift < PeripheralInfo::PIN_MAX && StartBitShift >= 0,
					this_port_does_not_have_so_much_pins);

				// �����������ڴ˿���ʱ��
				ClockSource::ClockOn();
			}



			~Port()
			{
				// ͬ�������������Ҫ�ر�ʱ��
				ClockSource::ClockOff();
			}




			

			/*
			 * @brief	�������Ŷ�·����
			 *			ѡͨĳ���ź����ӵ��ⲿ����
			 * @param	alt_type �˿��ź�ͨ����
			 */
			static void MultiplexConfig(unsigned int alt)
			{
				for (PinIndex i = StartBitShift; i < StartBitShift + BitWidth; ++i)
				{
					PeripheralInfo::BaseAccess()->PCR[i].MUX = alt;
				}
			}
			static unsigned int GetMultiplexing()
			{
				// һ��Port���������Pin��������ͬ��MUX����
				return PeripheralInfo::BaseAccess()->PCR[StartBitShift].MUX;
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



	};



	/*
	 * @brief	ȫ�ֶ˿ڣ�ģ��˿ڼ��ϣ�
	 */
	template<>
	class Peripheral< PORT_INFO<ANALOG_PORT> >
	{
	public:



		template< PinIndex INDEX >
		class ConstPin
		{
		public:

			
			/*
			 * @brief	ģ��˿�ǿ��ΪALT0״̬�������·����������
			 *			�������ڴ���ռλ���ã�
			 */
			static void MultiplexConfig(unsigned int)
			{
				// always ALT0
			}

		};
	};



	

}

#endif  /*_ADX_K60_GLOBALPORT_H_*/
