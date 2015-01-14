#include "GraphicRenderer.h"
#include "../graphics/GLcommon.h"
#include "base/frustumParams.h"

IMPGEARS_BEGIN

GraphicRenderer* GraphicRenderer::instance = NULL;

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
        exit(-1);
    }

    /// Gl FACE CULLING
    glCullFace(GL_BACK); // GL_FRONT, GL_BACK or GL_FRONT_AND_BACK
    glFrontFace(GL_CCW); // GL_CW or GL_CCW
    glEnable(GL_CULL_FACE);

    /// Enable gl texture
    glEnable(GL_TEXTURE_2D);

    /// Enable Blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /// Enable Z-buffer read and write
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_TRUE);
    glClearDepth(1.f);

    /// Setup a perspective projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    Uint32 width = EvnContextInterface::getInstance()->getWidth(windowID);
    Uint32 height = EvnContextInterface::getInstance()->getHeight(windowID);
    gluPerspective(FRUSTUM_FOVY, width/height, FRUSTUM_NEAR, FRUSTUM_FAR);      /// go to camera

    /// Fog
    //glEnable(GL_FOG);
    GLfloat fogDensity = 0.3f;
    GLfloat fogColor[4] = {0.5f, 0.5f, 0.5f, 1.f};
    glFogi(GL_FOG_MODE, GL_EXP2);
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogf(GL_FOG_DENSITY, fogDensity);
    glHint(GL_FOG_HINT, GL_NICEST);

    setInstance(this);
}

GraphicRenderer::~GraphicRenderer()
{
}

void GraphicRenderer::renderScene(){

    glClearColor(0.4f,0.5f,1.f,1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    Uint32 width = EvnContextInterface::getInstance()->getWidth(windowID);
    Uint32 height = EvnContextInterface::getInstance()->getHeight(windowID);
    glViewport(0, 0, width, height);

    camera->lookAt();

    scene.renderAll();
}

void GraphicRenderer::updateScene(){
    scene.updateAll();

    camera->update();

    if(centerCursor)
    {
        EvnContextInterface* interface = EvnContextInterface::getInstance();
        Uint32 width = interface->getWidth(windowID);
        Uint32 height = interface->getHeight(windowID);

        interface->setCursorPosition(windowID, width/2,height/2);
    }
}

void GraphicRenderer::onEventScene(imp::Event evn){

    scene.onEventAll(evn);

    camera->onEvent(evn);
}

IMPGEARS_END
