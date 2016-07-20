#ifndef IMP_VECTOR3_H
#define IMP_VECTOR3_H

#include "impBase.h"

IMPGEARS_BEGIN

/// \brief Defines a float vector3.
class IMP_API Vector3
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

        /// \brief Sets the vector value from angles and norm.
        /// \param theta - Z axis rotation angle.
        /// \param phi - X axis rotation angle.
        /// \param norm - The norm of the vector.
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

        /// \brief Applies a dot product.
        /// \param other - An other vector3.
        /// \return The result of the dot product.
        float dotProduct(const Vector3& other) const;

        /// \brief Applies a cross product.
        /// \param other - An other vector3.
        /// \return The result of the cross product.
        Vector3 crossProduct(const Vector3& other) const;

        float getNorm() const;
        float getSqNorm() const;

        /// \brief Applies a truncation on the vector values.
        void truncate();

        void normalize();

    protected:
    private:

        float m_x;
        float m_y;
        float m_z;
};

IMPGEARS_END

#endif // IMP_VECTOR3_H
