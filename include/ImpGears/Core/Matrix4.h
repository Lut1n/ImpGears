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
	Matrix4(const Matrix<4,4,float>& other);
	Matrix4(const float* buf, bool transp = false);
	virtual ~Matrix4();

	const Matrix4& operator*=(const Matrix4& other);
	const Matrix4 operator*(const Matrix4& other) const;

	const Matrix4& operator*=(float scalar);
	const Matrix4 operator*(float scalar) const;

	float getDet() const;

	const Matrix4 getInverse() const;

	static const Matrix4 getPerspectiveProjectionMat(float fovx, float whRatio, float nearValue, float farValue);
	static const Matrix4 getOrthographicProjectionMat(float l, float r, float b, float t, float nearValue, float farValue);
	static const Matrix4 getOrthographicProjectionMat(float width, float height, float nearValue, float farValue);
	static const Matrix4 getViewMat(const Vec3& pos, const Vec3& target, const Vec3& up);
	static const Matrix4 getTranslationMat(float tx, float ty, float tz);
	static const Matrix4 getRotationMat(float rx, float ry, float rz);
	static const Matrix4 getRotationXAxisMat(float rx);
	static const Matrix4 getRotationYAxisMat(float ry);
	static const Matrix4 getRotationZAxisMat(float rz);
	static const Matrix4 getScaleMat(float sx, float sy, float sz);
};

IMPGEARS_END

#endif // IMP_Matrix4_H
