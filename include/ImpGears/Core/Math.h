#ifndef IMP_MATH_H
#define IMP_MATH_H

#include <Core/Object.h>
#include <Core/Vec.h>

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

template<int Dim,typename Ty>
IMP_API Vec<Dim,Ty> clamp(const Vec<Dim,Ty>& x, Ty edge0 = (Ty)0.0, Ty edge1 = (Ty)1.0)
{
	Vec<Dim,Ty> r;
	for(int i=0;i<Dim;++i)r[i]=std::min(std::max(edge0,x[i]),edge1);
	return r;
}

template<int Dim,typename Ty>
IMP_API Vec<Dim,Ty> mix(const Vec<Dim,Ty>& a, const Vec<Dim,Ty>& b, Ty delta)
{
    return (b - a)*delta + a;
}

template<typename Ty>
IMP_API Ty step(Ty edge, Ty x)
{
    return x<edge?(Ty)0:(Ty)1;
}

template<typename Ty>
IMP_API Ty linearstep(Ty edge0, Ty edge1, Ty x)
{
    return (x-edge0)/(edge1-edge0);
}

template<typename Ty>
IMP_API Ty mix(const Ty& a, const Ty& b, Ty delta)
{
    return (b - a)*delta + a;
}

template <typename Ty>
IMP_API void swap(Ty& a, Ty& b)
{
    Ty t=a; a=b; b=t;
}

template<typename Ty>
IMP_API Ty pow(const Ty& x, int p)
{
    Ty res = x;
    for(int i=0;i<p;++i) res = res * x;
    return res;
}

template<typename Ty>
IMP_API Ty min3(Ty a, Ty b, Ty c)
{
    return std::min(a,std::min(b,c));
}

template<typename Ty>
IMP_API Ty max3(Ty a, Ty b, Ty c)
{
    return std::max(a,std::max(b,c));
}



IMPGEARS_END

#endif // IMP_MATH_H
