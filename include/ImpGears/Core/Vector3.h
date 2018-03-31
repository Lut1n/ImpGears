#ifndef IMP_VECTOR3_H
#define IMP_VECTOR3_H

#include <Core/Object.h>

IMPGEARS_BEGIN

/// \brief Defines a float vector3.
class IMP_API Vector3 : public Object
{
    public:
		
		Meta_Class(Vector3)
	
        Vector3();
        Vector3(const float* buf);
        Vector3(float x, float y, float z);
        Vector3(const Vector3& other);
        virtual ~Vector3();

        float& x();
        float& y();
        float& z();

        float x() const;
        float y() const;
        float z() const;

        void set(float x, float y, float z);
		
        void setRadial(float theta, float phi, float norm = 1);

        Vector3& operator*=(const float scalar);
        Vector3 operator*(const float scalar) const;
        Vector3 operator/(const float scalar) const;

        Vector3& operator=(const Vector3& other);
        Vector3& operator+=(const Vector3& other);
        Vector3& operator-=(const Vector3& other);
		Vector3& operator*=(const Vector3& other);

        Vector3 operator+(const Vector3& other) const;
        Vector3 operator-(const Vector3& other) const;
        Vector3 operator*(const Vector3& other) const;
		
		bool operator==(const Vector3& other) const;
		
		float& operator[](unsigned int i);
		float operator[](unsigned int i) const;

        void rotX(float rx);
        void rotY(float ry);
        void rotZ(float rz);
		
        float dot(const Vector3& other) const;
		
        Vector3 cross(const Vector3& other) const;

        float length() const;
        float length2() const;

        void normalize();

    private:
	
        float _data[3];
};

IMPGEARS_END

#endif // IMP_VECTOR3_H
