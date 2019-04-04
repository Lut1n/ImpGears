#include <SceneGraph/GraphicRenderer.h>
#include <SceneGraph/OpenGL.h>
#include <SceneGraph/SceneVisitor.h>

#include <cstdlib>

IMPGEARS_BEGIN

//--------------------------------------------------------------
GraphicRenderer::GraphicRenderer()
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
    _parameters.reset(new RenderParameters());
    _parameters->setPerspectiveProjection(60.0, 1.0, 0.1, 128.0);
    _parameters->setClearColor(imp::Vec3(0.0, 0.0, 0.0));
    _parameters->setClearDepth( 1.0 );
    _parameters->setBlendMode(imp::RenderParameters::BlendMode_SrcAlphaBased);
    _parameters->setFog(RenderParameters::ParamState_Disable);
    _parameters->setFaceCullingMode(RenderParameters::FaceCullingMode_None);
    _parameters->setViewport(0.0,0.0,500.0,500.0);
    _parameters->setLineWidth(1.0);
    
    _state.reset(new GraphicState());
    _state->setParameters(_parameters);
}

//--------------------------------------------------------------
GraphicRenderer::~GraphicRenderer()
{
}

//--------------------------------------------------------------
void GraphicRenderer::renderScene(SceneNode::Ptr& scene)
{
	GraphicStatesManager& states = getStateManager();
	states.pushState( _state.get() );

	SceneVisitor::Ptr sceneVis = SceneVisitor::create(&states);
	Visitor<SceneNode*>::Ptr visitor = sceneVis;
	scene->accept(visitor);
	sceneVis->synchronizeStack();
	
	states.popState();
}

IMPGEARS_END
