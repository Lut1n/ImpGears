#include "RenderTarget.h"
#include "EvnContextInterface.h"
#include "cstdio"

IMPGEARS_BEGIN

RenderTarget::RenderTarget():
    m_type(TargetType_Unkown),
    m_width(0),
    m_height(0),
    m_bpp(0),
    m_texture(IMP_NULL)
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
}

void RenderTarget::createBufferTarget(Uint32 width, Uint32 height, Uint32 bpp)
{
    destroy();

    m_texture = new Texture();
    m_texture->create(width, height, Texture::Format_RGBA, Texture::MemoryMode_ramAndVideo);
    m_texture->updateGlTex();

    glGenFramebuffers(1, &m_frameBufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferID);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_texture->getVideoID(), 0);
    //glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_texture->getVideoID(), 0);

    GLenum DrawBuffers[2] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers);
    //glDrawBuffer(GL_NONE);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        fprintf(stderr, "[impError] FrameBuffer creation failed.\n");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderTarget::destroy()
{
    if(m_type == TargetType_Buffer)
    {
        glDeleteFramebuffers(1, &m_frameBufferID);
        delete m_texture;
        m_texture = IMP_NULL;
    }

    m_type = TargetType_Unkown;
}

const Texture* RenderTarget::asTexture()
{
    if(m_type != TargetType_Buffer)
    {
        fprintf(stderr, "[impError] Render target cannot be convert as texture.\n");
        return IMP_NULL;
    }

    return m_texture;
}

void RenderTarget::bind()
{
    if(m_type != TargetType_Buffer)
        return;

    m_texture->bind();

    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferID);
}

void RenderTarget::unbind()
{
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

IMPGEARS_END
