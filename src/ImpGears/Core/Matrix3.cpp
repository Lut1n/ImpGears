#include <Core/Matrix3.h>

#include <cstring>
#include <cmath>

#include <Core/Matrix4.h>

IMPGEARS_BEGIN

#define IDX(x,y)m_values[y*3+x]

Matrix3::Matrix3()
{
    for(std::uint32_t i=0; i<9; ++i)
        m_values[i] = 0.f;
}

Matrix3::Matrix3(const Matrix3& other)
{
    memcpy(m_values, other.m_values, sizeof(float)*9);
}

Matrix3::Matrix3(const Matrix4& Matrix4)
{
    const float* value4 = Matrix4.getData();
    for(int c=0; c<3; ++c)
    {
        memcpy(&(m_values[c*3]), &(value4[c*4]), sizeof(float)*3);
    }
}

Matrix3::Matrix3(const float values[9], bool transpose)
{
    memcpy(m_values, values, sizeof(float)*9);
	
    if(transpose)
    {
        Matrix3 copy(*this);
        *this = copy.getTranspose();
    }
}

Matrix3::~Matrix3()
{
}

void Matrix3::setValue(std::uint32_t l, std::uint32_t c, float value)
{
    m_values[c*3+l] = value;
}

float Matrix3::getValue(std::uint32_t l, std::uint32_t c) const
{
    return m_values[c*3+l];
}

Matrix3& Matrix3::operator=(const Matrix3& other)
{
    memcpy(m_values, other.m_values, sizeof(float)*9);

    return *this;
}

Matrix3& Matrix3::operator+=(const Matrix3& other)
{
    for(std::uint32_t i=0; i<9; ++i)
        m_values[i] += other.m_values[i];

    return *this;
}

Matrix3& Matrix3::operator-=(const Matrix3& other)
{
    for(std::uint32_t i=0; i<9; ++i)
        m_values[i] -= other.m_values[i];

    return *this;
}

Matrix3& Matrix3::operator*=(const Matrix3& other)
{
    const Matrix3 before(*this);

    for(std::uint32_t l=0;l<3;l++)
    for(std::uint32_t c=0;c<3;c++){

        m_values[c*3+l] = 0.f;
        for(std::uint32_t k=0; k<3;k++)
            m_values[c*3+l] += ( before.m_values[c*3+k] * other.m_values[k*3+l] );
    }

    return *this;
}

Matrix3& Matrix3::operator*=(float scalar)
{
    for(std::uint32_t i=0; i<9; ++i)
        m_values[i] *= scalar;

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

Matrix3 Matrix3::operator*(const Matrix3& other) const
{
    return Matrix3(*this) *= other;
}

Matrix3 Matrix3::operator*(float scalar) const
{
    return Matrix3(*this) *= scalar;
}

/*
imp::Vec3 Matrix3::operator*(const imp::Vec3& vector) const
{
    imp::Vec3 result;

    imp::Vec3 line1(m_values[0], m_values[1], m_values[2]);
    imp::Vec3 line2(m_values[3], m_values[4], m_values[5]);
    imp::Vec3 line3(m_values[6], m_values[7], m_values[8]);

    result.x() = line1.dot(vector);
    result.y() = line2.dot(vector);
    result.z() = line3.dot(vector);

    return result;
}
*/

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

Matrix3 Matrix3::getTranspose() const
{
    Matrix3 transpose(*this);

    for(std::uint32_t i=0;i<3;i++){
    for(std::uint32_t j=0;j<3;j++){
        transpose.setValue(i,j, getValue(j,i));
    }}

    return transpose;
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
    mat = mat.getTranspose();

    return mat * (1.f/matDet);
}

Matrix4 Matrix3::asMatrix4() const
{
    Matrix4 matrix4 = Matrix4::getIdentityMat();

    for(int c=0; c<3; ++c)
    {
        for(int l=0; l<3; ++l)
        {
            matrix4.setValue(c, l, getValue(l,c));
        }
    }

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

const Matrix3 Matrix3::getIdentity()
{
    const float values[9] = { 1.f, 0.f, 0.f,
                        0.f, 1.f, 0.f,
                        0.f, 0.f, 1.f};

    return Matrix3(values);
}
IMPGEARS_END
