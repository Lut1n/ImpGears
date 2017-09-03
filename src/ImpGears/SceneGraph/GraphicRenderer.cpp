#include <SceneGraph/GraphicRenderer.h>
#include <SceneGraph/OpenGL.h>

#include <cstdlib>

IMPGEARS_BEGIN

//--------------------------------------------------------------
GraphicRenderer* GraphicRenderer::instance = NULL;

//--------------------------------------------------------------
GraphicRenderer::GraphicRenderer()
{
    //ctor
    setCenterCursor(false);

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        exit(0);
    }

    int major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);
	fprintf(stdout, "OGL version %d.%d\n", major, minor);
	fprintf(stdout, "OpenGL version supported by this platform (%s): \n", glGetString(GL_VERSION));

    
    // default parameters values
    _parameters.reset(new RenderParameters());
    _parameters->setPerspectiveProjection(60.0, 4.0/3.0, 0.1, 128.0);
    _parameters->setClearColor(imp::Vector3(0.0, 0.0, 0.0));
    _parameters->setClearDepth( 1.0 );
    _parameters->setBlendMode(imp::RenderParameters::BlendMode_SrcAlphaBased);
    _parameters->setFog(RenderParameters::ParamState_Disable);
    _parameters->setFaceCullingMode(RenderParameters::FaceCullingMode_Back);
    
    _state.reset(new GraphicState());
    _state->setParameters(_parameters);
    
    setInstance(this);
}

//--------------------------------------------------------------
GraphicRenderer::~GraphicRenderer()
{
}

//--------------------------------------------------------------
void GraphicRenderer::renderScene(){

	GraphicStatesManager& states = getStateManager();
	states.pushState( _state.get() );

	if(_root != nullptr)
	{
		_root->renderAll();
	}
	
	states.popState();
}

IMPGEARS_END
