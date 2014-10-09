
#ifndef _ADX_ASSERT_H_
#define _ADX_ASSERT_H_

namespace adxM3
{
	template <bool> struct StaticAssert;
	template <> struct StaticAssert<true> {};
}


#define STATIC_ASSERT(exp, msg) \
	{ adxM3::StaticAssert<((exp) != 0)> ERROR_##msg; (void)ERROR_##msg; }

#endif  /*_ADX_ASSERT_H_*/
