#ifndef IMP_MATH_H
#define IMP_MATH_H

#include "base/impBase.hpp"

IMPGEARS_BEGIN

void initRandom();

float random(float min, float max, float precision = 0.000001f);

float lerp(float a, float b, float value);



IMPGEARS_END

#endif // IMP_MATH_H
