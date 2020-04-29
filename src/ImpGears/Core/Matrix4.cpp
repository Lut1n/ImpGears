#include <ImpGears/Core/Matrix4.h>
#include <cstring>

#include <cmath>

IMPGEARS_BEGIN

//--------------------------------------------------------------
Matrix4::Matrix4()
	: Matrix<4,4,float>()
{
}

//--------------------------------------------------------------
Matrix4::Matrix4(const float* buf, bool transp)
	: Matrix<4,4,float>(buf,transp)
{
}

//--------------------------------------------------------------
Matrix4::Matrix4(const Vec4& c1,const Vec4& c2,const Vec4& c3,const Vec4& c4)
{
	at(0,0)=c1[0]; at(0,1)=c1[1]; at(0,2)=c1[2]; at(0,3)=c1[3];
	at(1,0)=c2[0]; at(1,1)=c2[1]; at(1,2)=c2[2]; at(1,3)=c2[3];
	at(2,0)=c3[0]; at(2,1)=c3[1]; at(2,2)=c3[2]; at(2,3)=c3[3];
	at(3,0)=c4[0]; at(3,1)=c4[1]; at(3,2)=c4[2]; at(3,3)=c4[3];
}

//--------------------------------------------------------------
Matrix4::~Matrix4()
{
}

//--------------------------------------------------------------
float Matrix4::det() const
{
	float result = 
	at(0,0)*at(1,1)*at(2,2)*at(3,3) + at(0,0)*at(2,1)*at(3,2)*at(1,3) + at(0,0)*at(3,1)*at(1,2)*at(2,3)
	+ at(1,0)*at(0,1)*at(3,2)*at(2,3) + at(1,0)*at(2,1)*at(0,2)*at(3,3) + at(1,0)*at(3,1)*at(2,2)*at(0,3)
	+ at(2,0)*at(0,1)*at(1,2)*at(3,3) + at(2,0)*at(1,1)*at(3,2)*at(0,3) + at(2,0)*at(3,1)*at(0,2)*at(1,3)
	+ at(3,0)*at(0,1)*at(2,2)*at(1,3) + at(3,0)*at(1,1)*at(0,2)*at(2,3) + at(3,0)*at(2,1)*at(1,2)*at(0,3)
	- at(0,0)*at(1,1)*at(3,2)*at(2,3) - at(0,0)*at(2,1)*at(1,2)*at(3,3) - at(0,0)*at(3,1)*at(2,2)*at(1,3)
	- at(1,0)*at(0,1)*at(2,2)*at(3,3) - at(1,0)*at(2,1)*at(3,2)*at(0,3) - at(1,0)*at(3,1)*at(0,2)*at(2,3)
	- at(2,0)*at(0,1)*at(3,2)*at(1,3) - at(2,0)*at(1,1)*at(0,2)*at(3,3) - at(2,0)*at(3,1)*at(1,2)*at(0,3)
	- at(3,0)*at(0,1)*at(1,2)*at(2,3) - at(3,0)*at(1,1)*at(2,2)*at(0,3) - at(3,0)*at(2,1)*at(0,2)*at(1,3);

	return result;
}

//--------------------------------------------------------------
Matrix4 Matrix4::inverse() const
{
	Matrix4 mat;

	mat(0,0) = at(1,1)*at(2,2)*at(3,3) + at(2,1)*at(3,2)*at(1,3) + at(3,1)*at(1,2)*at(2,3)
				- at(1,1)*at(3,2)*at(2,3) - at(2,1)*at(1,2)*at(3,3) - at(3,1)*at(2,2)*at(1,3);
	mat(0,1) = at(1,0)*at(3,2)*at(2,3) + at(2,0)*at(1,2)*at(3,3) + at(3,0)*at(2,2)*at(1,3)
				- at(1,0)*at(2,2)*at(3,3) - at(2,0)*at(3,2)*at(1,3) - at(3,0)*at(1,2)*at(2,3);
	mat(0,2) = at(1,0)*at(2,1)*at(3,3) + at(2,0)*at(3,1)*at(1,3) + at(3,0)*at(1,1)*at(2,3)
				- at(1,0)*at(3,1)*at(2,3) - at(2,0)*at(1,1)*at(3,3) - at(3,0)*at(2,1)*at(1,3);
	mat(0,3) = at(1,0)*at(3,1)*at(2,2) + at(2,0)*at(1,1)*at(3,2) + at(3,0)*at(2,1)*at(1,2)
				- at(1,0)*at(2,1)*at(3,2) - at(2,0)*at(3,1)*at(1,2) - at(3,0)*at(1,1)*at(2,2);
				
	mat(1,0) = at(0,1)*at(3,2)*at(2,3) + at(2,1)*at(0,2)*at(3,3) + at(3,1)*at(2,2)*at(0,3)
				- at(0,1)*at(2,2)*at(3,3) - at(2,1)*at(3,2)*at(0,3) - at(3,1)*at(0,2)*at(2,3);
	mat(1,1) = at(0,0)*at(2,2)*at(3,3) + at(2,0)*at(3,2)*at(0,3) + at(3,0)*at(0,2)*at(2,3)
				- at(0,0)*at(3,2)*at(2,3) - at(2,0)*at(0,2)*at(3,3) - at(3,0)*at(2,2)*at(0,3);
	mat(1,2) = at(0,0)*at(3,1)*at(2,3) + at(2,0)*at(0,1)*at(3,3) + at(3,0)*at(2,1)*at(0,3)
				- at(0,0)*at(2,1)*at(3,3) - at(2,0)*at(3,1)*at(0,3) - at(3,0)*at(0,1)*at(2,3);
	mat(1,3) = at(0,0)*at(2,1)*at(3,2) + at(2,0)*at(3,1)*at(0,2) + at(3,0)*at(0,1)*at(2,2)
				- at(0,0)*at(3,1)*at(2,2) - at(2,0)*at(0,1)*at(3,2) - at(3,0)*at(2,1)*at(0,2);
				
	mat(2,0) = at(0,1)*at(1,2)*at(3,3) + at(1,1)*at(3,2)*at(0,3) + at(3,1)*at(0,2)*at(1,3)
				- at(0,1)*at(3,2)*at(1,3) - at(1,1)*at(0,2)*at(3,3) - at(3,1)*at(1,2)*at(0,3);
	mat(2,1) = at(0,0)*at(3,2)*at(1,3) + at(1,0)*at(0,2)*at(3,3) + at(3,0)*at(1,2)*at(0,3)
				- at(0,0)*at(1,2)*at(3,3) - at(1,0)*at(3,2)*at(0,3) - at(3,0)*at(0,2)*at(1,3);
	mat(2,2) = at(0,0)*at(1,1)*at(3,3) + at(1,0)*at(3,1)*at(0,3) + at(3,0)*at(0,1)*at(1,3)
				- at(0,0)*at(3,1)*at(1,3) - at(1,0)*at(0,1)*at(3,3) - at(3,0)*at(1,1)*at(0,3);
	mat(2,3) = at(0,0)*at(3,1)*at(1,2) + at(1,0)*at(0,1)*at(3,2) + at(3,0)*at(1,1)*at(0,2)
				- at(0,0)*at(1,1)*at(3,2) - at(1,0)*at(3,1)*at(0,2) - at(3,0)*at(0,1)*at(1,2);
				
	mat(3,0) = at(0,1)*at(2,2)*at(1,3) + at(1,1)*at(0,2)*at(2,3) + at(2,1)*at(1,2)*at(0,3)
				- at(0,1)*at(1,2)*at(2,3) - at(1,1)*at(2,2)*at(0,3) - at(2,1)*at(0,2)*at(1,3);
	mat(3,1) = at(0,0)*at(1,2)*at(2,3) + at(1,0)*at(2,2)*at(0,3) + at(2,0)*at(0,2)*at(1,3)
				- at(0,0)*at(1,1)*at(2,3) - at(1,0)*at(2,1)*at(0,3) - at(2,0)*at(0,1)*at(1,3);
	mat(3,2) = at(0,0)*at(2,1)*at(1,3) + at(1,0)*at(0,1)*at(2,3) + at(2,0)*at(1,1)*at(0,3)
				- at(0,0)*at(1,1)*at(2,3) - at(1,0)*at(2,1)*at(0,3) - at(2,0)*at(0,1)*at(1,3);
	mat(3,3) = at(0,0)*at(1,1)*at(2,2) + at(1,0)*at(2,1)*at(0,2) + at(2,0)*at(0,1)*at(1,2)
				- at(0,0)*at(2,1)*at(1,2) - at(1,0)*at(0,1)*at(2,2) - at(2,0)*at(1,1)*at(0,2);

	return mat.transpose() * (1.f/det());
}

//--------------------------------------------------------------
Matrix4 Matrix4::perspectiveProj(float fovx, float whRatio, float nearValue, float farValue)
{
	fovx *= 3.14159f/180.f;
	fovx /= 2.f;

	const float projX = 1.f/(tanf(fovx));
	const float projY = projX * whRatio;
	const float A = - (farValue+nearValue) / (farValue-nearValue);
	const float B = -2.f * (farValue*nearValue) / (farValue-nearValue);

	const float data[16] = {
		projX,         0.f,           0.f,          0.f,
		0.f,           projY,         0.f,          0.f,
		0.f,           0.f,           A,            B,
		0.f,           0.f,           -1.f,         0.f
	};

	return Matrix4(data, true);
}

//--------------------------------------------------------------
Matrix4 Matrix4::orthographicProj(float l, float r, float b, float t, float nearValue, float farValue)
{
	const float projX = 2.f/(r-l);
	const float projY = 2.f/(t-b);
	const float projZ = -2/(farValue-nearValue);
	const float tX = -(r+l)/(r-l);
	const float tY = -(t+b)/(t-b);
	const float tZ = -(farValue+nearValue)/(farValue-nearValue);

	const float data[16] = {
		projX,    0.f,      0.f,        tX,
		0.f,      projY,    0.f,        tY,
		0.f,      0.f,      projZ,      tZ,
		0.f,      0.f,      0.f,        1.f
	};

	return Matrix4(data, true);
}

//--------------------------------------------------------------
Matrix4 Matrix4::orthographicProj(float width, float height, float nearValue, float farValue)
{
	const float projX = 2.f/width;
	const float projY = 2.f/height;
	const float projZ = -2/(farValue-nearValue);
	const float tZ = -(farValue+nearValue)/(farValue-nearValue);

	const float data[16] = {
		projX,    0.f,      0.f,        0.f,
		0.f,      projY,    0.f,        0.f,
		0.f,      0.f,      projZ,      tZ,
		0.f,      0.f,      0.f,        1.f
	};

	return Matrix4(data, true);
}

//--------------------------------------------------------------
Matrix4 Matrix4::view(const Vec3& pos, const Vec3& target, const Vec3& up)
{
	Vec3 zAxis = pos-target; zAxis.normalize();
	Vec3 xAxis = up.cross(zAxis); xAxis.normalize();
	Vec3 yAxis = zAxis.cross(xAxis);

	const float data[16] = {
		xAxis.x(),    xAxis.y(),    xAxis.z(),    -xAxis.dot(pos),
		yAxis.x(),    yAxis.y(),    yAxis.z(),    -yAxis.dot(pos),
		zAxis.x(),    zAxis.y(),    zAxis.z(),    -zAxis.dot(pos),
		0.0,          0.0,          0.0,          1.f
	};

	return Matrix4(data,true);
}

//--------------------------------------------------------------
Matrix4 Matrix4::translation(float tx, float ty, float tz)
{
	const float data[16] = {
		1.f,    0.f,    0.f,    tx,
		0.f,    1.f,    0.f,    ty,
		0.f,    0.f,    1.f,    tz,
		0.f,    0.f,    0.f,    1.f
	};

	return Matrix4(data, true);
}

//--------------------------------------------------------------
Matrix4 Matrix4::rotationX(float rx)
{
	float c = std::cos(rx);
	float s = std::sin(rx);

	const float data[16] = {
		1.f,    0.f,    0.f,    0.f,
		0.f,    c,      -s,     0.f,
		0.f,    s,      c,      0.f,
		0.f,    0.f,    0.f,    1.f
	};

	return Matrix4(data, true);
}


//--------------------------------------------------------------
Matrix4 Matrix4::rotationY(float ry)
{
	float c = std::cos(ry);
	float s = std::sin(ry);

	const float data[16] = {
		c,      0.f,    s,      0.f,
		0.f,    1.f,    0.f,    0.f,
		-s,     0.f,    c,      0.f,
		0.f,    0.f,    0.f,    1.f
	};

	return Matrix4(data, true);
}


//--------------------------------------------------------------
Matrix4 Matrix4::rotationZ(float rz)
{
	float c = std::cos(rz);
	float s = std::sin(rz);

	const float data[16] = {
		c,      -s,     0.f,    0.f,
		s,      c,      0.f,    0.f,
		0.f,    0.f,    1.f,    0.f,
		0.f,    0.f,    0.f,    1.f
	};

	return Matrix4(data, true);
}

//--------------------------------------------------------------
Matrix4 Matrix4::scale(float sx, float sy, float sz)
{
	const float data[16] = {
		sx,     0.f,    0.f,    0.f,
		0.f,    sy,     0.f,    0.f,
		0.f,    0.f,    sz,     0.f,
		0.f,    0.f,    0.f,    1.f
	};

	return Matrix4(data);
}

//--------------------------------------------------------------
Matrix4 Matrix4::translation(const Vec3& t)
{
	return translation(t[0],t[1],t[2]);
}

//--------------------------------------------------------------
Matrix4 Matrix4::rotationXYZ(const Vec3& r)
{
	return rotationX(r[0]) * rotationY(r[1]) * rotationZ(r[2]);
}

//--------------------------------------------------------------
Matrix4 Matrix4::scale(const Vec3& s)
{
	return scale(s[0],s[1],s[2]);
}

IMPGEARS_END
