#include "Graphics/GraphicRenderer.h"
#include "Graphics/GLcommon.h"
#include "Core/frustumParams.h"

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
	fprintf(stdout, "ogl version %d.%d\n", major, minor);
	fprintf(stdout, "OpenGL version supported by this platform (%s): \n", glGetString(GL_VERSION));

    setInstance(this);
}

//--------------------------------------------------------------
GraphicRenderer::~GraphicRenderer()
{
}

//--------------------------------------------------------------
void GraphicRenderer::renderScene(imp::Uint32 passID){

    m_parameters.enable();

	if(_root != nullptr)
	{
		_root->renderAll(passID);
	}

    m_parameters.disable();
}

IMPGEARS_END
