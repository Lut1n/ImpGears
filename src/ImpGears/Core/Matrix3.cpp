#include <Core/Matrix3.h>

#include <cstring>
#include <cmath>

#include <Core/Matrix4.h>

IMPGEARS_BEGIN

#define IDX(x,y)_data[y*3+x]

Matrix3::Matrix3()
	: Matrix<3,3,float>()
{
}

Matrix3::Matrix3(const float* buf, bool transp)
	: Matrix<3,3,float>(buf,transp)
{
}

Matrix3::Matrix3(const Vec3& c1,const Vec3& c2,const Vec3& c3)
{
	at(0,0)=c1[0]; at(0,1)=c1[1]; at(0,2)=c1[2];
	at(1,0)=c2[0]; at(1,1)=c2[1]; at(1,2)=c2[2];
	at(2,0)=c3[0]; at(2,1)=c3[1]; at(2,2)=c3[2];
}

Matrix3::~Matrix3()
{
}

Matrix3& Matrix3::operator+=(const Matrix3& other)
{
    for(std::uint32_t i=0; i<9; ++i)
        _data[i] += other._data[i];

    return *this;
}

Matrix3& Matrix3::operator-=(const Matrix3& other)
{
    for(std::uint32_t i=0; i<9; ++i)
        _data[i] -= other._data[i];

    return *this;
}

Matrix3 Matrix3::operator+(const Matrix3& other) const
{
    return Matrix3(*this) += other;
}

Matrix3 Matrix3::operator-(const Matrix3& other) const
{
    return Matrix3(*this) -= other;
}

float Matrix3::det() const
{
    /**
    A = | a b c |
        | d e f |
        | g h i |

    det(A) = aei + bfg + cdh - ceg - fha - ibd
    */

    float result =
    IDX(0,0)*IDX(1,1)*IDX(2,2)
    + IDX(1,0)*IDX(2,1)*IDX(0,2)
    + IDX(2,0)*IDX(0,1)*IDX(1,2)
    - IDX(2,0)*IDX(1,1)*IDX(0,2)
    - IDX(2,1)*IDX(1,2)*IDX(0,0)
    - IDX(2,2)*IDX(1,0)*IDX(0,1);

    return result;
}

Matrix3 Matrix3::inverse() const
{
    /**
    A = | a b c |
        | d e f |
        | g h i |

    A-1 = 1/det(A) * transpose( | ei-fh  fg-di  dh-eg | )
                                | ch-bi  ai-cg  bg-ah |
                                | bf-ce  cd-af  ae-bd |
    */

    float result[9];
    result[0] = IDX(1,1)*IDX(2,2) - IDX(2,1)*IDX(1,2);
    result[1] = IDX(2,0)*IDX(1,2) - IDX(1,0)*IDX(2,2);
    result[2] = IDX(1,0)*IDX(2,1) - IDX(2,0)*IDX(1,1);

    result[3] = IDX(2,1)*IDX(0,2) - IDX(0,1)*IDX(2,2);
    result[4] = IDX(0,0)*IDX(2,2) - IDX(2,0)*IDX(0,2);
    result[5] = IDX(2,0)*IDX(0,1) - IDX(0,0)*IDX(2,1);

    result[6] = IDX(0,1)*IDX(1,2) - IDX(1,1)*IDX(0,2);
    result[7] = IDX(1,0)*IDX(0,2) - IDX(0,0)*IDX(1,2);
    result[8] = IDX(0,0)*IDX(1,1) - IDX(1,0)*IDX(0,1);

    Matrix3 mat(result);
    float matDet = mat.det();
    mat.transpose();

    return mat * (1.f/matDet);
}

Matrix3 Matrix3::rotationX(float rad)
{
    ///     1           0           0
    ///     0           std::cos(a)      -std::sin(a)
    ///     0           std::sin(a)      std::cos(a)

    const float values[9] = {
        1.f,        0.f,            0.f,
        0.f,        std::cos(rad),      -std::sin(rad),
        0.f,        std::sin(rad),      std::cos(rad)
    };

    return Matrix3(values,true);
}

Matrix3 Matrix3::rotationY(float rad)
{
    ///     std::cos(a)      0           std::sin(a)
    ///     0           1           0
    ///     -std::sin(a)     0           std::cos(a)

    const float values[9] = {
        std::cos(rad),      0.f,        std::sin(rad),
        0.f,            1.f,        0.f,
        -std::sin(rad),     0.f,        std::cos(rad)
    };

    return Matrix3(values,true);
}

Matrix3 Matrix3::rotationZ(float rad)
{
    ///     std::cos(a)      -std::sin(a)     0
    ///     std::sin(a)      std::cos(a)      0
    ///     0           0           1

    const float values[9] = {
        std::cos(rad),      -std::sin(rad),         0.f,
        std::sin(rad),      std::cos(rad),          0.f,
        0.f,            0.f,                1.f
    };

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
    const float data[9] = {
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
