#ifndef IMP_RENDERPARAMETERS_H
#define IMP_RENDERPARAMETERS_H

#include "base/impBase.hpp"
#include "base/Vector3.h"

#include "graphics/GLcommon.h"

IMPGEARS_BEGIN

class RenderParameters
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

    protected:
    private:

        Vector3 m_clearColor;
        FaceCullingMode m_faceCullingMode;
        BlendMode m_blendMode;
};

IMPGEARS_END

#endif // IMP_RENDERPARAMETERS_H
