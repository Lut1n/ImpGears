#ifndef IMP_VEC4_H
#define IMP_VEC4_H

#include <Core/Vec.h>

IMPGEARS_BEGIN

class IMP_API Vec4 : public Vec<4,float>
{
	public:
	
	Meta_Class(Vec4)
	
	static const Vec4 X;
	static const Vec4 Y;
	static const Vec4 Z;
	static const Vec4 W;
	
	Vec4() : Vec() {}
	Vec4(float v) : Vec(v) {}
	Vec4(float x, float y, float z, float w) : Vec(x,y,z,w) {}
	Vec4(const float* buf) : Vec(buf) {}
	
	template<int Dim2>
	Vec4(const Vec<Dim2,float>& v, float dv=1) : Vec<4,float>(v,dv) {}
	
	template<int Dim2>
	const Vec4& operator=(const Vec<Dim2,float>& other) { Vec<4,float>::operator=(other); return *this; }
	
	void set(float x, float y, float z, float w) { Vec<4,float>::set(x,y,z,w); }
	
	float x() const { return _data[0]; }
	float y() const { return _data[1]; }
	float z() const { return _data[2]; }
	float w() const { return _data[3]; }
	
	float& x() { return _data[0]; }
	float& y() { return _data[1]; }
	float& z() { return _data[2]; }
	float& w() { return _data[3]; }
};

IMPGEARS_END

#endif // IMP_VEC4_H
