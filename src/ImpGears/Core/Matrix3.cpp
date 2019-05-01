#include <Core/Matrix3.h>

#include <cstring>
#include <cmath>

#include <Core/Matrix4.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
Matrix3::Matrix3()
	: Matrix<3,3,float>()
{
}

//--------------------------------------------------------------
Matrix3::Matrix3(const float* buf, bool transp)
	: Matrix<3,3,float>(buf,transp)
{
}

//--------------------------------------------------------------
Matrix3::Matrix3(const Vec3& c1,const Vec3& c2,const Vec3& c3)
{
	at(0,0)=c1[0]; at(0,1)=c1[1]; at(0,2)=c1[2];
	at(1,0)=c2[0]; at(1,1)=c2[1]; at(1,2)=c2[2];
	at(2,0)=c3[0]; at(2,1)=c3[1]; at(2,2)=c3[2];
}

//--------------------------------------------------------------
Matrix3::~Matrix3()
{
}

//--------------------------------------------------------------
Matrix3& Matrix3::operator+=(const Matrix3& other)
{
    for(int i=0; i<9; ++i) _data[i] += other._data[i];
    return *this;
}

//--------------------------------------------------------------
Matrix3& Matrix3::operator-=(const Matrix3& other)
{
	for(int i=0; i<9; ++i) _data[i] -= other._data[i];
	return *this;
}

//--------------------------------------------------------------
Matrix3 Matrix3::operator+(const Matrix3& other) const
{
	return Matrix3(*this) += other;
}

//--------------------------------------------------------------
Matrix3 Matrix3::operator-(const Matrix3& other) const
{
	return Matrix3(*this) -= other;
}

//--------------------------------------------------------------
float Matrix3::det() const
{
	float result =
	at(0,0)*at(1,1)*at(2,2)
	+ at(1,0)*at(2,1)*at(0,2)
	+ at(2,0)*at(0,1)*at(1,2)
	- at(2,0)*at(1,1)*at(0,2)
	- at(2,1)*at(1,2)*at(0,0)
	- at(2,2)*at(1,0)*at(0,1);

	return result;
}

//--------------------------------------------------------------
Matrix3 Matrix3::inverse() const
{
	Matrix3 mat;
	mat(0,0) = at(1,1)*at(2,2) - at(2,1)*at(1,2);
	mat(0,1) = at(2,0)*at(1,2) - at(1,0)*at(2,2);
	mat(0,2) = at(1,0)*at(2,1) - at(2,0)*at(1,1);
	
	mat(1,0) = at(2,1)*at(0,2) - at(0,1)*at(2,2);
	mat(1,1) = at(0,0)*at(2,2) - at(2,0)*at(0,2);
	mat(1,2) = at(2,0)*at(0,1) - at(0,0)*at(2,1);
	
	mat(2,0) = at(0,1)*at(1,2) - at(1,1)*at(0,2);
	mat(2,1) = at(1,0)*at(0,2) - at(0,0)*at(1,2);
	mat(2,2) = at(0,0)*at(1,1) - at(1,0)*at(0,1);
	
	return mat.transpose() * (1.f/det());
}

//--------------------------------------------------------------
Matrix3 Matrix3::rotationX(float rad)
{
	float c = std::cos(rad);
	float s = std::sin(rad);
	
	const float values[9] = { 1.f,0.f,0.f,0.f,c,-s,0.f,s,c};
	return Matrix3(values,true);
}

//--------------------------------------------------------------
Matrix3 Matrix3::rotationY(float rad)
{
	float c = std::cos(rad);
	float s = std::sin(rad);
	
	const float values[9] = { c,0.f,s,0.f,1.f,0.f,-s,0.f,c};
	return Matrix3(values,true);
}

//--------------------------------------------------------------
Matrix3 Matrix3::rotationZ(float rad)
{
	float c = std::cos(rad);
	float s = std::sin(rad);
	
	const float values[9] = { c,-s,0.f,s,c,0.f,0.f,0.f,1.f};
	return Matrix3(values,true);
}

//--------------------------------------------------------------
Matrix3 Matrix3::rotationXYZ(const Vec3& r)
{
	return rotationX(r[0]) * rotationY(r[1]) * rotationZ(r[2]);
}

//--------------------------------------------------------------
Matrix3 Matrix3::scale(float sx, float sy, float sz)
{
	const float data[9] =
	{
		sx,     0.f,    0.f,
		0.f,    sy,     0.f,
		0.f,    0.f,    sz
	};

	return Matrix3(data);
}

//--------------------------------------------------------------
Matrix3 Matrix3::scale(const Vec3& s)
{
	return scale(s[0],s[1],s[2]);
}

IMPGEARS_END
