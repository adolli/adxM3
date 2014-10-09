
#ifndef _FUNCTION_H_
#define _FUNCTION_H_


#include "defs.h"
#include "../../loki/TypeTraits.h"

// 关闭void*到特定FnPtr强制转型无效警告
#pragma diag_suppress 171
#pragma diag_suppress 167

namespace adxM3
{



	/*
	 * @brief	简易的函数类型
	 *			可声明任意类型的函子，并带有绑定和调用两种方法
	 *			初始时函子内的函数指针指向一个不执行任何内容的方法
	 */
	template< typename T >
	class Function;



	template< typename R >
	class Function< R() >
	{
	public:

		/*
		 * @brief	类型简记
		 * @type	FnPtrType 该函子对应函数指针类型
		 *			ThisType 该函子的类型
		 */
		typedef R (*FnPtrType)();
		typedef Function< R() > ThisType;
		


		/*
		 * @brief	函数调用
		 */
		virtual R operator()()
		{
			return FnPtr();
		}
	


		/*
		 * @brief	函数绑定，绑定到一个新的函数指针
		 * @param	fnptr 无论何类型的函数指针，都会被强制转换到本函子对应类型的函数指针
		 */
		template< typename fn_type >
		ThisType& operator=(fn_type fnptr)
		{
			FnPtr = static_cast<FnPtrType>(fnptr);
			return *this;
		}



		/*
		 * @brief	注销函数，当用0给函数赋值时，则当该函数绑定到do_nothing
		 * @param	zero 仅当为0时有效
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
		 * @brief	默认构造函数，构造空函子时默认绑定到一个空函数上
		 */
		Function< R() >()
		{
			FnPtr = do_nothing;
		}
		


		/*
		 * @brief	标准构造函数
		 * @param	fnptr 任意一个函数指针，无论何种指针都会被强制转换到该函子对应的函数指针类型，请务必注意类型匹配
		 */
		template< typename fn_type >
		Function< R() >(fn_type fnptr)
		{
			FnPtr = static_cast<FnPtrType>(fnptr);
		}


	protected:
		FnPtrType FnPtr;



		/*
		 * @brief	空函数，用以初始化函子对象时绑定到该函数上
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
