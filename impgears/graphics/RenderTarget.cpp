#include "RenderTarget.h"
#include "EvnContextInterface.h"
#include <cstdio>

#include "camera/SceneCamera.h"

IMPGEARS_BEGIN

RenderTarget::RenderTarget():
    m_type(TargetType_Unkown),
    m_width(0),
    m_height(0),
    m_bpp(0),
    m_hasDepthBuffer(false),
    m_colorTexture(IMP_NULL),
    m_depthTexture(IMP_NULL)
{
}

RenderTarget::~RenderTarget()
{
    destroy();
}

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

void RenderTarget::createBufferTarget(Uint32 width, Uint32 height, bool depthBuffer)
{
    destroy();
    m_type = TargetType_Buffer;
    m_hasDepthBuffer = depthBuffer;


    glGenFramebuffers(1, &m_frameBufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferID);

    m_colorTexture = new Texture();
    m_colorTexture->create(width, height, Texture::Format_RGBA, Texture::MemoryMode_ramAndVideo);
    m_colorTexture->setSmooth(false);
    m_colorTexture->setRepeated(false);
    m_colorTexture->updateGlTex();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTexture->getVideoID(), 0);

    m_depthTexture = new Texture();
    m_depthTexture->create(width, height, Texture::Format_Depth16, Texture::MemoryMode_ramAndVideo);
    m_depthTexture->setSmooth(false);
    m_depthTexture->setRepeated(false);
    m_depthTexture->updateGlTex();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTexture->getVideoID(), 0);

    GLenum DrawBuffers[2] = {GL_COLOR_ATTACHMENT0, GL_DEPTH_ATTACHMENT};

    int drawBuffersSize = 1;
    if(m_hasDepthBuffer)
        ++drawBuffersSize;
    glDrawBuffers(drawBuffersSize, DrawBuffers);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        fprintf(stderr, "[impError] FrameBuffer creation failed.\n");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderTarget::destroy()
{
    if(m_type == TargetType_Buffer)
    {
        glDeleteFramebuffers(1, &m_frameBufferID);
        delete m_colorTexture;
        m_colorTexture = IMP_NULL;

        if(m_hasDepthBuffer)
        {
            delete m_depthTexture;
            m_depthTexture = IMP_NULL;
        }
    }

    m_hasDepthBuffer = false;
    m_type = TargetType_Unkown;
}

const Texture* RenderTarget::asTexture()
{
    if(m_type != TargetType_Buffer)
    {
        fprintf(stderr, "[impError] Render target cannot be convert as texture.\n");
        return IMP_NULL;
    }

    return m_colorTexture;
}

const Texture* RenderTarget::getDepthTexture()
{
    if(m_type != TargetType_Buffer || m_hasDepthBuffer == false)
    {
        fprintf(stderr, "[impError] Render target has not depth texture.\n");
        return IMP_NULL;
    }

    return m_depthTexture;
}

void RenderTarget::bind()
{
    if(m_type != TargetType_Buffer)
        return;

    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferID);
}

void RenderTarget::unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

IMPGEARS_END
