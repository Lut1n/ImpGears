#ifndef IMP_MATRIX4_H
#define IMP_MATRIX4_H

#include <ImpGears/Core/Matrix.h>
#include <ImpGears/Core/Vec3.h>
#include <ImpGears/Core/Vec4.h>
#include <ImpGears/Core/Export.h>

IMPGEARS_BEGIN

class IG_CORE_API Matrix4 : public Matrix<4,4,float>
{
public:

	Meta_Class(Matrix4)

	Matrix4();
	template<int Cn2, int Rn2>
	Matrix4(const Matrix<Cn2,Rn2,float>& mat) : Matrix(mat) {}
	Matrix4(const float* buf, bool transp = false);
	Matrix4(const Vec4& c1,const Vec4& c2,const Vec4& c3,const Vec4& c4);
	virtual ~Matrix4();

	const Matrix4& operator=(const Matrix& other){ Matrix::operator=(other); return *this; }
	
	float det() const;

	Matrix4 inverse() const;

	static Matrix4 perspectiveProj(float fovx, float whRatio, float nearValue, float farValue);
	static Matrix4 orthographicProj(float l, float r, float b, float t, float nearValue, float farValue);
	static Matrix4 orthographicProj(float width, float height, float nearValue, float farValue);
	static Matrix4 view(const Vec3& pos, const Vec3& target, const Vec3& up);
	static Matrix4 translation(float tx, float ty, float tz);
	static Matrix4 translation(const Vec3& translation);
	static Matrix4 rotationX(float rx);
	static Matrix4 rotationY(float ry);
	static Matrix4 rotationZ(float rz);
	static Matrix4 rotationXYZ(const Vec3& rotation);
	static Matrix4 scale(float sx, float sy, float sz);
	static Matrix4 scale(const Vec3& scale);
};

IMPGEARS_END

#endif // IMP_Matrix4_H
