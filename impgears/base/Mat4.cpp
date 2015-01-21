#include "Mat4.h"
#include <cstring>

Mat4::Mat4()
{
    memset(m_data, 0, 16*sizeof(float));
}

Mat4::Mat4(const Mat4& other)
{
    setData(other.m_data);
}

Mat4::Mat4(const float* data)
{
    setData(data);
}

Mat4::~Mat4()
{
}

const Mat4& Mat4::operator=(const Mat4& other)
{
    setData(other.m_data);

    return *this;
}

void Mat4::setData(const float* data)
{
    memcpy(m_data, data, 16*sizeof(float));
}

const float* Mat4::getData() const
{
    return m_data;
}
