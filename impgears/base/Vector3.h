#ifndef IMP_Vector3_H
#define IMP_Vector3_H

#include "impBase.hpp"

IMPGEARS_BEGIN

class Vector3
{
    public:
        Vector3();
        Vector3(float x, float y, float z);
        Vector3(const Vector3& other);
        virtual ~Vector3();

        void setX(float x){m_x = x;}
        void setY(float y){m_y = y;}
        void setZ(float z){m_z = z;}

        const float getX() const{return m_x;}
        const float getY() const{return m_y;}
        const float getZ() const{return m_z;}

        void setXYZ(float x, float y, float z)
        {
            m_x = x;
            m_y = y;
            m_z = z;
        }

        void setRadial(float theta, float phi, float norm = 1);

        Vector3& operator*=(const float scalar);
        Vector3 operator*(const float scalar) const;

        Vector3& operator=(const Vector3& other);
        Vector3& operator+=(const Vector3& other);
        Vector3& operator-=(const Vector3& other);

        Vector3 operator+(const Vector3& other) const;
        Vector3 operator-(const Vector3& other) const;

        void rotationX(float rx);
        void rotationY(float ry);
        void rotationZ(float rz);

        float dotProduct(const Vector3& other) const;
        Vector3 crossProduct(const Vector3& other) const;

        float getNorm() const;
        float getSqNorm() const;

        void truncate();
        void normalize();

    protected:
    private:

        float m_x;
        float m_y;
        float m_z;
};

IMPGEARS_END

#endif // IMP_Vector3_H
