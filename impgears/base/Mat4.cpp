#include "Mat4.h"
#include <cstring>

IMPGEARS_BEGIN

#define IDX(x,y)m_data[(y-1)*4+(x-1)]

//--------------------------------------------------------------
Mat4::Mat4()
{
    memset(m_data, 0, 16*sizeof(float));
}

//--------------------------------------------------------------
Mat4::Mat4(const Mat4& other)
{
    setData(other.m_data);
}

//--------------------------------------------------------------
Mat4::Mat4(const float* data)
{
    setData(data);
}

//--------------------------------------------------------------
Mat4::~Mat4()
{
}

//--------------------------------------------------------------
const Mat4& Mat4::operator=(const Mat4& other)
{
    setData(other.m_data);

    return *this;
}

//--------------------------------------------------------------
const Mat4& Mat4::operator*=(const Mat4& other)
{
    Mat4 original(*this);

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
const Mat4 Mat4::operator*(const Mat4& other) const
{
    Mat4 result(*this);

    return result *= other;
}

//--------------------------------------------------------------
const Mat4& Mat4::operator*=(float scalar)
{
    for(Uint32 i=0; i<16; ++i)
        m_data[0] *= scalar;

    return *this;
}

//--------------------------------------------------------------
const Mat4 Mat4::operator*(float scalar) const
{
    Mat4 result(*this);
    result*=scalar;

    return result;
}

//--------------------------------------------------------------
float Mat4::getDet() const
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
const Mat4 Mat4::getInverse() const
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

    Mat4 resultMat(result);
    resultMat*=(1.f/getDet());

    return resultMat;
}

//--------------------------------------------------------------
void Mat4::setData(const float* data)
{
    memcpy(m_data, data, 16*sizeof(float));
}

//--------------------------------------------------------------
const float* Mat4::getData() const
{
    return m_data;
}

//--------------------------------------------------------------
void Mat4::setValue(Uint32 c, Uint32 l, float v)
{
    m_data[c*4+l] = v;
}

//--------------------------------------------------------------
float Mat4::getValue(Uint32 c, Uint32 l) const
{
    return m_data[c*4+l];
}

IMPGEARS_END
