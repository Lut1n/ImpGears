#ifndef IMP_RENDERTARGET_H
#define IMP_RENDERTARGET_H

#include "base/impBase.hpp"
#include "graphics/GLcommon.h"
#include "graphics/Texture.h"

IMPGEARS_BEGIN

/// \brief Defines a render target. A render target can be a texture or a window.
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

        /// \brief Creates a window render target.
        /// \param windowID - The ID of the window.
        void createScreenTarget(Uint32 windowID);

        /// \brief Creates a texture render target.
        /// \param width - The width of the internal texture.
        /// \param height - The height of the internal texture.
        /// \param depthBuffer - A second buffer for the depth must be created ?
        void createBufferTarget(Uint32 width, Uint32 height, Uint32 textureCount, bool depthBuffer);

        void destroy();

        bool hasDepthBuffer() const{return m_hasDepthBuffer;}


        Texture* getTexture(Uint32 n);
        Texture* getDepthTexture();

        void bind();
        void unbind();

    protected:
    private:

    TargetType m_type;
    Uint32 m_width;
    Uint32 m_height;
    Uint32 m_bpp;

    Uint32 m_textureCount;
    bool m_hasDepthBuffer;

    Texture* m_colorTextures[5];
    Texture* m_depthTexture;
    GLuint m_frameBufferID;
};

IMPGEARS_END

#endif // IMP_RENDERTARGET_H
