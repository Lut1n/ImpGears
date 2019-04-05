#include <SceneGraph/RenderParameters.h>
#include <SceneGraph/OpenGL.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
RenderParameters::RenderParameters()
	: _projection()
	, _viewport(0.0,0.0,1.0,1.0)
	, _faceCullingMode(FaceCullingMode_None)
	, _blendMode(BlendMode_SrcAlphaBased)
	, _lineWidth(1.0)
	, _depthTest(false)
	, _viewportChanged(false)
	, _faceCullingChanged(false)
	, _blendModeChanged(false)
	, _lineWidthChanged(false)
	, _depthTestChanged(false)
{
}

//--------------------------------------------------------------
RenderParameters::RenderParameters(const RenderParameters& other)
	: _projection(other._projection)
	, _viewport(other._viewport)
	, _faceCullingMode(other._faceCullingMode)
	, _blendMode(other._blendMode)
	, _lineWidth(other._lineWidth)
	, _depthTest(other._depthTest)
	, _viewportChanged(other._viewportChanged)
	, _faceCullingChanged(other._faceCullingChanged)
	, _blendModeChanged(other._blendModeChanged)
	, _lineWidthChanged(other._lineWidthChanged)
	, _depthTestChanged(other._depthTestChanged)

{
}

//--------------------------------------------------------------
RenderParameters::~RenderParameters()
{
}

//--------------------------------------------------------------
const RenderParameters& RenderParameters::operator=(const RenderParameters& other)
{
	_projection = other._projection;
	_viewport = other._viewport;
	_faceCullingMode = other._faceCullingMode;
	_blendMode = other._blendMode;
	_lineWidth = other._lineWidth;
	_depthTest = other._depthTest;
	_viewportChanged = other._viewportChanged;
	_faceCullingChanged = other._faceCullingChanged;
	_blendModeChanged = other._blendModeChanged;
	_lineWidthChanged = other._lineWidthChanged;
	_depthTestChanged = other._depthTestChanged;

	return *this;
}

//--------------------------------------------------------------
void RenderParameters::apply() const
{
	
	if(_faceCullingChanged)
	{
		if(_faceCullingMode == FaceCullingMode_None)
		{
			glDisable(GL_CULL_FACE);
		}
		else
		{
			glEnable(GL_CULL_FACE);
			glFrontFace(GL_CCW); // GL_CW or GL_CCW

			if(_faceCullingMode == FaceCullingMode_Back)
				glCullFace(GL_BACK); // GL_FRONT, GL_BACK or GL_FRONT_AND_BACK
			else if(_faceCullingMode == FaceCullingMode_Front)
				glCullFace(GL_FRONT);
		}
	}
	
	if(_blendModeChanged)
	{
		if(_blendMode == BlendMode_None)
		{
			glDisable(GL_BLEND);
		}
		else if(_blendMode == BlendMode_SrcAlphaBased)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
	}
	
	if(_lineWidthChanged)
    {
        glLineWidth(_lineWidth);
    }
	
	if(_depthTestChanged)
	{
		if(_depthTest)
		{
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);
			glDepthMask(GL_TRUE);
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
		}
	}
	
	if(_viewportChanged)
	{
			glViewport(_viewport[0], _viewport[1], _viewport[2], _viewport[3]);
	}
}

//--------------------------------------------------------------
void RenderParameters::setPerspectiveProjection(float fovx, float ratio, float nearValue, float farValue)
{
	_projection = Matrix4::perspectiveProj(fovx, ratio, nearValue, farValue);
}

//--------------------------------------------------------------
void RenderParameters::setOrthographicProjection(float left, float right, float bottom, float top, float nearValue, float farValue)
{
	_projection = Matrix4::orthographicProj(left, right, bottom, top, nearValue, farValue);
}

//--------------------------------------------------------------
void RenderParameters::setDepthTest(bool test)
{
	_depthTest = test;
	_depthTestChanged = true;
}

//--------------------------------------------------------------
void RenderParameters::setViewport(float x, float y, float width, float height)
{
	_viewport = Vec4(x,y,width,height);
	_viewportChanged = true;
}


void RenderParameters::setLineWidth(float lw)
{
    _lineWidth = lw;
    _lineWidthChanged = true;
}

IMPGEARS_END
