#ifndef IMP_MATH_H
#define IMP_MATH_H

#include <ImpGears/Core/Object.h>
#include <ImpGears/Core/Vec.h>

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

template<typename Ty>
Ty min(Ty a, Ty b)
{
	return a<b?a:b;
}

template<typename Ty>
Ty max(Ty a, Ty b)
{
	return a>b?a:b;
}

template<typename Ty>
Ty clamp(Ty x, Ty edge0 = (Ty)0, Ty edge1 = (Ty)1)
{
	return x>edge1?edge1:(x<edge0?edge0:x);
}

template<typename Ty>
Ty floor(Ty v)
{
	return (Ty)static_cast<long long>(v);
}

template<typename Ty>
Ty frac(Ty v)
{
	return v - floor(v);
}

template<typename Ty>
Ty smoothstep(Ty edge0, Ty edge1, float delta)
{
	delta = (delta-edge0) / (edge1-edge0);
    Ty x = clamp<Ty>(delta, 0.0, 1.0); 
    return x*x*(3.0 - 2.0*x);
}

template<int Dim,typename Ty>
Vec<Dim,Ty> dotClamp(const Vec<Dim,Ty>& x, Ty edge0 = (Ty)0.0, Ty edge1 = (Ty)1.0)
{
	Vec<Dim,Ty> r;
	for(int i=0;i<Dim;++i)r[i]=clamp<Ty>(x[i],edge0,edge1);
	return r;
}

template<int Dim,typename Ty>
Vec<Dim,Ty> dotMax(const Vec<Dim,Ty>& x, const Vec<Dim,Ty>& y)
{
	Vec<Dim,Ty> r;
	for(int i=0;i<Dim;++i)r[i]=std::max(x[i],y[i]);
	return r;
}

template<int Dim,typename Ty>
Vec<Dim,Ty> dotMin(const Vec<Dim,Ty>& x, const Vec<Dim,Ty>& y)
{
	Vec<Dim,Ty> r;
	for(int i=0;i<Dim;++i)r[i]=std::min(x[i],y[i]);
	return r;
}

template<typename Ty>
Ty step(Ty edge, Ty x)
{
    return x<edge?(Ty)0:(Ty)1;
}

template<typename Ty>
Ty linearstep(Ty edge0, Ty edge1, Ty x)
{
	Ty range = (edge1-edge0);
	if(range == (Ty)0) return (Ty)0;
    else return (x-edge0)/range;
}

template<typename Ty>
Ty mix(const Ty& a, const Ty& b, float delta)
{
    return (b - a)*delta + a;
}

template<typename Ty>
Ty mix2d(Ty oo, Ty xo, Ty oy, Ty xy, float deltaX, float deltaY)
{
	Ty lx1 = mix<Ty>(oo, xo, deltaX);
	Ty lx2 = mix<Ty>(oy, xy, deltaX);
	return mix<Ty>(lx1, lx2, deltaY);
}

template<typename Ty>
Ty pow(const Ty& x, int p)
{
    Ty res = x;
    for(int i=0;i<p;++i) res = res * x;
    return res;
}

template<typename Ty>
Ty min3(Ty a, Ty b, Ty c)
{
    return std::min(a,std::min(b,c));
}

template<typename Ty>
Ty max3(Ty a, Ty b, Ty c)
{
    return std::max(a,std::max(b,c));
}



IMPGEARS_END

#endif // IMP_MATH_H
