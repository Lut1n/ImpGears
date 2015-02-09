#ifndef IMP_MATRIX4_H
#define IMP_MATRIX4_H

#include "base/impBase.hpp"
#include "base/Vector3.h"

IMPGEARS_BEGIN

/// \brief Defines an openGL matrix4x4 wrapper.
class Matrix4
{
    public:

        Matrix4();

        Matrix4(const Matrix4& other);

        /// \brief Constructor using float buffer.
        /// \param data - The Matrix4 data (column major).
        /// \param transpose - The Matrix4 data must be transposed.
        Matrix4(const float* data, bool transpose = false);

        virtual ~Matrix4();

        const Matrix4& operator=(const Matrix4& other);

        const Matrix4& operator*=(const Matrix4& other);
        const Matrix4 operator*(const Matrix4& other) const;

        const Matrix4& operator*=(float scalar);
        const Matrix4 operator*(float scalar) const;

        const Matrix4 getTranspose() const;

        float getDet() const;

        const Matrix4 getInverse() const;

        /// \brief Sets the Matrix4 with float buffer.
        /// \param data - The Matrix4 data (column major).
        void setData(const float* data);

        /// \brief Gets the Matrix4 float buffer.
        /// \return The internal Matrix4 data (column major).
        const float* getData() const;

        /// \brief Set a value
        /// \param c - the column
        /// \param l - the line
        void setValue(Uint32 c, Uint32 l, float v);

        /// \brief Get a value
        /// \param c - the column
        /// \param l - the line
        float getValue(Uint32 c, Uint32 l) const;

        static const Matrix4 getPerspectiveProjectionMat(float fovx, float whRatio, float nearValue, float farValue);
        static const Matrix4 getOrthographicProjectionMat(float l, float r, float b, float t, float nearValue, float farValue);
        static const Matrix4 getOrthographicProjectionMat(float width, float height, float nearValue, float farValue);
        static const Matrix4 getViewMat(const Vector3& pos, const Vector3& target, const Vector3& up);
        static const Matrix4 getIdentityMat();
        static const Matrix4 getTranslationMat(float tx, float ty, float tz);
        static const Matrix4 getRotationMat(float rx, float ry, float rz);
        static const Matrix4 getScaleMat(float sx, float sy, float sz);

    protected:
    private:

        float m_data[16];
};

IMPGEARS_END

#endif // IMP_Matrix4_H
