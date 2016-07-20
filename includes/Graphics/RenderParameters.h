#ifndef IMP_RENDERPARAMETERS_H
#define IMP_RENDERPARAMETERS_H

#include "Core/impBase.h"
#include "Math/Vector3.h"
#include "Math/Matrix4.h"

#include "Graphics/GLcommon.h"

IMPGEARS_BEGIN

/// \brief Defines the render parameters.
class IMP_API RenderParameters
{
    public:

        enum FaceCullingMode
        {
            FaceCullingMode_None = 0,
            FaceCullingMode_Front,
            FaceCullingMode_Back
        };

        enum BlendMode
        {
            BlendMode_None = 0,
            BlendMode_SrcAlphaBased
        };

        RenderParameters();
        RenderParameters(const RenderParameters& other);
        virtual ~RenderParameters();

        const RenderParameters& operator=(const RenderParameters& other);

        void setClearColor(const Vector3& clearColor);
        const Vector3& getClearColor() const{return m_clearColor;}

        void setFaceCullingMode(FaceCullingMode faceCullingMode){m_faceCullingMode = faceCullingMode;}
        FaceCullingMode getFaceCullingMode() const{return m_faceCullingMode;}

        void setBlendMode(BlendMode blendMode){m_blendMode = blendMode;}
        BlendMode getBlendMode() const{return m_blendMode;}

        void enable() const;
        void disable() const;

        void setPerspectiveProjection(float fovx, float ratio, float nearValue, float farValue);
        void setOrthographicProjection(float left, float right, float bottom, float top, float nearValue, float farValue);

        void setProjectionMatrix(const Matrix4& projection){m_projectionMatrix = projection;}
        const Matrix4& getProjectionMatrix() const {return m_projectionMatrix;}

    protected:
    private:

        Vector3 m_clearColor;
        Matrix4 m_projectionMatrix;
        FaceCullingMode m_faceCullingMode;
        BlendMode m_blendMode;
};

IMPGEARS_END

#endif // IMP_RENDERPARAMETERS_H
