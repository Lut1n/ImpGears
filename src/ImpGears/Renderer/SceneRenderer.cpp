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
}

//--------------------------------------------------------------
SceneRenderer::~SceneRenderer()
{
}

//---------------------------------------------------------------
void SceneRenderer::render(const Graph::Ptr& scene)
{
	static Uniform::Ptr u_view;
	if(u_view == nullptr) u_view = Uniform::create("u_view",Uniform::Type_Mat4);
	
	Visitor::Ptr visitor = _visitor;
	_visitor->reset();
	scene->accept(visitor);
	
	RenderQueue::Ptr queue = _visitor->getQueue();
	Matrix4 view;
	if(queue->_camera) view = queue->_camera->getViewMatrix();
	u_view->set( view );
		
	for(int i=0;i<(int)queue->_nodes.size();++i)
	{
		queue->_states[i]->setUniform(u_view);
		queue->_states[i]->apply();
		queue->_nodes[i]->render();
	}	
}

IMPGEARS_END
