#include "Graphics/BmpLoader.h"
#include "Graphics/DefaultShader.h"
#include "Graphics/GraphicRenderer.h"
#include "Graphics/GraphicState.h"
#include "Graphics/RenderTarget.h"

#include "Core/Timer.h"
#include "Core/State.h"

#include "System/SFMLContextInterface.h"
#include "Graphics/ScreenVertex.h"

#include <System/FileInfo.h>

imp::Texture* texture;

struct ViewerState
{
	imp::State state;
	std::shared_ptr<imp::GraphicRenderer> renderer;
	std::shared_ptr<imp::RenderParameters> screenParameters;
	std::shared_ptr<imp::RenderTarget> screenTarget;

	std::shared_ptr<imp::DefaultShader> defaultShader;
	imp::ScreenVertex* screen;

	imp::EvnContextInterface* evnContext;

	time_t accessLast, modifLast, statusLast;

	std::string filename;
	
	ViewerState()
	{	
		evnContext = new imp::SFMLContextInterface();
		evnContext->createWindow(800,600);
		evnContext->setCursorVisible(0, true);
		
		// screen render target
		screenTarget = std::shared_ptr<imp::RenderTarget>( new imp::RenderTarget() );
		screenTarget->createScreenTarget(0);
		
		renderer = std::shared_ptr<imp::GraphicRenderer>(new imp::GraphicRenderer(0,IMP_NULL) );
		renderer->setCenterCursor(false);
	
		// screen render parameters
		screenParameters = std::shared_ptr<imp::RenderParameters>(new imp::RenderParameters() );
		screenParameters->setOrthographicProjection(0.f, 1.f, 0.f, 1.f, 0.f, 1.f);
		screenParameters->setClearColor(imp::Vector3(0.f, 0.f, 1.f));
		
		defaultShader = std::shared_ptr<imp::DefaultShader>(new imp::DefaultShader() );

		const imp::Matrix4& i4 = imp::Matrix4::getIdentityMat();
		defaultShader->addProjection( &(screenParameters->getProjectionMatrix() ));
		defaultShader->addView(&i4);
		defaultShader->addModel(&i4);
		// defaultShader->addTextureParameter("u_colorTexture", texture, 0);
	
	
		screen = new imp::ScreenVertex();
		screen->getGraphicState()->setTarget(screenTarget);
		screen->getGraphicState()->setParameters(screenParameters);
		screen->getGraphicState()->setShader(defaultShader);
		
		renderer->setSceneRoot(screen);
	}

};

void onEvent_private( imp::EvnContextInterface& evnContext )
{
	// get event
	evnContext.getEvents(0);

	// manage event
	imp::Event event;
	while (evnContext.nextEvent(event))
		imp::State::getInstance()->onEvent(event);
}

void onEvent(imp::EvnContextInterface& evnContext)
{
	// engine event handling
	onEvent_private(evnContext);

	// user event handling
	imp::State* state = imp::State::getInstance();

	if(state->m_pressedKeys[imp::Event::Escape])
		exit(0);

}

void onUpdate(ViewerState& state)
{
	// reload if changed
	time_t access, modif, status;
	imp::getTimeInfo(const_cast<char*>(state.filename.c_str()), access, modif, status);

	if(modif > state.modifLast)
	{
		state.modifLast = modif;
		texture = BmpLoader::loadFromFile( state.filename.c_str() );
		
		state.state.setWindowDim(texture->getWidth(), texture->getHeight());
		state.evnContext->resizeWindow(0, texture->getWidth(), texture->getHeight());
		state.evnContext->setWindowTitle(0, state.filename.c_str());
	}
}

void onDraw(ViewerState& state)
{
}

int main(int argc, char* argv[])
{
	texture = IMP_NULL;
	
	ViewerState v_state;
	
	if(argc<2)
	{
		std::cout << "error : no file path given" << std::endl;
		return 0;
	}

	v_state.filename = argv[1];
	texture = BmpLoader::loadFromFile(v_state.filename.c_str());
	if(texture == IMP_NULL)
	{
		std::cout << "error : BMP file loading has failed" << std::endl;
		return 0;
	}

	v_state.defaultShader->addTextureParameter("u_colorTexture", texture, 0);
	imp::getTimeInfo(const_cast<char*>(v_state.filename.c_str()), v_state.accessLast, v_state.modifLast, v_state.statusLast);
	
	while (v_state.evnContext->isOpen(0))
	{
	
		onEvent(*imp::EvnContextInterface::getInstance());
	
		onUpdate(v_state);
	
		// rendering
		v_state.renderer->renderScene(-1);

		// display
		v_state.evnContext->display(0);
	}

	delete imp::VBOManager::getInstance();

	exit(EXIT_SUCCESS);
}
