#include "Graphics/SkyBox.h"

#include "Graphics/GLcommon.h"

#include "Graphics/Camera.h"
#include "Graphics/GraphicRenderer.h"

#include <vector>

IMPGEARS_BEGIN

//--------------------------------------------------------------
SkyBox::SkyBox(const Texture* left, const Texture* right, const Texture* front, const Texture* back, const Texture* top, const Texture* bottom, float scale):
    m_left(left),
    m_right(right),
    m_front(front),
    m_back(back),
    m_top(top),
    m_bottom(bottom),
    m_scale(scale)
{

    std::vector<float> vertexBuffer;

    /// Left
    m_leftOffset = 0;
    vertexBuffer.push_back(-1.f); vertexBuffer.push_back(-1.f); vertexBuffer.push_back(-1.f);
    vertexBuffer.push_back(-1.f); vertexBuffer.push_back(1.f); vertexBuffer.push_back(-1.f);
    vertexBuffer.push_back(-1.f); vertexBuffer.push_back(1.f); vertexBuffer.push_back(1.f);
    vertexBuffer.push_back(-1.f); vertexBuffer.push_back(-1.f); vertexBuffer.push_back(1.f);
    /// Right
    m_rightOffset = vertexBuffer.size()*sizeof(float);
    vertexBuffer.push_back(1.f); vertexBuffer.push_back(1.f); vertexBuffer.push_back(-1.f);
    vertexBuffer.push_back(1.f); vertexBuffer.push_back(-1.f); vertexBuffer.push_back(-1.f);
    vertexBuffer.push_back(1.f); vertexBuffer.push_back(-1.f); vertexBuffer.push_back(1.f);
    vertexBuffer.push_back(1.f); vertexBuffer.push_back(1.f); vertexBuffer.push_back(1.f);
    /// Front
    m_frontOffset = vertexBuffer.size()*sizeof(float);
    vertexBuffer.push_back(-1.f); vertexBuffer.push_back(1.f); vertexBuffer.push_back(-1.f);
    vertexBuffer.push_back(1.f); vertexBuffer.push_back(1.f); vertexBuffer.push_back(-1.f);
    vertexBuffer.push_back(1.f); vertexBuffer.push_back(1.f); vertexBuffer.push_back(1.f);
    vertexBuffer.push_back(-1.f); vertexBuffer.push_back(1.f); vertexBuffer.push_back(1.f);
    /// Back
    m_backOffset = vertexBuffer.size()*sizeof(float);
    vertexBuffer.push_back(1.f); vertexBuffer.push_back(-1.f); vertexBuffer.push_back(-1.f);
    vertexBuffer.push_back(-1.f); vertexBuffer.push_back(-1.f); vertexBuffer.push_back(-1.f);
    vertexBuffer.push_back(-1.f); vertexBuffer.push_back(-1.f); vertexBuffer.push_back(1.f);
    vertexBuffer.push_back(1.f); vertexBuffer.push_back(-1.f); vertexBuffer.push_back(1.f);
    /// Top
    m_topOffset = vertexBuffer.size()*sizeof(float);
    vertexBuffer.push_back(-1.f); vertexBuffer.push_back(1.f); vertexBuffer.push_back(1.f);
    vertexBuffer.push_back(1.f); vertexBuffer.push_back(1.f); vertexBuffer.push_back(1.f);
    vertexBuffer.push_back(1.f); vertexBuffer.push_back(-1.f); vertexBuffer.push_back(1.f);
    vertexBuffer.push_back(-1.f); vertexBuffer.push_back(-1.f); vertexBuffer.push_back(1.f);
    /// Bottom
    m_bottomOffset = vertexBuffer.size()*sizeof(float);
    vertexBuffer.push_back(-1.f); vertexBuffer.push_back(-1.f); vertexBuffer.push_back(-1.f);
    vertexBuffer.push_back(1.f); vertexBuffer.push_back(-1.f); vertexBuffer.push_back(-1.f);
    vertexBuffer.push_back(1.f); vertexBuffer.push_back(1.f); vertexBuffer.push_back(-1.f);
    vertexBuffer.push_back(-1.f); vertexBuffer.push_back(1.f); vertexBuffer.push_back(-1.f);


    /// TexCoord (sames for all)
    m_txCoordOffset = vertexBuffer.size()*sizeof(float);
    vertexBuffer.push_back(0.f); vertexBuffer.push_back(0.f);
    vertexBuffer.push_back(1.f); vertexBuffer.push_back(0.f);
    vertexBuffer.push_back(1.f); vertexBuffer.push_back(1.f);
    vertexBuffer.push_back(0.f); vertexBuffer.push_back(1.f);

    Uint32 vboSize = vertexBuffer.size()*sizeof(float);
    requestVBO(vboSize);

    setData(vertexBuffer.data(), vboSize);
}

//--------------------------------------------------------------
SkyBox::~SkyBox()
{
    releaseVBO();
}

//--------------------------------------------------------------
void SkyBox::render(Uint32 passID)
{
    if(passID != 1)
        return;

    glScalef(m_scale, m_scale, m_scale);

    bindVBO(*this);
    enableTexCoordArray(m_txCoordOffset);

    m_shader.enable();
    m_shader.setProjection(GraphicRenderer::getInstance()->getProjectionMatrix());
    m_shader.setView(Camera::getActiveCamera()->getViewMatrix());
    m_shader.setModel(getModelMatrix());

    /// Left
    m_shader.setTextureParameter("skyTexture", m_left, 0);
    enableVertexArray(m_leftOffset);
    glDrawArrays(GL_QUADS, 0, 4);
    /// Right
    m_shader.setTextureParameter("skyTexture", m_right, 0);
    enableVertexArray(m_rightOffset);
    glDrawArrays(GL_QUADS, 0, 4);
    /// Front
    m_shader.setTextureParameter("skyTexture", m_front, 0);
    enableVertexArray(m_frontOffset);
    glDrawArrays(GL_QUADS, 0, 4);
    /// Back
    m_shader.setTextureParameter("skyTexture", m_back, 0);
    enableVertexArray(m_backOffset);
    glDrawArrays(GL_QUADS, 0, 4);
    /// Top
    m_shader.setTextureParameter("skyTexture", m_top, 0);
    enableVertexArray(m_topOffset);
    glDrawArrays(GL_QUADS, 0, 4);
    /// Bottom
    m_shader.setTextureParameter("skyTexture", m_bottom, 0);
    enableVertexArray(m_bottomOffset);
    glDrawArrays(GL_QUADS, 0, 4);

    m_shader.disable();

    unbindVBO();
}

//--------------------------------------------------------------
void SkyBox::update()
{
    setPosition(Camera::getActiveCamera()->getPosition());
}

//--------------------------------------------------------------
void SkyBox::onEvent(const Event& evn)
{

}

IMPGEARS_END
