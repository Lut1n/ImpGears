#ifndef IMP_RENDERTARGET_H
#define IMP_RENDERTARGET_H

#include "base/impBase.hpp"
#include "graphics/GLcommon.h"
#include "graphics/Texture.h"

IMPGEARS_BEGIN

class RenderTarget
{
    public:

        enum TargetType
        {
          TargetType_Unkown = 0,
          TargetType_Screen,
          TargetType_Buffer
        };

        RenderTarget();
        virtual ~RenderTarget();

        void createScreenTarget(Uint32 windowID);
        void createBufferTarget(Uint32 width, Uint32 height, bool depthBuffer);

        void destroy();

        bool hasDepthBuffer() const{return m_hasDepthBuffer;}

        const Texture* asTexture();
        const Texture* getDepthTexture();

        void bind();
        void unbind();

    protected:
    private:

    TargetType m_type;
    Uint32 m_width;
    Uint32 m_height;
    Uint32 m_bpp;
    bool m_hasDepthBuffer;

    Texture* m_colorTexture;
    Texture* m_depthTexture;
    GLuint m_frameBufferID;
};

IMPGEARS_END

#endif // IMP_RENDERTARGET_H
