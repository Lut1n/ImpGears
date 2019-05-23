#include <SceneGraph/State.h>
#include <Renderer/SceneRenderer.h>

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
		_projection = other->_projection;
		_viewport = other->_viewport;
		_faceCullingMode = other->_faceCullingMode;
		_blendMode = other->_blendMode;
		_lineWidth = other->_lineWidth;
		_depthTest = other->_depthTest;
		_target = other->_target;
		_shader = other->_shader;
		_uniforms = other->_uniforms;
		_projectionChanged = other->_projectionChanged;
		_viewportChanged = other->_viewportChanged;
		_faceCullingChanged = other->_faceCullingChanged;
		_blendModeChanged = other->_blendModeChanged;
		_lineWidthChanged = other->_lineWidthChanged;
		_depthTestChanged = other->_depthTestChanged;
		_targetChanged = other->_targetChanged;
		_shaderChanged = other->_shaderChanged;
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
	if(SceneRenderer::s_interface == nullptr) return;
		
		
	if(_faceCullingChanged)
		SceneRenderer::s_interface->setCulling(_faceCullingMode);
	
	if(_blendModeChanged)
		SceneRenderer::s_interface->setBlend(_blendMode);
	
	if(_lineWidthChanged)
		SceneRenderer::s_interface->setLineW(_lineWidth);
	
	if(_depthTestChanged)
		SceneRenderer::s_interface->setDepthTest(_depthTest);
	
	if(_viewportChanged)
		SceneRenderer::s_interface->setViewport(_viewport);
	
	if(_targetChanged)
	{
		if(_target == nullptr)
		{
			SceneRenderer::s_interface->unbind(nullptr);
		}
		else 
		{
			if(_target->_d == -1)
			{
				SceneRenderer::s_interface->init(_target.get());
				_target->_d = 0;
			}
			SceneRenderer::s_interface->bind(_target.get());
			_target->change();
		}
	}
	
	if(_shader != nullptr)
	{
		if(_shaderChanged)
		{
			if(_shader->_d == -1)
			{
				SceneRenderer::s_interface->load( _shader.get() );//->vertCode, _shader->fragCode);
				_shader->_d = 0;
			}
			SceneRenderer::s_interface->bind(_shader.get());
		}
		if(_shader->_d != -1 && _uniformsChanged)
		{
			for(auto u : _uniforms)
				SceneRenderer::s_interface->update(_shader.get(), u.second);
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
void State::setShader(ReflexionModel::Ptr shader)
{
	_shader = shader;
	_shaderChanged = true;
}

//--------------------------------------------------------------
void State::setUniforms(const std::map<std::string,Uniform::Ptr>& uniforms)
{
	clearUniforms();
	for(auto kv : uniforms)
	{
		_uniforms[kv.first] = Uniform::create("no id",Uniform::Type_Undefined);
		_uniforms[kv.first]->clone( *kv.second );
	}
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
	_uniforms[ uniform->getID() ] =  Uniform::create("no id",Uniform::Type_Undefined);
	_uniforms[ uniform->getID() ]->clone( *uniform );
	_uniformsChanged = true;
}

//--------------------------------------------------------------
void State::setUniform(const std::string&& name, int i1)
{
	_uniforms[ name ] =  Uniform::create(name,Uniform::Type_1i);
	_uniforms[ name ]->set(i1);
	_uniformsChanged = true;
}

//--------------------------------------------------------------
void State::setUniform(const std::string&& name, float f1)
{
	_uniforms[ name ] =  Uniform::create(name,Uniform::Type_1f);
	_uniforms[ name ]->set(f1);
	_uniformsChanged = true;
}

//--------------------------------------------------------------
void State::setUniform(const std::string&& name, const Vec2& f2)
{
	_uniforms[ name ] =  Uniform::create(name,Uniform::Type_2f);
	_uniforms[ name ]->set(f2);
	_uniformsChanged = true;
}

//--------------------------------------------------------------
void State::setUniform(const std::string&& name, const Vec3& f3)
{
	_uniforms[ name ] =  Uniform::create(name,Uniform::Type_3f);
	_uniforms[ name ]->set(f3);
	_uniformsChanged = true;
}

//--------------------------------------------------------------
void State::setUniform(const std::string&& name, const Vec4& f4)
{
	_uniforms[ name ] =  Uniform::create(name,Uniform::Type_4f);
	_uniforms[ name ]->set(f4);
	_uniformsChanged = true;
}

//--------------------------------------------------------------
void State::setUniform(const std::string&& name, const Matrix3& mat3)
{
	_uniforms[ name ] =  Uniform::create(name,Uniform::Type_Mat3);
	_uniforms[ name ]->set(mat3);
	_uniformsChanged = true;
}

//--------------------------------------------------------------
void State::setUniform(const std::string&& name, const Matrix4& mat4)
{
	_uniforms[ name ] =  Uniform::create(name,Uniform::Type_Mat4);
	_uniforms[ name ]->set(mat4);
	_uniformsChanged = true;
}

//--------------------------------------------------------------
void State::setUniform(const std::string&& name, const ImageSampler::Ptr& sampler, int index)
{
	_uniforms[ name ] =  Uniform::create(name,Uniform::Type_Sampler);
	_uniforms[ name ]->set(sampler, index);
	_uniformsChanged = true;
}

IMPGEARS_END
