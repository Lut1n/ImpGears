#include "scene/RenderParameters.h"

IMPGEARS_BEGIN

//--------------------------------------------------------------
RenderParameters::RenderParameters():
    m_faceCullingMode(FaceCullingMode_Back),
    m_blendMode(BlendMode_SrcAlphaBased)
{
    m_clearColor = Vector3(1.f, 1.f, 1.f); //alpha = 1.f
    m_projectionMatrix = Matrix4::getPerspectiveProjectionMat(45.f, 4.f/3.f, 0.1f, 100.f);
}

//--------------------------------------------------------------
RenderParameters::RenderParameters(const RenderParameters& other):
    m_faceCullingMode(other.m_faceCullingMode),
    m_blendMode(other.m_blendMode)
{
    m_clearColor = other.m_clearColor;
    m_projectionMatrix = other.m_projectionMatrix;
}

//--------------------------------------------------------------
RenderParameters::~RenderParameters()
{
    disable();
}

//--------------------------------------------------------------
const RenderParameters& RenderParameters::operator=(const RenderParameters& other)
{
    m_faceCullingMode = other.m_faceCullingMode;
    m_blendMode = other.m_blendMode;
    m_clearColor = other.m_clearColor;
    m_projectionMatrix = other.m_projectionMatrix;

    return *this;
}

//--------------------------------------------------------------
void RenderParameters::setClearColor(const Vector3& clearColor)
{
    m_clearColor = clearColor;
}

//--------------------------------------------------------------
void RenderParameters::enable() const
{
    glClearColor(m_clearColor.getX(), m_clearColor.getY(), m_clearColor.getZ(), 0.f);

    /// Face culling mode
    if(m_faceCullingMode == FaceCullingMode_None)
    {
        glDisable(GL_CULL_FACE);
    }
    else
    {
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW); // GL_CW or GL_CCW

        if(m_faceCullingMode == FaceCullingMode_Back)
            glCullFace(GL_BACK); // GL_FRONT, GL_BACK or GL_FRONT_AND_BACK
        else if(m_faceCullingMode == FaceCullingMode_Front)
            glCullFace(GL_FRONT);
    }

    /// Blending mode
    if(m_blendMode == BlendMode_None)
    {
        glDisable(GL_BLEND);
    }
    else if(m_blendMode == BlendMode_SrcAlphaBased)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    /// Enable Z-buffer
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_TRUE);
    glClearDepth(1.f);

    /// Fog
    glDisable(GL_FOG);
    /*glEnable(GL_FOG);
    GLfloat fogDensity = 2.f;
    GLfloat fogColor[4] = {0.8f, 0.8f, 0.8f, 1.f};
    glFogi(GL_FOG_MODE, GL_EXP2);
    //glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogf(GL_FOG_DENSITY, fogDensity);
    glFogf(GL_FOG_START, 1.f);
    glFogf(GL_FOG_END, 100.f);
    glHint(GL_FOG_HINT, GL_NICEST);*/
}

//--------------------------------------------------------------
void RenderParameters::disable() const
{
}

//--------------------------------------------------------------
void RenderParameters::setPerspectiveProjection(float fovx, float ratio, float nearValue, float farValue)
{
    m_projectionMatrix = Matrix4::getPerspectiveProjectionMat(fovx, ratio, nearValue, farValue);
}

//--------------------------------------------------------------
void RenderParameters::setOrthographicProjection(float left, float right, float bottom, float top, float nearValue, float farValue)
{
    m_projectionMatrix = Matrix4::getOrthographicProjectionMat(left, right, bottom, top, nearValue, farValue);
}

IMPGEARS_END
