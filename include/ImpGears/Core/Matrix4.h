#ifndef IMP_MATRIX4_H
#define IMP_MATRIX4_H

#include <Core/Matrix.h>
#include <Core/Vec3.h>

IMPGEARS_BEGIN

class IMP_API Matrix4 : public Matrix<4,4,float>
{
public:

	Meta_Class(Matrix4)

	Matrix4();
	template<int Cn2, int Rn2>
	Matrix4(const Matrix<Cn2,Rn2,float>& mat) : Matrix(mat) {}
	Matrix4(const float* buf, bool transp = false);
	virtual ~Matrix4();

	const Matrix4& operator=(const Matrix& other){ Matrix::operator=(other); return *this; }
	
	float det() const;

	Matrix4 inverse() const;

	static Matrix4 perspectiveProj(float fovx, float whRatio, float nearValue, float farValue);
	static Matrix4 orthographicProj(float l, float r, float b, float t, float nearValue, float farValue);
	static Matrix4 orthographicProj(float width, float height, float nearValue, float farValue);
	static Matrix4 view(const Vec3& pos, const Vec3& target, const Vec3& up);
	static Matrix4 translation(float tx, float ty, float tz);
	static Matrix4 rotationX(float rx);
	static Matrix4 rotationY(float ry);
	static Matrix4 rotationZ(float rz);
	static Matrix4 scale(float sx, float sy, float sz);
};

IMPGEARS_END

#endif // IMP_Matrix4_H
