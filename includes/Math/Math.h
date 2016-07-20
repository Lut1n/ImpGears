#ifndef IMP_MATH_H
#define IMP_MATH_H

#include "Core/impBase.h"

IMPGEARS_BEGIN

IMP_API void initRandom();

IMP_API float random(float min, float max, float precision = 0.000001f);

IMP_API float lerp(float a, float b, float value);



IMPGEARS_END

#endif // IMP_MATH_H
