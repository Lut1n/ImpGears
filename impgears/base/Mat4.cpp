#include "Mat4.h"
#include <cstring>

IMPGEARS_BEGIN

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

IMPGEARS_END
