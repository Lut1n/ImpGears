#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <iostream>

#include "Core/State.h"
#include "Core/frustumParams.h"

#include "System/SFMLContextInterface.h"
#include "System/EntityManager.h"

#include "VBOChunk.h"
#include "VoxelWorld.h"
#include "VoxelWordGenerator.h"

#include "Graphics/BmpLoader.h"
#include "Graphics/StrategicCamera.h"
#include "Graphics/DefaultShader.h"
#include "Graphics/GraphicRenderer.h"
#include "Graphics/RenderTarget.h"

#define FPS_LIMIT 60

using namespace imp;


void onEvent(imp::EvnContextInterface& evnContext){

    evnContext.getEvents(0);

	imp::State* state = imp::State::getInstance();
	
	for(unsigned int i=0; i<Event::Mouse_ButtonCount ; ++i)
	{
		if(state->m_pressedMouseButtons[i] == State::ActionState_Pressed)
			state->m_pressedMouseButtons[i] = State::ActionState_True;
		else if(state->m_pressedMouseButtons[i] == State::ActionState_Released)
			state->m_pressedMouseButtons[i] = State::ActionState_False;
	}
	for(unsigned int i=0; i<Event::KeyCount ; ++i)
	{
		if(state->m_pressedKeys[i] == State::ActionState_Pressed)
			state->m_pressedKeys[i] = State::ActionState_True;
		else if(state->m_pressedKeys[i] == State::ActionState_Released)
			state->m_pressedKeys[i] = State::ActionState_False;
	}

    imp::Event event;
    while (evnContext.nextEvent(event))
	{
        imp::State::getInstance()->onEvent(event);
	}

	if(state->m_pressedKeys[imp::Event::Escape] == State::ActionState_True)
		exit(0);
}

int main(void)
{
	const unsigned int winW = 800;
	const unsigned int winH = 600;

	imp::StrategicCamera cam;
	cam.move(imp::Vector3(-40.0,-130.0,0.0));
	imp::VoxelWorld* world;

    imp::State state;
	state.setWindowDim(winW,winH);

    imp::EvnContextInterface* evnContext = new imp::SFMLContextInterface();
    evnContext->createWindow(winW, winH);
    evnContext->setCursorVisible(0, true);
	evnContext->setWindowTitle(0, "voxel propotype");
	
	
	imp::RenderTarget screenTarget;
	screenTarget.createScreenTarget(0);
	
    imp::GraphicRenderer renderer;
	
	Vector3 color(1.0, 1.0, 1.0);
	imp::DefaultShader shader;
	imp::DefaultShader::_instance = &shader;
	shader.addVector3Parameter("u_color", &color);
	shader.addFloatParameter("u_type", 0);


    /// atlas - color
    imp::Texture* atlasColor = BmpLoader::loadFromFile("data/atlas-color.bmp");
    atlasColor->setSmooth(true);
    atlasColor->setRepeated(false);
    atlasColor->setMipmap(true, 9);
    atlasColor->synchronize();
    imp::EntityManager entityManager;

	// default render parameters
    imp::RenderParameters defaultParameters;
    defaultParameters.setPerspectiveProjection(FRUSTUM_FOVY, 4.f/3.f, FRUSTUM_NEAR, FRUSTUM_FAR);
	defaultParameters.setClearColor(imp::Vector3(0.f, 0.f, 1.f));

	// world generation
    world = new imp::VoxelWorld(128,128,64);
	fprintf(stdout, "world generation...\t");
    imp::VoxelWordGenerator::GetInstance()->Generate(world);
	fprintf(stdout, "OK\n");
	fprintf(stdout, "update chunk vbo...\t");
    world->UpdateAll();
    fprintf(stdout, "gpu memory used : %dMo\n", imp::VBOManager::getInstance()->getMemoryUsed()/1000000);

    cam.initFrustum(winW, winH, FRUSTUM_FOVY*3.14159f/180.f, FRUSTUM_NEAR, FRUSTUM_FAR);
    entityManager.addEntity(&cam);
	
    while (evnContext->isOpen(0))
    {
		onEvent(*imp::EvnContextInterface::getInstance());

		entityManager.updateAll();
		cam.lookAt();
		
		renderer.setRenderParameters(defaultParameters);
		screenTarget.bind();
		defaultParameters.enable();
		defaultParameters.apply();
		shader.enable();
		shader.updateAllParameters();
		shader.setMatrix4Parameter("u_projection", defaultParameters.getProjectionMatrix());
		shader.setMatrix4Parameter("u_view", cam.getViewMatrix());
		shader.setTextureParameter("u_colorTexture", atlasColor, 0);
		renderer.renderScene(0);
		world->render(0);
		shader.disable();
		screenTarget.unbind();

		evnContext->display(0);
    }

    delete imp::VBOManager::getInstance();

    exit(EXIT_SUCCESS);
}
