#ifndef IMP_MAT4_H
#define IMP_MAT4_H

#include "base/impBase.hpp"

IMPGEARS_BEGIN

/// \brief Defines an openGL matrix4x4 wrapper.
class Mat4
{
    public:

        Mat4();

        Mat4(const Mat4& other);

        /// \brief Constructor using float buffer.
        /// \param data - The mat4 data (column major).
        Mat4(const float* data);

        virtual ~Mat4();

        const Mat4& operator=(const Mat4& other);

        /// \brief Sets the mat4 with float buffer.
        /// \param data - The mat4 data (column major).
        void setData(const float* data);

        /// \brief Gets the mat4 float buffer.
        /// \return The internal mat4 data (column major).
        const float* getData() const;

        /// \brief Set a value
        /// \param c - the column
        /// \param l - the line
        void setValue(Uint32 c, Uint32 l, float v);

    protected:
    private:

        float m_data[16];
};

IMPGEARS_END

#endif // IMP_MAT4_H
