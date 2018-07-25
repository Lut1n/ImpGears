#ifndef IMP_VEC4_H
#define IMP_VEC4_H

#include <Core/Object.h>

#include <Core/Vec3.h>
#include <Core/Matrix4.h>

IMPGEARS_BEGIN

/// \brief Defines a float vector3.
class IMP_API Vec4 : public Object
{
    public:
		
		Meta_Class(Vec4)

		Vec4();

		Vec4(const Vec3& v);

		Vec4(const Vec4& v);

		Vec4(float x,float y, float z, float w);

		virtual ~Vec4();

		operator Vec3() const;

		void uniformize();

		float dot(const Vec4& other) const;

		Vec4 cross(const Vec4& other) const;

		Vec4 operator*(float scalar) const;

		Vec4& operator=(const Vec4& other);

		Vec4 operator+(const Vec4& other) const;

		Vec4 operator-(const Vec4& other) const;

		Vec4 operator-() const;

		Vec4 operator*(const Vec4& other) const;

		bool operator==(const Vec4& other) const;

		void set(float x, float y, float z, float w);

		float& operator[](unsigned int i);

		float operator[](unsigned int i) const;

		Vec4 operator*(const Matrix4& mat4) const;

		float length() const;

		float length2() const;

		void normalize();

    private:

        float _data[4];
};

IMPGEARS_END

#endif // IMP_VEC4_H
