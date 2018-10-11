#ifndef IMP_VEC3_H
#define IMP_VEC3_H

#include <Core/Vec.h>

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
	
	void rotX(float a) {}
	void rotY(float a) {}
	void rotZ(float a) {}
	
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
