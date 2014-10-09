
#ifndef _TYPETRAITS_H_
#define _TYPETRAITS_H_

#include "defs.h"

namespace adxM3
{


	/*
	 * @group	pointer type traits 
	 */
	template< typename T >
	class PointerTraits;



	template< typename T >
	class PointerTraits<T*>
	{
	public:
		enum 
		{ 
			IsPointerToConst = false, 
			IsPointerToNonconst = !IsPointerToConst,
			IsConstPointer = false
		};
		typedef T value_type;
	};


	template< typename T >
	class PointerTraits<T* const>
	{
	public:
		enum 
		{ 
			IsPointerToConst = false, 
			IsPointerToNonconst = !IsPointerToConst,
			IsConstPointer = true
		};
		typedef T value_type;
	};


	template< typename T >
	class PointerTraits<const T*>
	{
	public:
		enum 
		{ 
			IsPointerToConst = true, 
			IsPointerToNonconst = !IsPointerToConst,
			IsConstPointer = false
		};
		typedef T value_type;
	};


	template< typename T >
	class PointerTraits<const T* const>
	{
	public:
		enum 
		{ 
			IsPointerToConst = true, 
			IsPointerToNonconst = !IsPointerToConst,
			IsConstPointer = true
		};
		typedef T value_type;
	};



#include <cstdint>

	/*
	 * @group	Integer type traits 
	 */
	template< typename T >
	class ArithTraits;


	template<>
	class ArithTraits<int8_t>
	{
	public:
		enum { IsSigned = 1, IsUnsigned = !IsSigned };
		enum { BitWide = 8 };
		enum { MAXVALUE = 0x7F, MINVALUE = ~0x7F };
		typedef int8_t Signed;
		typedef uint8_t Unsigned;
		typedef int32_t MaxBitsSigned;
		typedef uint32_t MaxBitsUnsigned;
	};

	template<>
	class ArithTraits<uint8_t>
	{
	public:
		enum { IsSigned = 0, IsUnsigned = !IsSigned };
		enum { BitWide = 8 };
		enum { MAXVALUE = 0xFFU, MINVALUE = 0 };
		typedef int8_t Signed;
		typedef uint8_t Unsigned;
		typedef int32_t MaxBitsSigned;
		typedef uint32_t MaxBitsUnsigned;
	};

	template<>
	class ArithTraits<int16_t>
	{
	public:
		enum { IsSigned = 1, IsUnsigned = !IsSigned };
		enum { BitWide = 16 };
		enum { MAXVALUE = 0x7FFF, MINVALUE = ~0x7FFF };
		typedef int16_t Signed;
		typedef uint16_t Unsigned;
		typedef int32_t MaxBitsSigned;
		typedef uint32_t MaxBitsUnsigned;
	};

	template<>
	class ArithTraits<uint16_t>
	{
	public:
		enum { IsSigned = 0, IsUnsigned = !IsSigned };
		enum { BitWide = 16 };
		enum { MAXVALUE = 0xFFFFU, MINVALUE = 0 };
		typedef int16_t Signed;
		typedef uint16_t Unsigned;
		typedef int32_t MaxBitsSigned;
		typedef uint32_t MaxBitsUnsigned;
	};

	template<>
	class ArithTraits<int32_t>
	{
	public:
		enum { IsSigned = 1, IsUnsigned = !IsSigned };
		enum { BitWide = 32 };
		enum { MAXVALUE = 0x7FFFFFFF, MINVALUE = ~0x7FFFFFFF };
		typedef int32_t Signed;
		typedef uint32_t Unsigned;
		typedef int32_t MaxBitsSigned;
		typedef uint32_t MaxBitsUnsigned;
	};

	template<>
	class ArithTraits<uint32_t>
	{
	public:
		enum { IsSigned = 0, IsUnsigned = !IsSigned };
		enum { BitWide = 32 };
		enum { MAXVALUE = 0xFFFFFFFFU, MINVALUE = 0 };
		typedef int32_t Signed;
		typedef uint32_t Unsigned;
		typedef int32_t MaxBitsSigned;
		typedef uint32_t MaxBitsUnsigned;
	};

	template<>
	class ArithTraits<int64_t>
	{
	public:
		enum { IsSigned = 1, IsUnsigned = !IsSigned };
		enum { BitWide = 64 };
		enum { MAXVALUE = 0x7FFFFFFFFFFFFFFFLL, MINVALUE = ~0x7FFFFFFFFFFFFFFFLL };
		typedef int64_t Signed;
		typedef uint64_t Unsigned;
		typedef int64_t MaxBitsSigned;
		typedef uint64_t MaxBitsUnsigned;
	};

	template<>
	class ArithTraits<uint64_t>
	{
	public:
		enum { IsSigned = 0, IsUnsigned = !IsSigned };
		enum { BitWide = 64 };
		enum { MAXVALUE = 0xFFFFFFFFFFFFFFFFULL, MINVALUE = 0 };
		typedef int64_t Signed;
		typedef uint64_t Unsigned;
		typedef int64_t MaxBitsSigned;
		typedef uint64_t MaxBitsUnsigned;
	};

	
	template<>
	class ArithTraits<float>
	{
	public:
		enum { IsSigned = 1, IsUnsigned = !IsSigned };
		enum { BitWide = 32 };
		static const float MAXVALUE;
		static const float MINVALUE;
		typedef float Signed;
		typedef float Unsigned;
		typedef double MaxBitsSigned;
		typedef double MaxBitsUnsigned;
	};
	
	
	template<>
	class ArithTraits<double>
	{
	public:
		enum { IsSigned = 1, IsUnsigned = !IsSigned };
		enum { BitWide = 64 };
		static const double MAXVALUE;
		static const double MINVALUE;
		typedef double Signed;
		typedef double Unsigned;
		typedef double MaxBitsSigned;
		typedef double MaxBitsUnsigned;
	};



}

#endif  /*_TYPETRAITS_H_*/
