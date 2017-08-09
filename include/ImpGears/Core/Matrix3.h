#ifndef IMP_MATRIX3_H
#define IMP_MATRIX3_H

#include "Core/impBase.h"
#include "../Math/Vector3.h"

IMPGEARS_BEGIN

class Matrix4;

class IMP_API Matrix3
{
    public:
        Matrix3();
        Matrix3(const Matrix3& other);
        Matrix3(const Matrix4& Matrix4);
        Matrix3(const float values[9]);
        virtual ~Matrix3();

        void setValue(Uint32 l, Uint32 c, float value);
        float getValue(Uint32 l, Uint32 c) const;

        Matrix3& operator=(const Matrix3& other);
        Matrix3& operator+=(const Matrix3& other);
        Matrix3& operator-=(const Matrix3& other);
        Matrix3& operator*=(const Matrix3& other);
        Matrix3& operator*=(float scalar);

        Matrix3 operator+(const Matrix3& other) const;
        Matrix3 operator-(const Matrix3& other) const;
        Matrix3 operator*(const Matrix3& other) const;
        Matrix3 operator*(float scalar) const;
        imp::Vector3 operator*(const imp::Vector3& vector) const;

        float getDet() const;
        Matrix3 getTranspose() const;
        Matrix3 getInverse() const;

        Matrix4 asMatrix4() const;

        static const Matrix3 getRotationMatrixX(float rad);
        static const Matrix3 getRotationMatrixY(float rad);
        static const Matrix3 getRotationMatrixZ(float rad);

        static const Matrix3 getIdentity();

    protected:
    private:

    float m_values[9];
};

IMPGEARS_END

#endif // IMP_MATRIX3_H
