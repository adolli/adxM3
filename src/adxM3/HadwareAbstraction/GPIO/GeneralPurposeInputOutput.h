
#ifndef _GENERALPURPOSEINPUTOUTPUT_H_
#define _GENERALPURPOSEINPUTOUTPUT_H_

#include "../../common/defs.h"

namespace adxM3
{



	/*
	 * @brief	AFIO����ѡ��
	 */
	enum AFT
	{
		NotAFMode	= 0x0000,			// �Ǹ���ģʽ
		AF_0,
		AF_1,
		AF_2,
		AF_3,
		AF_4,
		AF_5,
		AF_6,
		AF_7,
		AF_PushPull,					// �����������
		AF_OpenDrain					// ���ÿ�©���
	};




	/*
	 * @brief	ͨ��Ŀ���������Ӳ������
	 *			����������ݽӿ�˵���ϸ�ʵ�ֽӿڹ���
	 *			�����麯���ѵ��ýӿڷ���ʵ���书�ܣ������������и���Ч�Ľ���override�÷���
	 */
	class GeneralPurposeInputOutput
	{
	public:



		/*
		 * @brief	ͨ��Ŀ������������������������ѡ��
		 */
		enum IODIR
		{
			IN,
			OUT
		};




		/*
		 * @brief	�����˲���ѡ��
		 */
		enum IFT
		{
			Disable,				// �ر��˲���
			Lowpass,				// ��ͨ�˲���
			Bandpass,				// ��ͨ�˲���
			Highpass				// ��ͨ�˲���
		};



		/*
		 * @brief	����ģʽ����ѡ��
		 */
		enum IT
		{
			Floating	= 0x0100,
			PullUp		= 0x0200,
			PullDown	= 0x0400
		};



		/*
		 * @brief	���ģʽ����ѡ��
		 */
		enum OT
		{
			PushPull	= 0x0800,
			OpenDrain	= 0x1000
		};



		/*
		 * @brief	���ģʽ�ٶ�ѡ��
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
		 * @brief	���ģʽ��������ѡ��
		 */
		enum ODS
		{
			LowStrength,
			HighStrength
		};



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
		 * @ovride	SetOutputSpeed ֵ�����ģʽ��ʹ�ã�����ģʽ�½�ֹʹ��
		 *			����������Ҫ��ÿһ��ģʽ��ÿһ�����ý���ʵ��
		 *			����֧��ĳ��ģʽ����Ҫ�ں�������ʱ����0�����򷵻�1
		 */
		virtual void InputConfig(const IT input_type) = 0;
		virtual void OutputConfig(const OT output_type, const OSP speed = HighSpeed) = 0;
		
		



		/*
		 * @brief	ͨ��Ŀ����������������������������
		 * @param	dir ����
		 *				@arg IN
		 *				@arg OUT
		 * @retval	���ŷ���
		 */
		virtual void SetDirection(const IODIR dir) = 0;
		virtual IODIR GetDirection() const = 0;


		
		
		
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
		virtual void SetOutputSpeed(OSP speed) = 0;
		virtual OSP GetOutputSpeed() const = 0;
		




		/*
		 * @brief	ͨ��Ŀ�����������ȡ��ǰ�˿ں�
		 * @retval	�˿ں�
		 * @notice	���Ƕ˿���ϵͳ�еı��(0~n)
		 */
		virtual int GetPortSource() const = 0;



	protected:


		virtual ~GeneralPurposeInputOutput()
		{
		}


	};

}


#endif  /*_GENERALPURPOSEINPUTOUTPUT_H_*/
