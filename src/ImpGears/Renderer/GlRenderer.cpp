#include <Renderer/GlRenderer.h>
#include <Renderer/CpuRenderPlugin.h>

#include <cstdlib>

IMPGEARS_BEGIN

//--------------------------------------------------------------
GlRenderer::GlRenderer()
	: SceneRenderer()
{
	_renderPlugin = CpuRenderPlugin::create();
	_renderPlugin->init();
}

//--------------------------------------------------------------
GlRenderer::~GlRenderer()
{
}

//--------------------------------------------------------------
void GlRenderer::loadRenderPlugin(const std::string& renderPlugin)
{
	_renderPlugin = PluginManager::open(renderPlugin);
	if(_renderPlugin == nullptr)
	{
		std::cout << "fallback to CPU rendering" << std::endl;
		_renderPlugin = CpuRenderPlugin::create();
	}
	_renderPlugin->init();
}

//---------------------------------------------------------------
LightNode* closest(Node* node, const std::vector<LightNode*>& ls)
{
	Matrix4 m = node->getModelMatrix();
	Vec3 wPos(m(3,0),m(3,1),m(3,2));
	
	LightNode* res = nullptr;
	float dist = -1;
	
	for(auto l : ls)
	{
		float d = (l->_worldPosition - wPos).length2();
		if(dist == -1 || d<dist) { res=l;dist=d; }
	}
	
	return res;
}

//---------------------------------------------------------------
void GlRenderer::render(const Graph::Ptr& scene)
{
	if(_renderTargets == nullptr)
	{
		ImageSampler::Ptr sampler = ImageSampler::create(_target);
		_renderTargets = RenderTarget::create();
		_renderTargets->create({ sampler }, true);
	}
	
	Visitor::Ptr visitor = _visitor;
	_visitor->reset();
	scene->accept(visitor);
	
	RenderQueue::Ptr queue = _visitor->getQueue();
	Matrix4 view;
	if(queue->_camera) view = queue->_camera->getViewMatrix();
	
	Vec3 lightPos(0.0);
	Vec3 lightCol(1.0);
	Vec3 color(1.0);
	Vec3 latt(0.0);
	
	for(int i=0;i<(int)queue->_nodes.size();++i)
	{
		LightNode* light = closest(queue->_nodes[i], queue->_lights);
		if(light)
		{
			lightPos = light->_worldPosition;
			lightCol = light->_color;
			latt[0] = light->_power;
		}
		
		GeoNode* geode = dynamic_cast<GeoNode*>( queue->_nodes[i] );
		ClearNode* clear = dynamic_cast<ClearNode*>( queue->_nodes[i] );
		if(geode)
		{
			Material::Ptr mat = geode->_material;
			latt[1] = mat->_shininess;
			color = mat->_color;
			
			if(mat->_baseColor)
				queue->_states[i]->setUniform("u_sampler_color", mat->_baseColor, 0);
			if(mat->_normalmap)
				queue->_states[i]->setUniform("u_sampler_normal", mat->_normalmap, 1);
			if(mat->_emissive)
				queue->_states[i]->setUniform("u_sampler_emissive", mat->_emissive, 2);
		
			queue->_states[i]->setUniform("u_view", view);
			queue->_states[i]->setUniform("u_lightPos", lightPos);
			queue->_states[i]->setUniform("u_lightCol", lightCol);
			queue->_states[i]->setUniform("u_lightAtt", latt);
			queue->_states[i]->setUniform("u_color", color);
			applyState(queue->_states[i]);
			drawGeometry(geode);
		}
		else if(clear)
		{
			applyClear(clear);
		}
	}
}


//---------------------------------------------------------------
void GlRenderer::applyState(const State::Ptr& state)
{
	if(_renderPlugin == nullptr) return;
		
	_renderPlugin->setCulling(state->getFaceCullingMode());
	_renderPlugin->setBlend(state->getBlendMode());
	_renderPlugin->setLineW(state->getLineWidth());
	_renderPlugin->setDepthTest(state->getDepthTest());
	_renderPlugin->setViewport(state->getViewport());
	
	if(!_direct && _renderTargets)
	{
		RenderTarget::Ptr target = _renderTargets;
		_renderPlugin->init(target);
		_renderPlugin->bind(target);
		target->change();
	}
	else
	{
		_renderPlugin->unbind();
	}
	
	ReflexionModel::Ptr reflexion = state->getReflexion();
	_renderPlugin->load(reflexion);
	_renderPlugin->bind(reflexion);
	const std::map<std::string,Uniform::Ptr>& uniforms = state->getUniforms();
	for(auto u : uniforms) GlRenderer::_renderPlugin->update(reflexion, u.second);
}

//---------------------------------------------------------------
void GlRenderer::applyClear(ClearNode* clearNode)
{
	static ClearNode::Ptr clear;
	if(clear.get() == nullptr)
	{
		clear = ClearNode::create();
		clear->setDepth(1);
		clear->setColor( Vec4(0.0) );
		clear->enableDepth(true);
		clear->enableColor(true);
	}
	if(_renderPlugin != nullptr) _renderPlugin->apply(clear);
}

//---------------------------------------------------------------
void GlRenderer::drawGeometry(GeoNode* geoNode)
{
	if(_renderPlugin != nullptr )
	{
		_renderPlugin->load(geoNode->_geo);
		_renderPlugin->draw(geoNode->_geo);
	}
}

IMPGEARS_END
