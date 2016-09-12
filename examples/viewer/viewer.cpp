#include "Graphics/BmpLoader.h"
#include "Graphics/DefaultShader.h"
#include "Graphics/GraphicRenderer.h"
#include "Graphics/RenderTarget.h"

#include "Core/Timer.h"
#include "Core/State.h"

#include "System/SFMLContextInterface.h"
#include "Graphics/ScreenVertex.h"

#include <System/FileInfo.h>


void onEvent(imp::EvnContextInterface& evnContext)
{
	evnContext.getEvents(0);

	imp::Event event;
	while (evnContext.nextEvent(event))
		imp::State::getInstance()->onEvent(event);

	imp::State* state = imp::State::getInstance();

	if(state->m_pressedKeys[imp::Event::Escape])
		exit(0);
}

int main(int argc, char* argv[])
{
	imp::Timer fpsTimer;
	imp::State state;

	imp::EvnContextInterface* evnContext = new imp::SFMLContextInterface();
	evnContext->createWindow(800,600);
	evnContext->setCursorVisible(0, true);

	imp::GraphicRenderer renderer(0, NULL);
	renderer.setCenterCursor(false);

	if(argc<2)
		return 0;

	imp::Texture* texture = BmpLoader::loadFromFile(argv[1]);
	if(texture == IMP_NULL)
		return 0;


	time_t accessLast, modifLast, statusLast;
	imp::getTimeInfo(argv[1], accessLast, modifLast, statusLast);

	state.setWindowDim(texture->getWidth(), texture->getHeight());
	//state.setWindowDim(800, 600);

	 evnContext->resizeWindow(0, texture->getWidth(), texture->getHeight());
	evnContext->setWindowTitle(0, argv[1]);

	// screen render parameters
	imp::RenderParameters screenParameters;
	screenParameters.setOrthographicProjection(0.f, 1.f, 0.f, 1.f, 0.f, 1.f);
	screenParameters.setClearColor(imp::Vector3(0.f, 0.f, 1.f));

	// screen render target
	imp::RenderTarget screenTarget;
	screenTarget.createScreenTarget(0);

	imp::Matrix4 i4 = imp::Matrix4::getIdentityMat();

	imp::DefaultShader defaultShader;
	imp::ScreenVertex screen;

	while (evnContext->isOpen(0))
	{
		onEvent(*imp::EvnContextInterface::getInstance());

		// reload if changed
		time_t access, modif, status;
		imp::getTimeInfo(argv[1], access, modif, status);
		// std::cout << "MODIF time " << modif << "; " << modifLast << "\n";
		if(modif > modifLast)
		{
			modifLast = modif;
			texture = BmpLoader::loadFromFile(argv[1]);
		}

		renderer.setCamera(IMP_NULL);
		renderer.setRenderParameters(screenParameters);
		screenTarget.bind();
		defaultShader.enable();
		defaultShader.setMatrix4Parameter("u_projection", renderer.getProjectionMatrix());
		defaultShader.setMatrix4Parameter("u_view", i4);
		defaultShader.setMatrix4Parameter("u_model", i4);
		defaultShader.setTextureParameter("u_colorTexture", texture, 0);
		renderer.renderScene(-1);
		screen.render(0);
		defaultShader.disable();
		screenTarget.unbind();

		evnContext->display(0);
	}

	delete imp::VBOManager::getInstance();

	exit(EXIT_SUCCESS);
}
