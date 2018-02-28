#ifndef IMP_BASIC_FUNCTION_H
#define IMP_BASIC_FUNCTION_H

#include <Core/Object.h>

IMPGEARS_BEGIN

double IMP_API randHugosElias(int t);

template<typename T>
void IMP_API swap(T* c1, T* c2)
{
	T t = *c1;
	*c1 = *c2;
	*c2 = t;
}

void IMP_API intFrac(double t, int& integer, double& fraction);

double IMP_API Hermite(double t);

double IMP_API Quintic(double t);

double IMP_API Lerp(double a, double b, double t);

double IMP_API Sin(double t, double maxPeriodRatio = 0.5);

double IMP_API SquareSignal(double t, double maxPeriodRatio = 0.5);

double IMP_API TriangleSignal(double t, double maxPeriodRatio = 0.5);

IMPGEARS_END

#endif // IMP_BASIC_FUNCTION_H
