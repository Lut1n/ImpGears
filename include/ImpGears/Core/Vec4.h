#ifndef IMP_VEC4_H
#define IMP_VEC4_H

#include <Core/Vec.h>
#include <Core/Vec3.h>
#include <Core/Matrix4.h>

IMPGEARS_BEGIN

class IMP_API Vec4 : public Vec<4,float>
{
	public:
	
	Meta_Class(Vec4)
	
	Vec4() : Vec() {}
	Vec4(float v) : Vec(v) {}
	Vec4(float x, float y, float z, float w) : Vec(x,y,z,w) {}
	Vec4(const Vec& v) : Vec(v) {}
	Vec4(const float* buf) : Vec(buf) {}
	
	void set(float x, float y, float z, float w) { Vec::set(x,y,z,w); }
	
	float x() const { return _data[0]; }
	float y() const { return _data[1]; }
	float z() const { return _data[2]; }
	float w() const { return _data[3]; }
	
	float& x() { return _data[0]; }
	float& y() { return _data[1]; }
	float& z() { return _data[2]; }
	float& w() { return _data[3]; }
	
	const Vec4& operator*=(const Vec4& other) { Vec::operator*=(other); return *this; }
	
	const Vec4& operator*=(const Matrix4& mat4)
	{ 
		set(x()*mat4(0,0) + y()*mat4(1,0) + z()*mat4(2,0) + w()*mat4(3,0),
			x()*mat4(0,1) + y()*mat4(1,1) + z()*mat4(2,1) + w()*mat4(3,1),
			x()*mat4(0,2) + y()*mat4(1,2) + z()*mat4(2,2) + w()*mat4(3,2),
			x()*mat4(0,3) + y()*mat4(1,3) + z()*mat4(2,3) + w()*mat4(3,3));

		Vec::operator/=(w());
		
		return *this;
	}
	
	Vec4 operator*(const Vec4& other) const { return Vec::operator*(other); }
	
	Vec4 operator*(float scalar) const { return Vec::operator*(scalar); }
	
	Vec4 operator*(const Matrix4& mat4)
	{
		Vec4 result(*this);
		result *= mat4;
		return result;
	}
};

IMPGEARS_END

#endif // IMP_VEC4_H
