#include "RenderTarget.h"
#include "EvnContextInterface.h"
#include <cstdio>

#include "camera/SceneCamera.h"

IMPGEARS_BEGIN

//--------------------------------------------------------------
RenderTarget::RenderTarget():
    m_type(TargetType_Unkown),
    m_width(0),
    m_height(0),
    m_bpp(0),
    m_textureCount(0),
    m_hasDepthBuffer(false),
    m_depthTexture(IMP_NULL)
{
    for(Uint32 i=0; i<5; ++i)
        m_colorTextures[i] = IMP_NULL;
}

//--------------------------------------------------------------
RenderTarget::~RenderTarget()
{
    destroy();
}

//--------------------------------------------------------------
void RenderTarget::createScreenTarget(Uint32 windowID)
{
    destroy();
    m_type = TargetType_Screen;

    EvnContextInterface* evn = EvnContextInterface::getInstance();
    m_width = evn->getWidth(windowID);
    m_height = evn->getHeight(windowID);
    m_bpp = 32;
    m_hasDepthBuffer = false;
}

//--------------------------------------------------------------
void RenderTarget::createBufferTarget(Uint32 width, Uint32 height, Uint32 textureCount, bool depthBuffer)
{
    destroy();
    m_type = TargetType_Buffer;
    m_hasDepthBuffer = depthBuffer;

    glGenFramebuffers(1, &m_frameBufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferID);
    GL_CHECKERROR("bind FBO");

    GLenum DrawBuffers[5] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4};

    for(Uint32 i=0; i<textureCount; ++i)
    {
        m_colorTextures[i] = new Texture();
        m_colorTextures[i]->create(width, height, Texture::Format_RGBA, Texture::MemoryMode_ramAndVideo);
        m_colorTextures[i]->setSmooth(false);
        m_colorTextures[i]->setRepeated(false);
        m_colorTextures[i]->updateGlTex();
        m_colorTextures[i]->bind();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_2D, m_colorTextures[i]->getVideoID(), 0);
        GL_CHECKERROR("set color buffer");
    }

    if(m_hasDepthBuffer)
    {
        m_depthTexture = new Texture();
        m_depthTexture->create(width, height, Texture::Format_Depth16, Texture::MemoryMode_ramAndVideo);
        m_depthTexture->setSmooth(false);
        m_depthTexture->setRepeated(false);
        m_depthTexture->updateGlTex();
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

        for(Uint32 i=0; i<m_textureCount; ++i)
        {
            delete m_colorTextures[i];
            m_colorTextures[i] = IMP_NULL;
        }

        if(m_hasDepthBuffer)
        {
            delete m_depthTexture;
            m_depthTexture = IMP_NULL;
        }
    }

    m_textureCount = 0;
    m_hasDepthBuffer = false;
    m_type = TargetType_Unkown;
}

//--------------------------------------------------------------
const Texture* RenderTarget::getTexture(Uint32 n)
{
    if(m_type != TargetType_Buffer)
    {
        fprintf(stderr, "[impError] Render target has not texture buffer.\n");
        return IMP_NULL;
    }

    return m_colorTextures[n];
}

//--------------------------------------------------------------
const Texture* RenderTarget::getDepthTexture()
{
    if(m_type != TargetType_Buffer || m_hasDepthBuffer == false)
    {
        fprintf(stderr, "[impError] Render target has not depth texture.\n");
        return IMP_NULL;
    }

    return m_depthTexture;
}

//--------------------------------------------------------------
void RenderTarget::bind()
{
    if(m_type != TargetType_Buffer)
        return;

    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferID);
}

//--------------------------------------------------------------
void RenderTarget::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

IMPGEARS_END
