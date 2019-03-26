#ifndef IMP_MATRIX3_H
#define IMP_MATRIX3_H

#include <Core/Object.h>

IMPGEARS_BEGIN

class Matrix4;

class IMP_API Matrix3 : public Object
{
    public:
	
		Meta_Class(Matrix3)
	
        Matrix3();
        Matrix3(const Matrix3& other);
        Matrix3(const Matrix4& Matrix4);
        Matrix3(const float values[9], bool transpose = false);
        virtual ~Matrix3();

        void setValue(int c, int l, float value);
        float getValue(int c, int l) const;
		
		float operator()(int c, int l) const
		{
			return getValue(c,l);
		}

        Matrix3& operator=(const Matrix3& other);
        Matrix3& operator+=(const Matrix3& other);
        Matrix3& operator-=(const Matrix3& other);
        Matrix3& operator*=(const Matrix3& other);
        Matrix3& operator*=(float scalar);

        Matrix3 operator+(const Matrix3& other) const;
        Matrix3 operator-(const Matrix3& other) const;
        Matrix3 operator*(const Matrix3& other) const;
        Matrix3 operator*(float scalar) const;
		
        // imp::Vec3 operator*(const imp::Vec3& vector) const;

        float getDet() const;
        Matrix3 getTranspose() const;
        Matrix3 getInverse() const;

        Matrix4 asMatrix4() const;

        static Matrix3 rotationX(float rad);
        static Matrix3 rotationY(float rad);
        static Matrix3 rotationZ(float rad);

        static const Matrix3 getIdentity();

    protected:
    private:

    float m_values[9];
};

IMPGEARS_END

#endif // IMP_MATRIX3_H
