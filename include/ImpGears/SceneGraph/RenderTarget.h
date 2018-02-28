#ifndef IMP_RENDERTARGET_H
#define IMP_RENDERTARGET_H

#include <Core/Object.h>
#include <SceneGraph/OpenGL.h>
#include <SceneGraph/Texture.h>

IMPGEARS_BEGIN

/// \brief Defines a render target. A render target can be a texture or a window.
class IMP_API RenderTarget : public Object
{
    public:

        enum TargetType
        {
          TargetType_Unkown = 0,
          TargetType_Screen,
          TargetType_Buffer
        };
		
		Meta_Class(RenderTarget)

        RenderTarget();
        virtual ~RenderTarget();
		
        void createScreenTarget(std::uint32_t width, std::uint32_t height);

        /// \brief Creates a texture render target.
        /// \param width - The width of the internal texture.
        /// \param height - The height of the internal texture.
        /// \param depthBuffer - A second buffer for the depth must be created ?
        void createBufferTarget(std::uint32_t width, std::uint32_t height, std::uint32_t textureCount, bool depthBuffer);

        void destroy();

        bool hasDepthBuffer() const{return m_hasDepthBuffer;}


        Texture* getTexture(std::uint32_t n);
        Texture* getDepthTexture();

        void bind();
        void unbind();

    protected:
    private:

    TargetType m_type;
    std::uint32_t m_width;
    std::uint32_t m_height;
    std::uint32_t m_bpp;

    std::uint32_t m_textureCount;
    bool m_hasDepthBuffer;

    Texture* m_colorTextures[5];
    Texture* m_depthTexture;
    GLuint m_frameBufferID;
	std::uint32_t m_windowID;
};

IMPGEARS_END

#endif // IMP_RENDERTARGET_H
