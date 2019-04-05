#include <SceneGraph/GraphRenderer.h>
#include <SceneGraph/OpenGL.h>

#include <cstdlib>

IMPGEARS_BEGIN

//--------------------------------------------------------------
GraphRenderer::GraphRenderer()
{
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        std::cout << "Error: " << glewGetErrorString(err) << std::endl;
        exit(0);
    }

    int major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);
	std::cout << "OGL version " << major << "." << minor << std::endl;
	std::cout << "OpenGL version supported by this platform (" << glGetString(GL_VERSION) << ")" << std::endl;
    
	_initNode = ClearNode::create();
	_initNode->setDepth(1);
	_initNode->setColor(Vec3(0.0));
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

IMPGEARS_END
