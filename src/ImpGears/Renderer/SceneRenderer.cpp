#include <Renderer/SceneRenderer.h>
#include <Renderer/CpuRenderPlugin.h>

#include <SceneGraph/GeoNode.h>

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
	static Uniform::Ptr u_view;
	static Uniform::Ptr u_lightPos;
	static Uniform::Ptr u_lightAtt;
	static Uniform::Ptr u_lightCol;
	if(u_view == nullptr)
	{
		u_view = Uniform::create("u_view",Uniform::Type_Mat4);
		u_lightPos = Uniform::create("u_lightPos",Uniform::Type_3f);
		u_lightAtt = Uniform::create("u_lightAtt",Uniform::Type_3f);
		u_lightCol = Uniform::create("u_lightCol",Uniform::Type_3f);
	}
	
	Visitor::Ptr visitor = _visitor;
	_visitor->reset();
	scene->accept(visitor);
	
	RenderQueue::Ptr queue = _visitor->getQueue();
	Matrix4 view;
	if(queue->_camera) view = queue->_camera->getViewMatrix();
	u_view->set( view );
	
	Vec3 latt(0.0);
	
	for(int i=0;i<(int)queue->_nodes.size();++i)
	{
		LightNode* light = closest(queue->_nodes[i], queue->_lights);
		if(light)
		{
			u_lightPos->set( light->_worldPosition );
			u_lightCol->set( light->_color );
			latt[0] = light->_power;
		}
		
		GeoNode* geode = dynamic_cast<GeoNode*>( queue->_nodes[i] );
		if(geode && geode->_material) latt[1] = geode->_material->_shininess;
		else latt[1] = 0.0;
		
		u_lightAtt->set( latt );
		queue->_states[i]->setUniform(u_view);
		queue->_states[i]->setUniform(u_lightPos);
		queue->_states[i]->setUniform(u_lightCol);
		queue->_states[i]->setUniform(u_lightAtt);
		queue->_states[i]->apply();
		queue->_nodes[i]->render();
	}	
}

IMPGEARS_END
