

//////////////////////////////////////////////////////////////////////////
// @file		SignalMultiplexing.h
// @brief		��·���õ��ź����Ͷ���
//
//
//
//////////////////////////////////////////////////////////////////////////


#ifndef _ADX_K60_SIGNALMULTIPLEXING_H_
#define _ADX_K60_SIGNALMULTIPLEXING_H_

#include "adxM3.h"

namespace K60
{

	namespace PinMultiplexing
	{

		/*
		 * @brief	�źŸ��������ѡ��
		 */
		enum MUX_MODE
		{
			ALT0,
			ALT1,
			ALT2,
			ALT3,
			ALT4,
			ALT5,
			ALT6,
			ALT7,
			EzPort,
			
			ANALOG	= 0,
			GPIO	= 1
		};

	}
	
	using namespace PinMultiplexing;


	
	/*
	 * @brief	��·�������ź�����ģ��
	 *			ͨ��Port Pin MUX ��������ȷ���ź�ͨ��
	 * @tparam	PORT_INFO_Type �˿���Ϣ����
	 *			PIN_INDEX ����������
	 *			ALT_INDEX ����ͨ��������
	 */
	template< class PORT_INFO_Type, PinIndex PIN_INDEX, MUX_MODE ALT_INDEX >
	class Signal
	{
	public:
		typedef PORT_INFO_Type PortInfo;
		enum 
		{ 
			Pin = PIN_INDEX
		};

		static const MUX_MODE MUX_ALT = ALT_INDEX;
	};





}

#endif  /*_ADX_K60_SIGNALMULTIPLEXING_H_*/
