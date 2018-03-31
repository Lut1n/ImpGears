#include <Core/Vector3.h>

#include <cmath>

IMPGEARS_BEGIN

//--------------------------------------------------------------
Vector3::Vector3()
{
	_data[0] = _data[1] = _data[2] = 0.0;
}
	
//--------------------------------------------------------------
Vector3::Vector3(const float* buf)
{
	_data[0] = buf[0];
	_data[1] = buf[1];
	_data[2] = buf[2];
}

//--------------------------------------------------------------
Vector3::Vector3(float x, float y, float z)
{
	_data[0] = x;
	_data[1] = y;
	_data[2] = z;
}

//--------------------------------------------------------------
Vector3::Vector3(const Vector3& other)
{
	_data[0] = other[0];
	_data[1] = other[1];
	_data[2] = other[2];
}

//--------------------------------------------------------------
Vector3::~Vector3()
{
}

//--------------------------------------------------------------
void Vector3::setRadial(float theta, float phi, float norm)
{
    _data[0] = norm * std::cos(phi) * std::cos(theta);
    _data[1] = norm * std::cos(phi) * std::sin(theta);
    _data[2] = norm * std::sin(phi);
}

//--------------------------------------------------------------
float& Vector3::x() {return _data[0]; }
float& Vector3::y() {return _data[1]; }
float& Vector3::z() {return _data[2]; }

//--------------------------------------------------------------
float Vector3::x() const {return _data[0]; }
float Vector3::y() const {return _data[1]; }
float Vector3::z() const {return _data[2]; }

//--------------------------------------------------------------
void Vector3::set(float x, float y, float z)
{
	_data[0] = x;
	_data[1] = y;
	_data[2] = z;
}

//--------------------------------------------------------------
Vector3& Vector3::operator*=(const float scalar)
{
    _data[0] *= scalar;
    _data[1] *= scalar;
    _data[2] *= scalar;

    return *this;
}

//--------------------------------------------------------------
Vector3 Vector3::operator*(const float scalar) const
{
	Vector3 res(*this); res *= scalar;
    return res;
}

//--------------------------------------------------------------
Vector3 Vector3::operator/(const float scalar) const
{
    return Vector3(_data[0]/scalar, _data[1]/scalar, _data[2]/scalar);
}

//--------------------------------------------------------------
Vector3& Vector3::operator=(const Vector3& other)
{
    _data[0] = other[0];
    _data[1] = other[1];
    _data[2] = other[2];

    return *this;
}

//--------------------------------------------------------------
Vector3& Vector3::operator+=(const Vector3& other)
{
    _data[0] += other[0];
    _data[1] += other[1];
    _data[2] += other[2];

    return *this;
}

//--------------------------------------------------------------
Vector3& Vector3::operator-=(const Vector3& other)
{
    _data[0] -= other[0];
    _data[1] -= other[1];
    _data[2] -= other[2];

    return *this;
}

//--------------------------------------------------------------
Vector3& Vector3::operator*=(const Vector3& other)
{
    _data[0] *= other[0];
    _data[1] *= other[1];
    _data[2] *= other[2];

    return *this;
}

//--------------------------------------------------------------
Vector3 Vector3::operator+(const Vector3& other) const
{
	Vector3 res(*this); res += other;
    return res;
}

//--------------------------------------------------------------
Vector3 Vector3::operator-(const Vector3& other) const
{
	Vector3 res(*this); res -= other;
    return res;
}

//--------------------------------------------------------------
Vector3 Vector3::operator*(const Vector3& other) const
{
	Vector3 res(*this); res *= other;
    return res;
}

/*
//--------------------------------------------------------------
Vector3& Vector3::operator*=(const Matrix3& matrix)
{
}

//--------------------------------------------------------------
Vector3 Vector3::operator*(const Matrix3& matrix) const
{
}*/

//--------------------------------------------------------------
bool Vector3::operator==(const Vector3& other) const
{
	return _data[0]==other[0] && _data[1]==other[1] && _data[2]==other[2];
}
		
//--------------------------------------------------------------
float& Vector3::operator[](unsigned int i)
{
	return _data[i];
}

//--------------------------------------------------------------
float Vector3::operator[](unsigned int i) const
{
	return _data[i];
}

//--------------------------------------------------------------
void Vector3::rotX(float rx)
{
    /*
    rx =    |   1         0         0     |
            |   0         std::cos     -std::sin    |
            |   0         std::sin     std::cos     |
    */

    float ny = _data[1]*std::cos(rx) - _data[2]*std::sin(rx);
    float nz = _data[1]*std::sin(rx) + _data[2]*std::cos(rx);
	
	_data[1]  =ny;
	_data[2] = nz;
}

//--------------------------------------------------------------
void Vector3::rotY(float ry)
{
    /*
    ry =    |   std::cos     0         std::sin     |
            |   0         1         0     |
            |   -std::sin     0        std::cos     |
    */

	float nx = _data[0]*std::cos(ry) - _data[2]*std::sin(ry);
	float nz = _data[0]*std::sin(ry) + _data[2]*std::cos(ry);
	
	_data[0]  =nx;
	_data[2] = nz;
}

//--------------------------------------------------------------
void Vector3::rotZ(float rz)
{
    /*
    rz =    |   std::cos     -std::sin    0         |
            |   std::sin     std::cos     0         |
            |   0         0        1      |
    */

    float nx = _data[0]*std::cos(rz) - _data[1]*std::sin(rz);
    float ny = _data[0]*std::sin(rz) + _data[1]*std::cos(rz);
	
	_data[0] = nx;
	_data[1] = ny;
}

//--------------------------------------------------------------
float Vector3::dot(const Vector3& other) const
{
    return _data[0]*other[0] + _data[1]*other[1] + _data[2]*other[2];
}

//--------------------------------------------------------------
Vector3 Vector3::cross(const Vector3& other) const
{
    /*
    s1  =   u2.v3   -   u3.v2
    s2  =   u3.v1   -   u1.v3
    s3  =   u1.v2   -   u2.v1
    */
    return Vector3(
               _data[1]*other[2] - _data[2]*other[1],
               _data[2]*other[0] - _data[0]*other[2],
               _data[0]*other[1] - _data[1]*other[0]);
}

//--------------------------------------------------------------
float Vector3::length() const
{
    return std::sqrt(length2());
}

//--------------------------------------------------------------
float Vector3::length2() const
{
    return dot(*this);
}

//--------------------------------------------------------------
void Vector3::normalize()
{
    float len = length();

    if(len == 0.0)
	{
        _data[0] = 1.0;
        return;
    }

    _data[0] = _data[0]/len;
    _data[1] = _data[1]/len;
    _data[2] = _data[2]/len;
}

IMPGEARS_END
