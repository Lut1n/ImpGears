#ifndef IMP_MATRIX3_H
#define IMP_MATRIX3_H

#include <Core/Matrix.h>

IMPGEARS_BEGIN

class IMP_API Matrix3 : public Matrix<3,3,float>
{
public:

	Meta_Class(Matrix3)

	Matrix3();
	Matrix3(const Matrix<3,3,float>& other);
	Matrix3(const Matrix<4,4,float>& matrix4);
	Matrix3(const float* buf, bool transp = false);
	virtual ~Matrix3();

	Matrix3& operator+=(const Matrix3& other);
	Matrix3& operator-=(const Matrix3& other);
	Matrix3& operator*=(const Matrix3& other);
	Matrix3& operator*=(float scalar);

	Matrix3 operator+(const Matrix3& other) const;
	Matrix3 operator-(const Matrix3& other) const;
	Matrix3 operator*(const Matrix3& other) const;
	Matrix3 operator*(float scalar) const;

	float getDet() const;
	Matrix3 getInverse() const;

	Matrix<4,4,float> asMatrix4() const;

	static Matrix3 rotationX(float rad);
	static Matrix3 rotationY(float rad);
	static Matrix3 rotationZ(float rad);
};

IMPGEARS_END

#endif // IMP_MATRIX3_H
