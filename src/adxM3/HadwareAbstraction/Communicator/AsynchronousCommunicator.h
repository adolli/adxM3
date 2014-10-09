
#ifndef _ASYNCHRONOUSCOMMUNICATOR_H_
#define _ASYNCHRONOUSCOMMUNICATOR_H_

#include "Communicator.h"

namespace adxM3
{


	/*
	 * @brief	�첽ͨѶ���ӿ��࣬�̳��ڻ���ͨѶ��
	 *			�ýӿ��ṩ�첽ͨѶֹͣλ���÷���
	 */
	class AsynchronousCommunicator : public ConfigurableCommunicator
	{
	public:



		/*
		 * @brief	ֹͣλö��
		 */
		enum STB
		{
			STOPBITS_05			= 0x01,				// 0.5��ֹͣλ
			STOPBITS_10			= 0x00,				// 1.0��ֹͣλ
			STOPBITS_15			= 0x03,				// 1.5��ֹͣλ
			STOPBITS_20			= 0x02,				// 2.0��ֹͣλ
		};



		/*
		 * @brief	�첽ͨѶ��ֹͣλ����/��ȡ
		 * @param	stb ֹͣλö��
		 *				@arg STOPBITS_05
		 *				@arg STOPBITS_10
		 *				@arg STOPBITS_15
		 *				@arg STOPBITS_20
		 * @retval	���÷����У��ɹ������򷵻�1�����򷵻�0
		 *			��ȡ�����У���֧��ֹͣλ����STOPBITS_10��û�ж�Ӧ��ֹͣλ�򷵻�ƥ��ֵ�����
		 */
		virtual int SetStopBits(const STB stb) = 0;
		virtual STB GetStopBits() const = 0;


	};

}


#endif  /*_ASYNCHRONOUSCOMMUNICATOR_H_*/
