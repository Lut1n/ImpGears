#include "Matrix4.h"
#include <cstring>

#include <cmath>

IMPGEARS_BEGIN

#define IDX(x,y)m_data[(y-1)*4+(x-1)]

//--------------------------------------------------------------
Matrix4::Matrix4()
{
    memset(m_data, 0, 16*sizeof(float));
}

//--------------------------------------------------------------
Matrix4::Matrix4(const Matrix4& other)
{
    setData(other.m_data);
}

//--------------------------------------------------------------
Matrix4::Matrix4(const float* data, bool transpose)
{
    setData(data);

    if(transpose)
    {
        Matrix4 copy(*this);
        *this = copy.getTranspose();
    }
}

//--------------------------------------------------------------
Matrix4::~Matrix4()
{
}

//--------------------------------------------------------------
const Matrix4& Matrix4::operator=(const Matrix4& other)
{
    setData(other.m_data);

    return *this;
}

//--------------------------------------------------------------
const Matrix4& Matrix4::operator*=(const Matrix4& other)
{
    Matrix4 original(*this);

    for(Uint32 l=0; l<4; ++l)
    {
        for(Uint32 c=0; c<4; ++c)
        {
            float r = 0.f;
            for(Uint32 n = 0; n<4; ++n)
                r += original.getValue(l,n) * other.getValue(n,c);

            setValue(l,c,r);
        }
    }

    return *this;
}

//--------------------------------------------------------------
const Matrix4 Matrix4::operator*(const Matrix4& other) const
{
    Matrix4 result(*this);

    return result *= other;
}

//--------------------------------------------------------------
const Matrix4& Matrix4::operator*=(float scalar)
{
    for(Uint32 i=0; i<16; ++i)
        m_data[0] *= scalar;

    return *this;
}

//--------------------------------------------------------------
const Matrix4 Matrix4::operator*(float scalar) const
{
    Matrix4 result(*this);
    result*=scalar;

    return result;
}

//--------------------------------------------------------------
const Vector3 Matrix4::operator*(const Vector3& vec) const
{
    Vector3 result;

    result.setXYZ(
                  vec.getX()*IDX(1,1) + vec.getY()*IDX(2,1) + vec.getZ()*IDX(3,1) + IDX(4,1),
                  vec.getX()*IDX(1,2) + vec.getY()*IDX(2,2) + vec.getZ()*IDX(3,2) + IDX(4,2),
                  vec.getX()*IDX(1,3) + vec.getY()*IDX(2,3) + vec.getZ()*IDX(3,3) + IDX(4,3)
                  );

    return result;
}

//--------------------------------------------------------------
const Matrix4 Matrix4::getTranspose() const
{
    Matrix4 transposed(*this);

    for(Uint32 i=0; i<4; ++i)
    {
        for(Uint32 j=0; j<4; ++j)
        {
            transposed.setValue(i, j, getValue(j,i));
        }
    }

    return transposed;
}

//--------------------------------------------------------------
float Matrix4::getDet() const
{
    float result = IDX(1,1)*IDX(2,2)*IDX(3,3)*IDX(4,4) + IDX(1,1)*IDX(2,3)*IDX(3,4)*IDX(4,2) + IDX(1,1)*IDX(2,4)*IDX(3,2)*IDX(4,3)
                 + IDX(1,2)*IDX(2,1)*IDX(3,4)*IDX(4,3) + IDX(1,2)*IDX(2,3)*IDX(3,1)*IDX(4,4) + IDX(1,2)*IDX(2,4)*IDX(3,3)*IDX(4,1)
                 + IDX(1,3)*IDX(2,1)*IDX(3,2)*IDX(4,4) + IDX(1,3)*IDX(2,2)*IDX(3,4)*IDX(4,1) + IDX(1,3)*IDX(2,4)*IDX(3,1)*IDX(4,2)
                 + IDX(1,4)*IDX(2,1)*IDX(3,3)*IDX(4,2) + IDX(1,4)*IDX(2,2)*IDX(3,1)*IDX(4,3) + IDX(1,4)*IDX(2,3)*IDX(3,2)*IDX(4,1)
                 - IDX(1,1)*IDX(2,2)*IDX(3,4)*IDX(4,3) + IDX(1,1)*IDX(2,3)*IDX(3,2)*IDX(4,4) + IDX(1,1)*IDX(2,4)*IDX(3,3)*IDX(4,2)
                 - IDX(1,2)*IDX(2,1)*IDX(3,3)*IDX(4,4) + IDX(1,2)*IDX(2,3)*IDX(3,4)*IDX(4,1) + IDX(1,2)*IDX(2,4)*IDX(3,1)*IDX(4,3)
                 - IDX(1,3)*IDX(2,1)*IDX(3,4)*IDX(4,2) + IDX(1,3)*IDX(2,2)*IDX(3,1)*IDX(4,4) + IDX(1,3)*IDX(2,4)*IDX(3,2)*IDX(4,1)
                 - IDX(1,4)*IDX(2,1)*IDX(3,2)*IDX(4,3) + IDX(1,4)*IDX(2,2)*IDX(3,3)*IDX(4,1) + IDX(1,4)*IDX(2,3)*IDX(3,1)*IDX(4,2);

    return result;
}

//--------------------------------------------------------------
const Matrix4 Matrix4::getInverse() const
{
    float result[16];

    result[0] = IDX(2,2)*IDX(3,3)*IDX(4,4) + IDX(2,3)*IDX(3,4)*IDX(4,2) + IDX(2,4)*IDX(3,2)*IDX(4,3)
              - IDX(2,2)*IDX(3,4)*IDX(4,3) - IDX(2,3)*IDX(3,2)*IDX(4,4) - IDX(2,4)*IDX(3,3)*IDX(4,2);
    result[1] = IDX(2,1)*IDX(3,4)*IDX(4,3) + IDX(2,3)*IDX(3,1)*IDX(4,4) + IDX(2,4)*IDX(3,3)*IDX(4,1)
              - IDX(2,1)*IDX(3,3)*IDX(4,4) - IDX(2,3)*IDX(3,4)*IDX(4,1) - IDX(2,4)*IDX(3,1)*IDX(4,3);
    result[2] = IDX(2,1)*IDX(3,2)*IDX(4,4) + IDX(2,2)*IDX(3,4)*IDX(4,1) + IDX(2,4)*IDX(3,1)*IDX(4,2)
              - IDX(2,1)*IDX(3,4)*IDX(4,2) - IDX(2,2)*IDX(3,1)*IDX(4,4) - IDX(2,4)*IDX(3,2)*IDX(4,1);
    result[3] = IDX(2,1)*IDX(3,3)*IDX(4,2) + IDX(2,2)*IDX(3,1)*IDX(4,3) + IDX(2,3)*IDX(3,2)*IDX(4,1)
              - IDX(2,1)*IDX(3,2)*IDX(4,3) - IDX(2,2)*IDX(3,3)*IDX(4,1) - IDX(2,3)*IDX(3,1)*IDX(4,2);

    result[4] = IDX(1,2)*IDX(3,4)*IDX(4,3) + IDX(1,3)*IDX(3,2)*IDX(4,4) + IDX(1,4)*IDX(3,3)*IDX(4,2)
              - IDX(1,2)*IDX(3,3)*IDX(4,4) - IDX(1,3)*IDX(3,4)*IDX(4,2) - IDX(1,4)*IDX(3,2)*IDX(4,3);
    result[5] = IDX(1,1)*IDX(3,3)*IDX(4,4) + IDX(1,3)*IDX(3,4)*IDX(4,1) + IDX(1,4)*IDX(3,1)*IDX(4,3)
              - IDX(1,1)*IDX(3,4)*IDX(4,3) - IDX(1,3)*IDX(3,1)*IDX(4,4) - IDX(1,4)*IDX(3,3)*IDX(4,1);
    result[6] = IDX(1,1)*IDX(3,4)*IDX(4,2) + IDX(1,2)*IDX(3,1)*IDX(4,4) + IDX(1,4)*IDX(3,2)*IDX(4,1)
              - IDX(1,1)*IDX(3,2)*IDX(4,4) - IDX(1,2)*IDX(3,4)*IDX(4,1) - IDX(1,4)*IDX(3,1)*IDX(4,2);
    result[7] = IDX(1,1)*IDX(3,2)*IDX(4,3) + IDX(1,2)*IDX(3,3)*IDX(4,1) + IDX(1,3)*IDX(3,1)*IDX(4,2)
              - IDX(1,1)*IDX(2,2)*IDX(4,3) - IDX(1,2)*IDX(2,3)*IDX(4,1) - IDX(1,3)*IDX(2,1)*IDX(4,2);

    result[8] = IDX(1,2)*IDX(2,3)*IDX(4,4) + IDX(1,3)*IDX(2,4)*IDX(4,2) + IDX(1,4)*IDX(2,2)*IDX(4,3)
              - IDX(1,2)*IDX(2,4)*IDX(4,3) - IDX(1,3)*IDX(2,2)*IDX(4,4) - IDX(1,4)*IDX(2,3)*IDX(4,2);
    result[9] = IDX(1,1)*IDX(2,4)*IDX(4,3) + IDX(1,3)*IDX(2,1)*IDX(4,4) + IDX(1,4)*IDX(2,3)*IDX(4,1)
              - IDX(1,1)*IDX(2,3)*IDX(4,4) - IDX(1,3)*IDX(2,4)*IDX(4,1) - IDX(1,4)*IDX(2,1)*IDX(4,3);
    result[10] = IDX(1,1)*IDX(2,2)*IDX(4,4) + IDX(1,2)*IDX(2,4)*IDX(4,1) + IDX(1,4)*IDX(2,1)*IDX(4,2)
               - IDX(1,1)*IDX(2,4)*IDX(4,2) - IDX(1,2)*IDX(2,1)*IDX(4,4) - IDX(1,4)*IDX(2,2)*IDX(4,1);
    result[11] = IDX(1,1)*IDX(2,3)*IDX(4,2) + IDX(1,2)*IDX(2,1)*IDX(4,3) + IDX(1,3)*IDX(2,2)*IDX(4,1)
               - IDX(1,1)*IDX(2,2)*IDX(4,3) - IDX(1,2)*IDX(2,3)*IDX(4,1) - IDX(1,3)*IDX(2,1)*IDX(4,2);

    result[12] = IDX(1,2)*IDX(2,4)*IDX(3,3) + IDX(1,3)*IDX(2,2)*IDX(3,4) + IDX(1,4)*IDX(2,3)*IDX(3,2)
               - IDX(1,2)*IDX(2,3)*IDX(3,4) - IDX(1,3)*IDX(2,4)*IDX(3,2) - IDX(1,4)*IDX(2,2)*IDX(3,3);
    result[13] = IDX(1,1)*IDX(2,3)*IDX(3,4) + IDX(1,3)*IDX(2,4)*IDX(3,1) + IDX(1,4)*IDX(2,1)*IDX(3,3)
               - IDX(1,1)*IDX(2,4)*IDX(3,3) - IDX(1,3)*IDX(2,1)*IDX(3,4) - IDX(1,4)*IDX(2,3)*IDX(3,1);
    result[14] = IDX(1,1)*IDX(2,4)*IDX(3,2) + IDX(1,2)*IDX(2,1)*IDX(3,4) + IDX(1,4)*IDX(2,2)*IDX(3,1)
               - IDX(1,1)*IDX(2,2)*IDX(3,4) - IDX(1,2)*IDX(2,4)*IDX(3,1) - IDX(1,4)*IDX(2,1)*IDX(3,2);
    result[15] = IDX(1,1)*IDX(2,2)*IDX(3,3) + IDX(1,2)*IDX(2,3)*IDX(3,1) + IDX(1,3)*IDX(2,1)*IDX(3,2)
               - IDX(1,1)*IDX(2,3)*IDX(3,2) - IDX(1,2)*IDX(2,1)*IDX(3,3) - IDX(1,3)*IDX(2,2)*IDX(3,1);

    Matrix4 resultMat(result);
    resultMat*=(1.f/getDet());

    return resultMat;
}

//--------------------------------------------------------------
void Matrix4::setData(const float* data)
{
    memcpy(m_data, data, 16*sizeof(float));
}

//--------------------------------------------------------------
const float* Matrix4::getData() const
{
    return m_data;
}

//--------------------------------------------------------------
void Matrix4::setValue(Uint32 c, Uint32 l, float v)
{
    m_data[c*4+l] = v;
}

//--------------------------------------------------------------
float Matrix4::getValue(Uint32 c, Uint32 l) const
{
    return m_data[c*4+l];
}

//--------------------------------------------------------------
const Matrix4 Matrix4::getPerspectiveProjectionMat(float fovx, float whRatio, float nearValue, float farValue)
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
const Matrix4 Matrix4::getOrthographicProjectionMat(float l, float r, float b, float t, float nearValue, float farValue)
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
const Matrix4 Matrix4::getOrthographicProjectionMat(float width, float height, float nearValue, float farValue)
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
const Matrix4 Matrix4::getViewMat(const Vector3& pos, const Vector3& target, const Vector3& up)
{
    Vector3 zAxis = pos-target; zAxis.normalize();
    Vector3 xAxis = up.crossProduct(zAxis); xAxis.normalize();
    Vector3 yAxis = zAxis.crossProduct(xAxis);

    const float data[16] = {
        xAxis.getX(),                   yAxis.getX(),                     zAxis.getX(),                    0.f,
        xAxis.getY(),                   yAxis.getY(),                     zAxis.getY(),                    0.f,
        xAxis.getZ(),                   yAxis.getZ(),                     zAxis.getZ(),                    0.f,
        -xAxis.dotProduct(pos),         -yAxis.dotProduct(pos),           -zAxis.dotProduct(pos),          1.f
    };

    return Matrix4(data);
}

//--------------------------------------------------------------
const Matrix4 Matrix4::getIdentityMat()
{
    const float data[16] = {
        1.f,    0.f,    0.f,    0.f,
        0.f,    1.f,    0.f,    0.f,
        0.f,    0.f,    1.f,    0.f,
        0.f,    0.f,    0.f,    1.f
    };

    return Matrix4(data);
}

//--------------------------------------------------------------
const Matrix4 Matrix4::getTranslationMat(float tx, float ty, float tz)
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
const Matrix4 Matrix4::getRotationMat(float rx, float ry, float rz)
{
    return getRotationXAxisMat(rx) * getRotationYAxisMat(ry) * getRotationZAxisMat(rz);
}

//--------------------------------------------------------------
const Matrix4 Matrix4::getRotationXAxisMat(float rx)
{
    const float data[16] = {
        1.f,    0.f,        0.f,        0.f,
        0.f,    cosf(rx),   -sinf(rx),  0.f,
        0.f,    sinf(rx),   cosf(rx),   0.f,
        0.f,    0.f,        0.f,        1.f
    };

    return Matrix4(data, true);
}


//--------------------------------------------------------------
const Matrix4 Matrix4::getRotationYAxisMat(float ry)
{
    const float data[16] = {
        cosf(ry),    0.f,        sinf(ry),   0.f,
        0.f,         1.f,        0.f,        0.f,
        -sinf(ry),   0.f,        cosf(ry),   0.f,
        0.f,         0.f,        0.f,        1.f
    };

    return Matrix4(data, true);
}


//--------------------------------------------------------------
const Matrix4 Matrix4::getRotationZAxisMat(float rz)
{
    const float data[16] = {
        cosf(rz),    -sinf(rz),   0.f,    0.f,
        sinf(rz),    cosf(rz),    0.f,    0.f,
        0.f,        0.f,          1.f,    0.f,
        0.f,        0.f,          0.f,    1.f
    };

    return Matrix4(data, true);
}

//--------------------------------------------------------------
const Matrix4 Matrix4::getScaleMat(float sx, float sy, float sz)
{
    const float data[16] = {
        sx,     0.f,    0.f,    0.f,
        0.f,    sy,     0.f,    0.f,
        0.f,    0.f,    sz,     0.f,
        0.f,    0.f,    0.f,    1.f
    };

    return Matrix4(data);
}

IMPGEARS_END
