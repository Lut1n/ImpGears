#include <SceneGraph/GraphRenderer.h>

#include <SceneGraph/CpuRenderPlugin.h>

#include <cstdlib>

IMPGEARS_BEGIN

RenderPlugin::Ptr GraphRenderer::s_interface;

//--------------------------------------------------------------
GraphRenderer::GraphRenderer()
{
	s_interface = PluginManager::open("libglPlugin");
	if(s_interface == nullptr) { s_interface = CpuRenderPlugin::create(); std::cout << "fallback to CPU rendering" << std::endl; }
	if(s_interface == nullptr) { std::cout << "fallback failed..." << std::endl; }
	if(s_interface != nullptr) s_interface->init();
    
	_initNode = ClearNode::create();
	_initNode->setDepth(1);
	_initNode->setColor( Vec4(0.0) );
	_initNode->enableDepth(true);
	_initNode->enableColor(true);
	
    // default parameters values
    _initState = _initNode->getState();
    _initState->setPerspectiveProjection(60.0, 1.0, 0.1, 128.0);
    _initState->setBlendMode(State::BlendMode_SrcAlphaBased);
    _initState->setFaceCullingMode(State::FaceCullingMode_None);
    _initState->setViewport(0.0,0.0,500.0,500.0);
    _initState->setLineWidth(1.0);
    _initState->setDepthTest( true );
	
	_visitor = SceneVisitor::create();
}

//--------------------------------------------------------------
GraphRenderer::~GraphRenderer()
{
}

//---------------------------------------------------------------
void GraphRenderer::renderScene(SceneNode::Ptr& scene)
{
	Visitor<SceneNode*>::Ptr visitor = _visitor;
	
	_visitor->push(_initNode.get());
	_visitor->apply(_initNode.get());
	scene->accept(visitor);
	_visitor->pop();
}

//---------------------------------------------------------------
void GraphRenderer::setClearColor(const Vec4& color)
{
	_initNode->setColor(color);
}

//---------------------------------------------------------------
void GraphRenderer::setClearDepth(float depth)
{
	_initNode->setDepth(depth);
}

IMPGEARS_END
