#include "Math/Vector3.h"

#include <cmath>

IMPGEARS_BEGIN

//--------------------------------------------------------------
Vector3::Vector3():
    m_x(0),
    m_y(0),
    m_z(0)
{
}

//--------------------------------------------------------------
Vector3::Vector3(float x, float y, float z):
    m_x(x),
    m_y(y),
    m_z(z)
{
}

//--------------------------------------------------------------
Vector3::Vector3(const Vector3& other):
    m_x(other.m_x),
    m_y(other.m_y),
    m_z(other.m_z)
{
}

//--------------------------------------------------------------
Vector3::~Vector3()
{
}

//--------------------------------------------------------------
void Vector3::setRadial(float theta, float phi, float norm)
{
    m_x = norm * cosf(phi) * cosf(theta);
    m_y = norm * cosf(phi) * sinf(theta);
    m_z = norm * sinf(phi);
}

//--------------------------------------------------------------
Vector3& Vector3::operator*=(const float scalar)
{
    m_x *= scalar;
    m_y *= scalar;
    m_z *= scalar;

    return *this;
}

//--------------------------------------------------------------
Vector3 Vector3::operator*(const float scalar) const
{
    return Vector3(m_x*scalar, m_y*scalar, m_z*scalar);
}

//--------------------------------------------------------------
Vector3& Vector3::operator=(const Vector3& other)
{
    m_x = other.m_x;
    m_y = other.m_y;
    m_z = other.m_z;

    return *this;
}

//--------------------------------------------------------------
Vector3& Vector3::operator+=(const Vector3& other)
{
    m_x += other.m_x;
    m_y += other.m_y;
    m_z += other.m_z;

    return *this;
}

//--------------------------------------------------------------
Vector3& Vector3::operator-=(const Vector3& other)
{
    m_x -= other.m_x;
    m_y -= other.m_y;
    m_z -= other.m_z;

    return *this;
}

//--------------------------------------------------------------
Vector3 Vector3::operator+(const Vector3& other) const
{
    return Vector3(m_x+other.m_x, m_y+other.m_y, m_z+other.m_z);
}

//--------------------------------------------------------------
Vector3 Vector3::operator-(const Vector3& other) const
{
    return Vector3(m_x-other.m_x, m_y-other.m_y, m_z-other.m_z);
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
void Vector3::rotationX(float rx)
{
    /*
    rx =    |   1         0         0     |
            |   0         cos     -sin    |
            |   0         sin     cos     |
    */

    m_y = m_y*cosf(rx) - m_z*sinf(rx);
    m_z = m_y*sinf(rx) + m_z*cosf(rx);
}

//--------------------------------------------------------------
void Vector3::rotationY(float ry)
{
    /*
    ry =    |   cos     0         sin     |
            |   0         1         0     |
            |   -sin     0        cos     |
    */

    m_x = m_x*cosf(ry) - m_z*sinf(ry);
    m_z = m_y*sinf(ry) + m_z*cosf(ry);
}

//--------------------------------------------------------------
void Vector3::rotationZ(float rz)
{
    /*
    rz =    |   cos     -sin    0         |
            |   sin     cos     0         |
            |   0         0        1      |
    */

    m_x = m_x*cosf(rz) - m_y*sinf(rz);
    m_y = m_x*sinf(rz) + m_y*cosf(rz);
}

//--------------------------------------------------------------
float Vector3::dotProduct(const Vector3& other) const
{
    return m_x*other.m_x + m_y*other.m_y + m_z*other.m_z;
}

//--------------------------------------------------------------
Vector3 Vector3::crossProduct(const Vector3& other) const
{
    /*
    s1  =   u2.v3   -   u3.v2
    s2  =   u3.v1   -   u1.v3
    s3  =   u1.v2   -   u2.v1
    */
    return Vector3(
               m_y*other.m_z - m_z*other.m_y,
               m_z*other.m_x - m_x*other.m_z,
               m_x*other.m_y - m_y*other.m_x);
}

//--------------------------------------------------------------
float Vector3::getNorm() const
{
    return sqrtf(getSqNorm());
}

//--------------------------------------------------------------
float Vector3::getSqNorm() const
{
    return (m_x*m_x + m_y*m_y + m_z*m_z);///3.f;
}

//--------------------------------------------------------------
void Vector3::truncate()
{
    int i;

    i=m_x; m_x=i;
    i=m_y; m_y=i;
    i=m_z; m_z=i;
}

//--------------------------------------------------------------
void Vector3::normalize()
{
    float n = getNorm();

    if(n == 0.f){
        m_x = 1.f;
        return;
    }

    m_x = m_x/n;
    m_y = m_y/n;
    m_z = m_z/n;
}

IMPGEARS_END
