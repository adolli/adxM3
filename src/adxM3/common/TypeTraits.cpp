
#include "TypeTraits.h"
using namespace adxM3;

const float ArithTraits<float>::MAXVALUE = 3.402823466e+38F;
const float ArithTraits<float>::MINVALUE = -ArithTraits<float>::MAXVALUE;
const double ArithTraits<double>::MAXVALUE = 1.7976931348623158e+308;
const double ArithTraits<double>::MINVALUE = -ArithTraits<double>::MAXVALUE;
