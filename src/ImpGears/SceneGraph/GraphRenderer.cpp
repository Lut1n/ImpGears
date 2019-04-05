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

    // default parameters values
    RenderParameters::Ptr params = RenderParameters::create();
    params->setPerspectiveProjection(60.0, 1.0, 0.1, 128.0);
    params->setBlendMode(imp::RenderParameters::BlendMode_SrcAlphaBased);
    params->setFaceCullingMode(RenderParameters::FaceCullingMode_None);
    params->setViewport(0.0,0.0,500.0,500.0);
    params->setLineWidth(1.0);
    params->setDepthTest( true );
    
	_initNode = ClearNode::create();
	_initNode->setDepth(1);
	_initNode->setColor(Vec3(0.0));
	_initNode->enableDepth(true);
	_initNode->enableColor(true);
	
    _state = _initNode->getGraphicState();
    _state->setParameters(params);
	_visitor = SceneVisitor::create();
}

//--------------------------------------------------------------
GraphRenderer::~GraphRenderer()
{
}

//---------------------------------------------------------x-----
void GraphRenderer::renderScene(SceneNode::Ptr& scene)
{
	_visitor->push(_initNode.get());
	Visitor<SceneNode*>::Ptr visitor = _visitor;
	_visitor->apply(_initNode.get());
	scene->accept(visitor);
	_visitor->pop();
}

IMPGEARS_END
