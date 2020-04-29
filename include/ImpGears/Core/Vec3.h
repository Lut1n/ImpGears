#ifndef IMP_VEC3_H
#define IMP_VEC3_H

#include <ImpGears/Core/Vec.h>

#include <cmath>

IMPGEARS_BEGIN

class IMP_API Vec3 : public Vec<3,float>
{
	public:
	
	Meta_Class(Vec3)
	
	static const Vec3 X;
	static const Vec3 Y;
	static const Vec3 Z;
	
	Vec3() : Vec() {}
	Vec3(float v) : Vec(v) {}
	Vec3(float x, float y, float z) : Vec(x,y,z) {}
	Vec3(const float* buf) : Vec(buf) {}
	
	template<int Dim2>
	Vec3(const Vec<Dim2,float>& v, float dv=1) : Vec<3,float>(v,dv) {}
	
	template<int Dim2>
	const Vec3& operator=(const Vec<Dim2,float>& other) { Vec<3,float>::operator=(other); return *this; }
	
	void set(float x, float y, float z) { Vec<3,float>::set(x,y,z); }
	
	Vec3 cross(const Vec3& v) const
	{
		return Vec3(
			_data[1]*v[2] - _data[2]*v[1],
			_data[2]*v[0] - _data[0]*v[2],
			_data[0]*v[1] - _data[1]*v[0]);
	}
	
	float angleFrom(Vec3 ref, Vec3 axe = Z)
	{
		ref.normalize();
		Vec3 btn = axe.cross(ref);
		return std::atan2(dot(btn),dot(ref));
	}
	
	float x() const { return _data[0]; }
	float y() const { return _data[1]; }
	float z() const { return _data[2]; }
	
	float& x() { return _data[0]; }
	float& y() { return _data[1]; }
	float& z() { return _data[2]; }
};

IMPGEARS_END

#endif // IMP_VEC3_H
