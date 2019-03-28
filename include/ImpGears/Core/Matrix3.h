#ifndef IMP_MATRIX3_H
#define IMP_MATRIX3_H

#include <Core/Matrix.h>

IMPGEARS_BEGIN

class IMP_API Matrix3 : public Matrix<3,3,float>
{
public:

	Meta_Class(Matrix3)

	Matrix3();
	template<int Cn2, int Rn2>
	Matrix3(const Matrix<Cn2,Rn2,float>& mat) : Matrix(mat) {}
	Matrix3(const float* buf, bool transp = false);
	virtual ~Matrix3();
	
	const Matrix3& operator=(const Matrix& other){ Matrix::operator=(other); return *this; }

	Matrix3& operator+=(const Matrix3& other);
	Matrix3& operator-=(const Matrix3& other);
	Matrix3 operator+(const Matrix3& other) const;
	Matrix3 operator-(const Matrix3& other) const;

	float getDet() const;
	Matrix3 getInverse() const;

	static Matrix3 rotationX(float rad);
	static Matrix3 rotationY(float rad);
	static Matrix3 rotationZ(float rad);
};

IMPGEARS_END

#endif // IMP_MATRIX3_H
