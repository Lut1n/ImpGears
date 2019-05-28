#include <Renderer/SceneRenderer.h>
#include <Renderer/CpuRenderPlugin.h>

#include <cstdlib>

IMPGEARS_BEGIN

RenderPlugin::Ptr SceneRenderer::s_interface;

//--------------------------------------------------------------
SceneRenderer::SceneRenderer()
{
	s_interface = PluginManager::open("libglPlugin");
	if(s_interface == nullptr) { s_interface = CpuRenderPlugin::create(); std::cout << "fallback to CPU rendering" << std::endl; }
	if(s_interface == nullptr) { std::cout << "fallback failed..." << std::endl; }
	if(s_interface != nullptr) s_interface->init();
	
	_visitor = RenderVisitor::create();
	_direct = true;
}

//--------------------------------------------------------------
SceneRenderer::~SceneRenderer()
{
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
void SceneRenderer::render(const Graph::Ptr& scene)
{	
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
void SceneRenderer::applyState(const State::Ptr& state)
{
	if(s_interface == nullptr) return;
		
	s_interface->setCulling(state->getFaceCullingMode());
	s_interface->setBlend(state->getBlendMode());
	s_interface->setLineW(state->getLineWidth());
	s_interface->setDepthTest(state->getDepthTest());
	s_interface->setViewport(state->getViewport());
	
	if(_direct)
	{
		s_interface->unbind();
	}
	else if(_targets)
	{
		RenderTarget::Ptr target = _targets;
		s_interface->init(target);
		s_interface->bind(target);
		target->change();
	}
	
	ReflexionModel::Ptr reflexion = state->getReflexion();
	s_interface->load(reflexion);
	s_interface->bind(reflexion);
	const std::map<std::string,Uniform::Ptr>& uniforms = state->getUniforms();
	for(auto u : uniforms) SceneRenderer::s_interface->update(reflexion, u.second);
}

//---------------------------------------------------------------
void SceneRenderer::applyClear(ClearNode* clearNode)
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
	if(s_interface != nullptr) s_interface->apply(clear);
}

//---------------------------------------------------------------
void SceneRenderer::drawGeometry(GeoNode* geoNode)
{
	if(s_interface != nullptr )
	{
		s_interface->load(geoNode->_geo);
		s_interface->draw(geoNode->_geo);
	}
}

IMPGEARS_END
