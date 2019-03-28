#include <Core/Matrix4.h>
#include <cstring>

#include <cmath>

IMPGEARS_BEGIN

#define IDX(x,y)_data[(y-1)*4+(x-1)]

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
Matrix4::~Matrix4()
{
}

//--------------------------------------------------------------
float Matrix4::getDet() const
{
    float result = IDX(1,1)*IDX(2,2)*IDX(3,3)*IDX(4,4) + IDX(1,1)*IDX(2,3)*IDX(3,4)*IDX(4,2) + IDX(1,1)*IDX(2,4)*IDX(3,2)*IDX(4,3)
                 + IDX(1,2)*IDX(2,1)*IDX(3,4)*IDX(4,3) + IDX(1,2)*IDX(2,3)*IDX(3,1)*IDX(4,4) + IDX(1,2)*IDX(2,4)*IDX(3,3)*IDX(4,1)
                 + IDX(1,3)*IDX(2,1)*IDX(3,2)*IDX(4,4) + IDX(1,3)*IDX(2,2)*IDX(3,4)*IDX(4,1) + IDX(1,3)*IDX(2,4)*IDX(3,1)*IDX(4,2)
                 + IDX(1,4)*IDX(2,1)*IDX(3,3)*IDX(4,2) + IDX(1,4)*IDX(2,2)*IDX(3,1)*IDX(4,3) + IDX(1,4)*IDX(2,3)*IDX(3,2)*IDX(4,1)
                 - IDX(1,1)*IDX(2,2)*IDX(3,4)*IDX(4,3) - IDX(1,1)*IDX(2,3)*IDX(3,2)*IDX(4,4) - IDX(1,1)*IDX(2,4)*IDX(3,3)*IDX(4,2)
                 - IDX(1,2)*IDX(2,1)*IDX(3,3)*IDX(4,4) - IDX(1,2)*IDX(2,3)*IDX(3,4)*IDX(4,1) - IDX(1,2)*IDX(2,4)*IDX(3,1)*IDX(4,3)
                 - IDX(1,3)*IDX(2,1)*IDX(3,4)*IDX(4,2) - IDX(1,3)*IDX(2,2)*IDX(3,1)*IDX(4,4) - IDX(1,3)*IDX(2,4)*IDX(3,2)*IDX(4,1)
                 - IDX(1,4)*IDX(2,1)*IDX(3,2)*IDX(4,3) - IDX(1,4)*IDX(2,2)*IDX(3,3)*IDX(4,1) - IDX(1,4)*IDX(2,3)*IDX(3,1)*IDX(4,2);

    return result;
}

//--------------------------------------------------------------
Matrix4 Matrix4::getInverse() const
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
Matrix4 Matrix4::getPerspectiveProjectionMat(float fovx, float whRatio, float nearValue, float farValue)
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
Matrix4 Matrix4::getOrthographicProjectionMat(float l, float r, float b, float t, float nearValue, float farValue)
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
Matrix4 Matrix4::getOrthographicProjectionMat(float width, float height, float nearValue, float farValue)
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
Matrix4 Matrix4::getViewMat(const Vec3& pos, const Vec3& target, const Vec3& up)
{
    Vec3 zAxis = pos-target; zAxis.normalize();
    Vec3 xAxis = up.cross(zAxis); xAxis.normalize();
    Vec3 yAxis = zAxis.cross(xAxis);

    const float data[16] = {
        xAxis.x(),                   yAxis.x(),                     zAxis.x(),                    0.f,
        xAxis.y(),                   yAxis.y(),                     zAxis.y(),                    0.f,
        xAxis.z(),                   yAxis.z(),                     zAxis.z(),                    0.f,
        -xAxis.dot(pos),         -yAxis.dot(pos),           -zAxis.dot(pos),          1.f
    };

    return Matrix4(data);
}

//--------------------------------------------------------------
Matrix4 Matrix4::getTranslationMat(float tx, float ty, float tz)
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
Matrix4 Matrix4::getRotationMat(float rx, float ry, float rz)
{
    return getRotationXAxisMat(rx) * getRotationYAxisMat(ry) * getRotationZAxisMat(rz);
}

//--------------------------------------------------------------
Matrix4 Matrix4::getRotationXAxisMat(float rx)
{
    const float data[16] = {
        1.f,    0.f,        0.f,        0.f,
        0.f,    std::cos(rx),   -std::sin(rx),  0.f,
        0.f,    std::sin(rx),   std::cos(rx),   0.f,
        0.f,    0.f,        0.f,        1.f
    };

    return Matrix4(data, true);
}


//--------------------------------------------------------------
Matrix4 Matrix4::getRotationYAxisMat(float ry)
{
    const float data[16] = {
        std::cos(ry),    0.f,        std::sin(ry),   0.f,
        0.f,         1.f,        0.f,        0.f,
        -std::sin(ry),   0.f,        std::cos(ry),   0.f,
        0.f,         0.f,        0.f,        1.f
    };

    return Matrix4(data, true);
}


//--------------------------------------------------------------
Matrix4 Matrix4::getRotationZAxisMat(float rz)
{
    const float data[16] = {
        std::cos(rz),    -std::sin(rz),   0.f,    0.f,
        std::sin(rz),    std::cos(rz),    0.f,    0.f,
        0.f,        0.f,          1.f,    0.f,
        0.f,        0.f,          0.f,    1.f
    };

    return Matrix4(data, true);
}

//--------------------------------------------------------------
Matrix4 Matrix4::getScaleMat(float sx, float sy, float sz)
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
