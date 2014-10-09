
#ifndef _FUNCTION_H_
#define _FUNCTION_H_


#include "defs.h"
#include "../../loki/TypeTraits.h"

// �ر�void*���ض�FnPtrǿ��ת����Ч����
#pragma diag_suppress 171
#pragma diag_suppress 167

namespace adxM3
{



	/*
	 * @brief	���׵ĺ�������
	 *			�������������͵ĺ��ӣ������а󶨺͵������ַ���
	 *			��ʼʱ�����ڵĺ���ָ��ָ��һ����ִ���κ����ݵķ���
	 */
	template< typename T >
	class Function;



	template< typename R >
	class Function< R() >
	{
	public:

		/*
		 * @brief	���ͼ��
		 * @type	FnPtrType �ú��Ӷ�Ӧ����ָ������
		 *			ThisType �ú��ӵ�����
		 */
		typedef R (*FnPtrType)();
		typedef Function< R() > ThisType;
		


		/*
		 * @brief	��������
		 */
		virtual R operator()()
		{
			return FnPtr();
		}
	


		/*
		 * @brief	�����󶨣��󶨵�һ���µĺ���ָ��
		 * @param	fnptr ���ۺ����͵ĺ���ָ�룬���ᱻǿ��ת���������Ӷ�Ӧ���͵ĺ���ָ��
		 */
		template< typename fn_type >
		ThisType& operator=(fn_type fnptr)
		{
			FnPtr = static_cast<FnPtrType>(fnptr);
			return *this;
		}



		/*
		 * @brief	ע������������0��������ֵʱ���򵱸ú����󶨵�do_nothing
		 * @param	zero ����Ϊ0ʱ��Ч
		 */
		ThisType& operator=(int zero)
		{
			if (zero == 0)
			{
				FnPtr = do_nothing;
			}
			return *this;
		}
		


		/*
		 * @brief	Ĭ�Ϲ��캯��������պ���ʱĬ�ϰ󶨵�һ���պ�����
		 */
		Function< R() >()
		{
			FnPtr = do_nothing;
		}
		


		/*
		 * @brief	��׼���캯��
		 * @param	fnptr ����һ������ָ�룬���ۺ���ָ�붼�ᱻǿ��ת�����ú��Ӷ�Ӧ�ĺ���ָ�����ͣ������ע������ƥ��
		 */
		template< typename fn_type >
		Function< R() >(fn_type fnptr)
		{
			FnPtr = static_cast<FnPtrType>(fnptr);
		}


	protected:
		FnPtrType FnPtr;



		/*
		 * @brief	�պ��������Գ�ʼ�����Ӷ���ʱ�󶨵��ú�����
		 */
		static R do_nothing()
		{
		}

	};



	template< typename R, typename P1 >
	class Function< R(P1) >
	{
	public:

		typedef typename Loki::TypeTraits<P1>::ParameterType Parm1;

		typedef R (*FnPtrType)(Parm1);
		typedef Function< R(Parm1) > ThisType;

		virtual R operator()(Parm1 p1)
		{
			return FnPtr(p1);
		}
	
		template< typename fn_type >
		ThisType& operator=(fn_type fnptr)
		{
			FnPtr = static_cast<FnPtrType>(fnptr);
			return *this;
		}

		ThisType& operator=(int zero)
		{
			if (zero == 0)
			{
				FnPtr = do_nothing;
			}
			return *this;
		}



		Function()
		{
			FnPtr = do_nothing;
		}

		template< typename fn_type >
		Function(fn_type fnptr)
		{
			FnPtr = static_cast<FnPtrType>(fnptr);
		}

	protected:
		FnPtrType FnPtr;
		
		
		static R do_nothing(Parm1)
		{
		}

	};



	template< typename R, typename P1, typename P2 >
	class Function< R(P1, P2) >
	{
	public:

		typedef typename Loki::TypeTraits<P1>::ParameterType Parm1;
		typedef typename Loki::TypeTraits<P2>::ParameterType Parm2;

		typedef R (*FnPtrType)(Parm1, Parm2);
		typedef Function< R(Parm1, Parm2) > ThisType;

		virtual R operator()(Parm1 p1, Parm2 p2)
		{
			return FnPtr(p1, p2);
		}
	
		template< typename fn_type >
		ThisType& operator=(fn_type fnptr)
		{
			FnPtr = static_cast<FnPtrType>(fnptr);
			return *this;
		}

		ThisType& operator=(int zero)
		{
			if (zero == 0)
			{
				FnPtr = do_nothing;
			}
			return *this;
		}

		

		Function()
		{
			FnPtr = do_nothing;
		}

		template< typename fn_type >
		Function(fn_type fnptr)
		{
			FnPtr = static_cast<FnPtrType>(fnptr);
		}

	protected:
		FnPtrType FnPtr;
		
		
		static R do_nothing(Parm1, Parm2)
		{
		}

	};



	template< typename R, typename P1, typename P2, typename P3 >
	class Function< R(P1, P2, P3) >
	{
	public:

		typedef typename Loki::TypeTraits<P1>::ParameterType Parm1;
		typedef typename Loki::TypeTraits<P2>::ParameterType Parm2;
		typedef typename Loki::TypeTraits<P3>::ParameterType Parm3;

		typedef R (*FnPtrType)(Parm1, Parm2, Parm3);
		typedef Function< R(Parm1, Parm2, Parm3) > ThisType;

		virtual R operator()(Parm1 p1, Parm2 p2, Parm3 p3)
		{
			return FnPtr(p1, p2, p3);
		}
	
		template< typename fn_type >
		ThisType& operator=(fn_type fnptr)
		{
			FnPtr = static_cast<FnPtrType>(fnptr);
			return *this;
		}

		ThisType& operator=(int zero)
		{
			if (zero == 0)
			{
				FnPtr = do_nothing;
			}
			return *this;
		}

		

		Function()
		{
			FnPtr = do_nothing;
		}

		template< typename fn_type >
		Function(fn_type fnptr)
		{
			FnPtr = static_cast<FnPtrType>(fnptr);
		}

	protected:
		FnPtrType FnPtr;
		
		
		static R do_nothing(Parm1, Parm2, Parm3)
		{
		}

	};


}


#endif  /*_FUNCTION_H_*/
