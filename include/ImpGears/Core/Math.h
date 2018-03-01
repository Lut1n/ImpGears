#ifndef IMP_MATH_H
#define IMP_MATH_H

#include <Core/Object.h>

IMPGEARS_BEGIN

IMP_API void RandSeed(double s = 0.0);

IMP_API double Rand(double min, double max, double precision = 1.0e-9);

IMP_API double Frac(double t);

IMP_API double Hermite(double t);

IMP_API double Quintic(double t);

IMP_API double Lerp(double a, double b, double t);

IMP_API double Sin(double t, double maxPeriodRatio = 0.5);

IMP_API double SquareSignal(double t, double maxPeriodRatio = 0.5);

IMP_API double TriangleSignal(double t, double maxPeriodRatio = 0.5);


IMPGEARS_END

#endif // IMP_MATH_H
