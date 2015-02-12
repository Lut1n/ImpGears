#include "GraphicRenderer.h"
#include "../graphics/GLcommon.h"
#include "base/frustumParams.h"

IMPGEARS_BEGIN

//--------------------------------------------------------------
GraphicRenderer* GraphicRenderer::instance = NULL;

//--------------------------------------------------------------
GraphicRenderer::GraphicRenderer(Uint32 windowID, Camera* camera)
{
    //ctor
    this->windowID = windowID;
    this->camera = camera;
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
	fprintf(stdout, "ogl version %d.%d\n", major, minor);
	fprintf(stdout, "OpenGL version supported by this platform (%s): \n", glGetString(GL_VERSION));

	if(!sf::Shader::isAvailable())
    {
        fprintf(stderr, "[impError] shader not supported. Exit...\n");
        exit(0);
    }

    setInstance(this);
}

//--------------------------------------------------------------
GraphicRenderer::~GraphicRenderer()
{
}

//--------------------------------------------------------------
void GraphicRenderer::renderScene(imp::Uint32 passID){

    m_parameters.enable();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(camera != IMP_NULL)
        camera->lookAt();

    scene.renderAll(passID);

    m_parameters.disable();
}

IMPGEARS_END
