#include <Core/Vec4.h>

#include <cmath>

IMPGEARS_BEGIN


// -----------------------------------------------------------------------------------------------------------------------
Vec4::Vec4()
{
	_data[0] = 0.0;
	_data[1] = 0.0;
	_data[2] = 0.0;
	_data[3] = 1.0;
}

// -----------------------------------------------------------------------------------------------------------------------
Vec4::Vec4(const Vector3& v)
{
	_data[0] = v.x();
	_data[1] = v.y();
	_data[2] = v.z();
	_data[3] = 1.0;
}

// -----------------------------------------------------------------------------------------------------------------------
Vec4::Vec4(const Vec4& v)
{
	_data[0] = v[0];
	_data[1] = v[1];
	_data[2] = v[2];
	_data[3] = v[3];
}

// -----------------------------------------------------------------------------------------------------------------------
Vec4::Vec4(float x,float y, float z, float w)
{
	_data[0] = x;
	_data[1] = y;
	_data[2] = z;
	_data[3] = w;
}

// -----------------------------------------------------------------------------------------------------------------------
Vec4::~Vec4()
{
}

// -----------------------------------------------------------------------------------------------------------------------
Vec4::operator Vector3() const
{
	return Vector3(_data[0],_data[1],_data[2]);
}

// -----------------------------------------------------------------------------------------------------------------------
void Vec4::uniformize()
{
	_data[0]/=_data[3];
	_data[1]/=_data[3];
	_data[2]/=_data[3];
	_data[3]/=_data[3];
}

// -----------------------------------------------------------------------------------------------------------------------
float Vec4::dot(const Vec4& other) const
{
	return other._data[0]*_data[0] + other[1]*_data[1] + other[2]*_data[2] + other[3]*_data[3];
}

// -----------------------------------------------------------------------------------------------------------------------
Vec4 Vec4::cross(const Vec4& other) const
{
	// TODO : implementation
	return other;
}

// -----------------------------------------------------------------------------------------------------------------------
Vec4 Vec4::operator*(float scalar) const
{
	Vec4 res;
	res[0] = _data[0]*scalar;
	res[1] = _data[1]*scalar;
	res[2] = _data[2]*scalar;
    res[3] = _data[3]*scalar;
	return res;
}

// -----------------------------------------------------------------------------------------------------------------------
Vec4& Vec4::operator=(const Vec4& other)
{
	_data[0] = other[0];
	_data[1] = other[1];
	_data[2] = other[2];
	_data[3] = other[3];
	
	return *this;
}

// -----------------------------------------------------------------------------------------------------------------------
Vec4 Vec4::operator+(const Vec4& other) const
{
	Vec4 res;
	res[0] = _data[0]+other[0];
	res[1] = _data[1]+other[1];
	res[2] = _data[2]+other[2];
    res[3] = _data[3]+other[3];
	return res;
}

// -----------------------------------------------------------------------------------------------------------------------
Vec4 Vec4::operator-(const Vec4& other) const
{
	Vec4 res;
	res[0] = _data[0]-other[0];
	res[1] = _data[1]-other[1];
	res[2] = _data[2]-other[2];
    res[3] = _data[3]-other[3];
	return res;
}

// -----------------------------------------------------------------------------------------------------------------------
Vec4 Vec4::operator*(const Vec4& other) const
{
	Vec4 res;
	res[0] = _data[0]*other[0];
	res[1] = _data[1]*other[1];
	res[2] = _data[2]*other[2];
    res[3] = _data[3]*other[3];
	return res;
}

// -----------------------------------------------------------------------------------------------------------------------
bool Vec4::operator==(const Vec4& other) const
{
	return _data[0]==other[0]
		&& _data[1]==other[1]
		&& _data[2]==other[2]
		&& _data[3]==other[3];
}

// -----------------------------------------------------------------------------------------------------------------------
void Vec4::set(float x, float y, float z, float w)
{
	_data[0] = x;
	_data[1] = y;
	_data[2] = z;
	_data[3] = z;
}

// -----------------------------------------------------------------------------------------------------------------------
float& Vec4::operator[](unsigned int i)
{
	return _data[i];
}

// -----------------------------------------------------------------------------------------------------------------------
float Vec4::operator[](unsigned int i) const
{
	return _data[i];
}

// -----------------------------------------------------------------------------------------------------------------------
Vec4 Vec4::operator*(const Matrix4& mat4) const
{
	const float* md = mat4.getData();
	  
	Vec4 res( dot(Vec4(md[0],md[4],md[8],md[12])),
		dot(Vec4(md[1],md[5],md[9],md[13])),
		dot(Vec4(md[2],md[6],md[10],md[14])),
		dot(Vec4(md[3],md[7],md[11],md[15])));
  
	res.uniformize();
  
	return res;
}

// -----------------------------------------------------------------------------------------------------------------------
float Vec4::length() const
{
	return std::sqrt(length2());
}

// -----------------------------------------------------------------------------------------------------------------------
float Vec4::length2() const
{
	return dot(*this);
}

// -----------------------------------------------------------------------------------------------------------------------
void Vec4::normalize()
{
	float len = length();
	
	_data[0] /= len;
	_data[1] /= len;
	_data[2] /= len;
	_data[3] /= len;
}

IMPGEARS_END
