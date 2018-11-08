#ifndef IMP_VEC3_H
#define IMP_VEC3_H

#include <Core/Vec.h>
#include <Core/Matrix3.h>

#include <cmath>

IMPGEARS_BEGIN

class IMP_API Vec3 : public Vec<3,float>
{
	public:
	
	Meta_Class(Vec3)
	
	Vec3() : Vec() {}
	Vec3(float v) : Vec(v) {}
	Vec3(float x, float y, float z) : Vec(x,y,z) {}
	Vec3(const Vec& v) : Vec(v) {}
	Vec3(const float* buf) : Vec(buf) {}

	void set(float x, float y, float z) { Vec::set(x,y,z); }
	void setRadial(float theta, float phi) { /*todo*/ }
	
	const Vec3& operator*=(const Vec3& other) { Vec::operator*=(other); return *this; }
	
	const Vec3& operator*=(const Matrix3& mat3)
	{ 
		set(x()*mat3(0,0) + y()*mat3(1,0) + z()*mat3(2,0),
			x()*mat3(0,1) + y()*mat3(1,1) + z()*mat3(2,1),
			x()*mat3(0,2) + y()*mat3(1,2) + z()*mat3(2,2));

		return *this;
	}
	
	Vec3 operator*(const Vec3& other) const { return Vec::operator*(other); }
	
	Vec3 operator*(float scalar) const { return Vec::operator*(scalar); }
	
	Vec3 operator*(const Matrix3& mat3)
	{
		Vec3 result(*this);
		result *= mat3;
		return result;
	}
	
	Vec3 cross(const Vec3& v) const
	{
		return Vec3(
			_data[1]*v[2] - _data[2]*v[1],
			_data[2]*v[0] - _data[0]*v[2],
			_data[0]*v[1] - _data[1]*v[0]);
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
