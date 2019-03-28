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

Matrix3::Matrix3(const Matrix<3,3,float>& other)
	: Matrix<3,3,float>(other)
{
}

Matrix3::Matrix3(const Matrix<4,4,float>& matrix4)
{
    for(int c=0; c<3; ++c)
		for(int r=0;r<3;++r) at(c,r)=matrix4(c,r);
}

Matrix3::Matrix3(const float* buf, bool transp)
	: Matrix<3,3,float>(buf,transp)
{
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

float Matrix3::getDet() const
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

Matrix3 Matrix3::getInverse() const
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
    float matDet = mat.getDet();
    mat.transpose();

    return mat * (1.f/matDet);
}

Matrix<4,4,float> Matrix3::asMatrix4() const
{
    Matrix<4,4,float> matrix4;

    for(int c=0; c<3; ++c)
        for(int r=0; r<3; ++r) matrix4(c,r) = at(c,r);

    return matrix4;
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

IMPGEARS_END
