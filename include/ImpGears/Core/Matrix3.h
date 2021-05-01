#ifndef IMP_MATRIX3_H
#define IMP_MATRIX3_H

#include <ImpGears/Core/Matrix.h>
#include <ImpGears/Core/Vec3.h>
#include <ImpGears/Core/Export.h>

IMPGEARS_BEGIN

class IG_CORE_API Matrix3 : public Matrix<3,3,float>
{
public:

	Meta_Class(Matrix3)

	Matrix3();
	template<int Cn2, int Rn2>
	Matrix3(const Matrix<Cn2,Rn2,float>& mat) : Matrix(mat) {}
	Matrix3(const float* buf, bool transp = false);
	Matrix3(const Vec3& c1,const Vec3& c2,const Vec3& c3);
	virtual ~Matrix3();
	
	const Matrix3& operator=(const Matrix& other){ Matrix::operator=(other); return *this; }

	Matrix3& operator+=(const Matrix3& other);
	Matrix3& operator-=(const Matrix3& other);
	Matrix3 operator+(const Matrix3& other) const;
	Matrix3 operator-(const Matrix3& other) const;

	float det() const;
	Matrix3 inverse() const;

	static Matrix3 rotationX(float rx);
	static Matrix3 rotationY(float ry);
	static Matrix3 rotationZ(float rz);
	static Matrix3 rotationXYZ(const Vec3& rotation);
	static Matrix3 scale(float sx, float sy, float sz);
	static Matrix3 scale(const Vec3& scale);
};

IMPGEARS_END

#endif // IMP_MATRIX3_H
