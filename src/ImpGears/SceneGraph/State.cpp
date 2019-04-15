#include <SceneGraph/State.h>
#include <SceneGraph/GraphRenderer.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
State::State()
	: _projection()
	, _viewport(0.0,0.0,1.0,1.0)
	, _faceCullingMode(FaceCullingMode_None)
	, _blendMode(BlendMode_SrcAlphaBased)
	, _lineWidth(1.0)
	, _depthTest(false)
	, _target(nullptr)
	, _shader(nullptr)
	, _uniforms()
	, _projectionChanged(false)
	, _viewportChanged(false)
	, _faceCullingChanged(false)
	, _blendModeChanged(false)
	, _lineWidthChanged(false)
	, _depthTestChanged(false)
	, _targetChanged(false)
	, _shaderChanged(false)
	, _uniformsChanged(false)
{
}

//--------------------------------------------------------------
State::State(const State& other)
	: _projection(other._projection)
	, _viewport(other._viewport)
	, _faceCullingMode(other._faceCullingMode)
	, _blendMode(other._blendMode)
	, _lineWidth(other._lineWidth)
	, _depthTest(other._depthTest)
	, _target(other._target)
	, _shader(other._shader)
	, _uniforms(other._uniforms)
	, _projectionChanged(other._projectionChanged)
	, _viewportChanged(other._viewportChanged)
	, _faceCullingChanged(other._faceCullingChanged)
	, _blendModeChanged(other._blendModeChanged)
	, _lineWidthChanged(other._lineWidthChanged)
	, _depthTestChanged(other._depthTestChanged)
	, _targetChanged(other._targetChanged)
	, _shaderChanged(other._shaderChanged)
	, _uniformsChanged(other._uniformsChanged)
{
}

//--------------------------------------------------------------
State::~State()
{
}

//--------------------------------------------------------------
void State::clone(const State::Ptr& other, CloneOpt opt)
{
	if(opt == CloneOpt_All)
	{
		setProjectionMatrix(other->_projection);
		setViewport(other->_viewport);
		setFaceCullingMode(other->_faceCullingMode);
		setBlendMode(other->_blendMode);
		setLineWidth(other->_lineWidth);
		setDepthTest(other->_depthTest);
		setTarget(other->_target);
		setShader(other->_shader);
		setUniforms(other->_uniforms);
	}
	else if(opt == CloneOpt_IfChanged)
	{
		if(other->_projectionChanged) setProjectionMatrix(other->_projection);
		if(other->_viewportChanged) setViewport(other->_viewport);
		if(other->_faceCullingChanged) setFaceCullingMode(other->_faceCullingMode);
		if(other->_blendModeChanged) setBlendMode(other->_blendMode);
		if(other->_lineWidthChanged) setLineWidth(other->_lineWidth);
		if(other->_depthTestChanged) setDepthTest(other->_depthTest);
		if(other->_targetChanged) setTarget(other->_target);
		if(other->_shaderChanged) setShader(other->_shader);
		if(other->_uniformsChanged) setUniforms(other->_uniforms);
	}
}

//--------------------------------------------------------------
const State& State::operator=(const State& other)
{
	_projection = other._projection;
	_viewport = other._viewport;
	_faceCullingMode = other._faceCullingMode;
	_blendMode = other._blendMode;
	_lineWidth = other._lineWidth;
	_depthTest = other._depthTest;
	_target = other._target;
	_shader = other._shader;
	_uniforms = other._uniforms;
	_projectionChanged = other._projectionChanged;
	_viewportChanged = other._viewportChanged;
	_faceCullingChanged = other._faceCullingChanged;
	_blendModeChanged = other._blendModeChanged;
	_lineWidthChanged = other._lineWidthChanged;
	_depthTestChanged = other._depthTestChanged;
	_targetChanged = other._targetChanged;
	_shaderChanged = other._shaderChanged;

	return *this;
}

//--------------------------------------------------------------
void State::apply() const
{
	if(GraphRenderer::s_interface == nullptr) return;
		
		
	if(_faceCullingChanged)
		GraphRenderer::s_interface->setCulling(_faceCullingMode);
	
	if(_blendModeChanged)
		GraphRenderer::s_interface->setBlend(_blendMode);
	
	if(_lineWidthChanged)
		GraphRenderer::s_interface->setLineW(_lineWidth);
	
	if(_depthTestChanged)
		GraphRenderer::s_interface->setDepthTest(_depthTest);
	
	if(_viewportChanged)
		GraphRenderer::s_interface->setViewport(_viewport);
	
	if(_targetChanged && _target != nullptr)
	{
		if(_target->_d == nullptr)
			GraphRenderer::s_interface->init(_target.get());
		GraphRenderer::s_interface->bind(_target->_d);
		_target->change();
	}
	
	if(_shader != nullptr)
	{
		if(_shaderChanged)
		{
			if(_shader->_d == nullptr)
				_shader->_d = GraphRenderer::s_interface->load( _shader.get() );//->vertCode, _shader->fragCode);
			GraphRenderer::s_interface->bind(_shader->_d);
		}
		if(_shader->_d != nullptr && _uniformsChanged)
		{
			for(auto u : _uniforms)
				GraphRenderer::s_interface->update(_shader->_d, u.second.get());
		}
	}
}

//--------------------------------------------------------------
void State::setPerspectiveProjection(float fovx, float ratio, float nearValue, float farValue)
{
	_projection = Matrix4::perspectiveProj(fovx, ratio, nearValue, farValue);
	_projectionChanged=true;
}

//--------------------------------------------------------------
void State::setOrthographicProjection(float left, float right, float bottom, float top, float nearValue, float farValue)
{
	_projection = Matrix4::orthographicProj(left, right, bottom, top, nearValue, farValue);
	_projectionChanged=true;
}

//--------------------------------------------------------------
void State::setDepthTest(bool test)
{
	_depthTest = test;
	_depthTestChanged = true;
}

//--------------------------------------------------------------
void State::setViewport(float x, float y, float width, float height)
{
	_viewport = Vec4(x,y,width,height);
	_viewportChanged = true;
}

//--------------------------------------------------------------
void State::setViewport(const Vec4& viewport)
{
	_viewport = viewport;
	_viewportChanged = true;
}

//--------------------------------------------------------------
void State::setLineWidth(float lw)
{
    _lineWidth = lw;
    _lineWidthChanged = true;
}

//--------------------------------------------------------------
void State::setTarget(Target::Ptr target)
{
	_target = target;
	_targetChanged = true;
}

//--------------------------------------------------------------
void State::setShader(ShaderDsc::Ptr shader)
{
	_shader = shader;
	_shaderChanged = true;
}

//--------------------------------------------------------------
void State::setUniforms(const std::map<std::string,Uniform::Ptr>& uniforms)
{
	for(auto kv : uniforms) _uniforms[kv.first] = kv.second;
	_uniformsChanged = true;
}

//--------------------------------------------------------------
void State::clearUniforms()
{
	_uniforms.clear();
	_uniformsChanged = true;
}

//--------------------------------------------------------------
void State::setUniform(const Uniform::Ptr& uniform)
{
	_uniforms[ uniform->getID() ] = uniform;
	_uniformsChanged = true;
}

IMPGEARS_END
