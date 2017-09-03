#include <SceneGraph/RenderParameters.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
RenderParameters::RenderParameters():
    m_faceCullingMode(FaceCullingMode_Back),
    m_blendMode(BlendMode_SrcAlphaBased),
	_clearColorChanged(false),
	_clearDepthChanged(false),
	_faceCullingChanged(false),
	_blendModeChanged(false),
	_fogChanged(false)
{
    m_clearColor = Vector3(1.f, 1.f, 1.f); //alpha = 1.f
	_clearDepth = 1.f;
	_fogState = ParamState_Disable;
    m_projectionMatrix = Matrix4::getPerspectiveProjectionMat(45.f, 4.f/3.f, 0.1f, 100.f);
}

//--------------------------------------------------------------
RenderParameters::RenderParameters(const RenderParameters& other):
    m_faceCullingMode(other.m_faceCullingMode),
    m_blendMode(other.m_blendMode),
	_clearColorChanged(other._clearColorChanged),
	_clearDepthChanged(other._clearDepthChanged),
	_faceCullingChanged(other._faceCullingChanged),
	_blendModeChanged(other._blendModeChanged),
	_fogChanged(other._fogChanged)

{
    m_clearColor = other.m_clearColor;
    m_projectionMatrix = other.m_projectionMatrix;
	_clearDepth = other._clearDepth;
	_fogState = other._fogState;
}

//--------------------------------------------------------------
RenderParameters::~RenderParameters()
{
}

//--------------------------------------------------------------
const RenderParameters& RenderParameters::operator=(const RenderParameters& other)
{
    m_faceCullingMode = other.m_faceCullingMode;
    m_blendMode = other.m_blendMode;
    m_clearColor = other.m_clearColor;
    m_projectionMatrix = other.m_projectionMatrix;
	_clearDepth = other._clearDepth;
	_fogState = other._fogState;
	
	_clearColorChanged = other._clearColorChanged;
	_clearDepthChanged = other._clearDepthChanged;
	_faceCullingChanged = other._faceCullingChanged;
	_blendModeChanged = other._blendModeChanged;
	_fogChanged = other._fogChanged;


    return *this;
}

//--------------------------------------------------------------
void RenderParameters::setClearColor(const Vector3& clearColor)
{
    m_clearColor = clearColor;
	_clearColorChanged = true;
}

//--------------------------------------------------------------
void RenderParameters::apply(bool clearBuffer) const
{
	if(_clearColorChanged)
	{
		glClearColor(m_clearColor.getX(), m_clearColor.getY(), m_clearColor.getZ(), 0.f);
	}
	
	if(_faceCullingChanged)
	{
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
	}
	
	if(_blendModeChanged)
	{
		if(m_blendMode == BlendMode_None)
		{
			glDisable(GL_BLEND);
		}
		else if(m_blendMode == BlendMode_SrcAlphaBased)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
	}
	
	if(_clearDepthChanged)
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glDepthMask(GL_TRUE);
		glClearDepth(_clearDepth);
	}
	
	if(_fogChanged)
	{
		if(_fogState == ParamState_Disable)
		{
			glDisable(GL_FOG);
		}
		else
		{
			glEnable(GL_FOG);
			GLfloat fogColor[4] = {0.8f, 0.8f, 0.8f, 1.f};
			glFogfv(GL_FOG_COLOR, fogColor);
			glFogi(GL_FOG_MODE, GL_EXP2);
			//glFogi(GL_FOG_MODE, GL_LINEAR);
			GLfloat fogDensity = 2.f;
			glFogf(GL_FOG_DENSITY, fogDensity);
			glFogf(GL_FOG_START, 1.f);
			glFogf(GL_FOG_END, 100.f);
			glHint(GL_FOG_HINT, GL_NICEST);
		}
	}
	
	if(clearBuffer && (_clearColorChanged || _clearDepthChanged) )
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
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
		
//--------------------------------------------------------------
void RenderParameters::setFog(ParamState state)
{
	_fogState = state;
	_fogChanged = true;
}

//--------------------------------------------------------------
void RenderParameters::setClearDepth(float depth)
{
	_clearDepth = depth;
	_clearDepthChanged = true;
}

IMPGEARS_END
