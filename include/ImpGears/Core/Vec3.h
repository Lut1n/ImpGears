#ifndef IMP_VECTOR3_H
#define IMP_VECTOR3_H

#include <Core/Object.h>

IMPGEARS_BEGIN

/// \brief Defines a float vector3.
class IMP_API Vec3 : public Object
{
    public:
		
		Meta_Class(Vec3)
	
        Vec3();
        Vec3(const float* buf);
        Vec3(float x, float y, float z);
        Vec3(const Vec3& other);
        virtual ~Vec3();

        float& x();
        float& y();
        float& z();

        float x() const;
        float y() const;
        float z() const;

        void set(float x, float y, float z);
		
        void setRadial(float theta, float phi, float norm = 1);

        Vec3& operator*=(const float scalar);
        Vec3 operator*(const float scalar) const;
        Vec3 operator/(const float scalar) const;

        Vec3& operator=(const Vec3& other);
        Vec3& operator+=(const Vec3& other);
        Vec3& operator-=(const Vec3& other);
		Vec3& operator*=(const Vec3& other);

        Vec3 operator+(const Vec3& other) const;
        Vec3 operator-(const Vec3& other) const;
        Vec3 operator*(const Vec3& other) const;
		
		bool operator==(const Vec3& other) const;
		
		float& operator[](unsigned int i);
		float operator[](unsigned int i) const;

        void rotX(float rx);
        void rotY(float ry);
        void rotZ(float rz);
		
        float dot(const Vec3& other) const;
		
        Vec3 cross(const Vec3& other) const;

        float length() const;
        float length2() const;

        void normalize();
		
		float* data() {return _data;}
		const float* data() const {return _data;}

    private:
	
        float _data[3];
};

IMPGEARS_END

#endif // IMP_VECTOR3_H
