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

    /// Perspective as default projection
    setPerspectiveProjection();

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

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    Uint32 width = EvnContextInterface::getInstance()->getWidth(windowID);
    Uint32 height = EvnContextInterface::getInstance()->getHeight(windowID);
    glViewport(0, 0, width, height);

    camera->lookAt();

    scene.renderAll(passID);

    m_parameters.disable();
}

//--------------------------------------------------------------
void GraphicRenderer::setPerspectiveProjection()
{
    Uint32 width = EvnContextInterface::getInstance()->getWidth(windowID);
    Uint32 height = EvnContextInterface::getInstance()->getHeight(windowID);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(FRUSTUM_FOVY, width/height, FRUSTUM_NEAR, FRUSTUM_FAR);
}

//--------------------------------------------------------------
void GraphicRenderer::setOrthographicProjection(float left, float right, float bottom, float top)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(left, right, bottom, top, FRUSTUM_NEAR, FRUSTUM_FAR);
}

//--------------------------------------------------------------
const Mat4 GraphicRenderer::getProjectionMatrix() const
{
    float data[16];
    glGetFloatv(GL_PROJECTION_MATRIX, data);

    return Mat4(data);
}

//--------------------------------------------------------------
const Mat4 GraphicRenderer::getModelViewMatrix() const
{
    float data[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, data);

    return Mat4(data);
}

IMPGEARS_END
