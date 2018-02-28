#include <SceneGraph/RenderTarget.h>

#include <cstdio>

#include <SceneGraph/Camera.h>

#include <iostream>

IMPGEARS_BEGIN

//--------------------------------------------------------------
RenderTarget::RenderTarget():
    m_type(TargetType_Unkown),
    m_width(0),
    m_height(0),
    m_bpp(0),
    m_textureCount(0),
    m_hasDepthBuffer(false),
    m_depthTexture(nullptr)
{
    for(std::uint32_t i=0; i<5; ++i)
        m_colorTextures[i] = nullptr;
}

//--------------------------------------------------------------
RenderTarget::~RenderTarget()
{
    destroy();
}

//--------------------------------------------------------------
void RenderTarget::createScreenTarget(std::uint32_t width, std::uint32_t height)
{
    destroy();
    m_type = TargetType_Screen;

    m_width = width;
    m_height = height;
    m_bpp = 32;
    m_hasDepthBuffer = false;
}

//--------------------------------------------------------------
void RenderTarget::createBufferTarget(std::uint32_t width, std::uint32_t height, std::uint32_t textureCount, bool depthBuffer)
{
    destroy();
    m_type = TargetType_Buffer;
    m_hasDepthBuffer = depthBuffer;
    m_width = width;
    m_height = height;
    m_textureCount = textureCount;

    glGenFramebuffers(1, &m_frameBufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferID);
    GL_CHECKERROR("bind FBO");

    GLenum DrawBuffers[5] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4};

    for(std::uint32_t i=0; i<textureCount; ++i)
    {
        m_colorTextures[i] = new Texture();
        m_colorTextures[i]->create(width, height, PixelFormat_RGBA8);
        m_colorTextures[i]->setSmooth(false);
        m_colorTextures[i]->setRepeated(false);
        m_colorTextures[i]->synchronize();
        m_colorTextures[i]->bind();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_2D, m_colorTextures[i]->getVideoID(), 0);
        GL_CHECKERROR("set color buffer");
    }

    if(m_hasDepthBuffer)
    {
        m_depthTexture = new Texture();
        m_depthTexture->create(width, height, PixelFormat_R16);
        m_depthTexture->setSmooth(false);
        m_depthTexture->setRepeated(false);
        m_depthTexture->synchronize();
        m_depthTexture->bind();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture->getVideoID(), 0);
        GL_CHECKERROR("set depth buffer");
    }

    glDrawBuffers(textureCount, DrawBuffers);
    GL_CHECKERROR("set draw buffers");

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        fprintf(stderr, "[impError] FrameBuffer creation failed.\n");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

//--------------------------------------------------------------
void RenderTarget::destroy()
{
    if(m_type == TargetType_Buffer)
    {
        glDeleteFramebuffers(1, &m_frameBufferID);

        for(std::uint32_t i=0; i<m_textureCount; ++i)
        {
            delete m_colorTextures[i];
            m_colorTextures[i] = nullptr;
        }

        if(m_hasDepthBuffer)
        {
            delete m_depthTexture;
            m_depthTexture = nullptr;
        }
    }

    m_textureCount = 0;
    m_hasDepthBuffer = false;
    m_type = TargetType_Unkown;
}

//--------------------------------------------------------------
Texture* RenderTarget::getTexture(std::uint32_t n)
{
    if(m_type != TargetType_Buffer)
    {
        fprintf(stderr, "[impError] Render target has not texture buffer.\n");
        return nullptr;
    }

    return m_colorTextures[n];
}

//--------------------------------------------------------------
Texture* RenderTarget::getDepthTexture()
{
    if(m_type != TargetType_Buffer || m_hasDepthBuffer == false)
    {
        fprintf(stderr, "[impError] Render target has not depth texture.\n");
        return nullptr;
    }

    return m_depthTexture;
}

//--------------------------------------------------------------
void RenderTarget::bind()
{
    if(m_type == TargetType_Buffer)
    { 
        glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferID);
    }
    else{
        unbind();
    }

    glViewport(0, 0, m_width, m_height);
}

//--------------------------------------------------------------
void RenderTarget::unbind()
{
    if(m_type == TargetType_Buffer)
    {
        for(std::uint32_t i=0; i<m_textureCount; ++i)
            {
                m_colorTextures[i]->notifyTextureRendering();
                m_colorTextures[i]->synchronize();
            }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

IMPGEARS_END
