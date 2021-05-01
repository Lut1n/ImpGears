#ifndef IMP_VEC2_H
#define IMP_VEC2_H

#include <ImpGears/Core/Vec.h>
#include <ImpGears/Core/Export.h>

#include <cmath>

IMPGEARS_BEGIN

class IG_CORE_API Vec2 : public Vec<2,float>
{
	public:
	
	Meta_Class(Vec2)
	
	static const Vec2 X;
	static const Vec2 Y;
	
	Vec2() : Vec() {}
	Vec2(float v) : Vec(v) {}
	Vec2(float x, float y) : Vec(x,y) {}
	Vec2(const float* buf) : Vec(buf) {}
	
	template<int Dim2>
	Vec2(const Vec<Dim2,float>& v, float dv=1) : Vec<2,float>(v,dv) {}
	
	template<int Dim2>
	const Vec2& operator=(const Vec<Dim2,float>& other) { Vec<2,float>::operator=(other); return *this; }
	
	void set(float x, float y) { Vec<2,float>::set(x,y); }
	
	float x() const { return _data[0]; }
	float y() const { return _data[1]; }
	
	float& x() { return _data[0]; }
	float& y() { return _data[1]; }
};

IMPGEARS_END

#endif // IMP_VEC2_H
