#include "Graphics/BmpLoader.h"
#include "Graphics/BasicShader.h"
#include "Graphics/GraphicRenderer.h"
#include "Graphics/GraphicState.h"
#include "Graphics/RenderTarget.h"
#include "Graphics/FreeFlyCamera.h"
#include "Core/frustumParams.h"

#include "Core/Timer.h"
#include "Core/State.h"

#include "System/SFMLContextInterface.h"
#include "Graphics/ScreenVertex.h"

#include <System/FileInfo.h>

class ProceduralCube : public imp::VBOData, public imp::SceneNode
{
    public:
	
		void pushVertice(float x, float y, float z)
		{
			_vertexBuffer.push_back(x);
			_vertexBuffer.push_back(y);
			_vertexBuffer.push_back(z);
		}
	
        ProceduralCube()
		{
			// bottom
			pushVertice(-1.f, -1.f, 1.f);
			pushVertice(1.f, -1.f, 1.f);
			pushVertice(1.f, 1.f, 1.f);
			pushVertice(-1.f, 1.f, 1.f);

			// up
			pushVertice(-1.f, -1.f, -1.f);
			pushVertice(-1.f, 1.f, -1.f);
			pushVertice(1.f, 1.f, -1.f);
			pushVertice(1.f, -1.f, -1.f);

			// left
			pushVertice(-1.f, -1.f, -1.f);
			pushVertice(-1.f, -1.f, 1.f);
			pushVertice(-1.f, 1.f, 1.f);
			pushVertice(-1.f, 1.f, -1.f);

			// right
			pushVertice(1.f, -1.f, -1.f);
			pushVertice(1.f, 1.f, -1.f);
			pushVertice(1.f, 1.f, 1.f);
			pushVertice(1.f, -1.f, 1.f);

			// face
			pushVertice(-1.f, -1.f, -1.f);
			pushVertice(1.f, -1.f, -1.f);
			pushVertice(1.f, -1.f, 1.f);
			pushVertice(-1.f, -1.f, 1.f);
			
			// back
			pushVertice(-1.f, 1.f, -1.f);
			pushVertice(-1.f, 1.f, 1.f);
			pushVertice(1.f, 1.f, 1.f);
			pushVertice(1.f, 1.f, -1.f);

			const float texCoord[6*8] =
			{
			  0.f, 0.f,
			  1.f, 0.f,
			  1.f, 1.f,
			  0.f, 1.f,
			  0.f, 0.f,
			  1.f, 0.f,
			  1.f, 1.f,
			  0.f, 1.f,
			  0.f, 0.f,
			  1.f, 0.f,
			  1.f, 1.f,
			  0.f, 1.f,
			  0.f, 0.f,
			  1.f, 0.f,
			  1.f, 1.f,
			  0.f, 1.f,
			  0.f, 0.f,
			  1.f, 0.f,
			  1.f, 1.f,
			  0.f, 1.f,
			  0.f, 0.f,
			  1.f, 0.f,
			  1.f, 1.f,
			  0.f, 1.f
			};

			imp::Uint32 vertexBuffSize = _vertexBuffer.size()*sizeof(float);
			imp::Uint32 texCoordSize = 6*8*sizeof(float);

			m_texCoordOffset = vertexBuffSize;

			requestVBO(vertexBuffSize+texCoordSize);
			setVertices(_vertexBuffer.data(), vertexBuffSize);
			//    setData(vertex, vertexBuffSize, 0);
			setData(texCoord, texCoordSize, m_texCoordOffset);
			
			setScale(imp::Vector3(0.5,0.5,0.5));
		}
		
        virtual ~ProceduralCube()
		{
			
		}

        virtual void render(imp::Uint32 passID)
		{
			setRotation(getRx()+0.03, getRy()+0.07, getRz()+0.01);
			
			drawVBO();
		}

    protected:
	
		std::vector<float> _vertexBuffer;
	
    private:

        imp::Uint64 m_texCoordOffset;
};

struct ViewerState
{
	imp::State state;
	std::shared_ptr<imp::GraphicRenderer> renderer;
	std::shared_ptr<imp::RenderParameters> screenParameters;
	std::shared_ptr<imp::RenderTarget> screenTarget;

	std::shared_ptr<imp::BasicShader> defaultShader;
	std::shared_ptr<ProceduralCube> screen;

	imp::EvnContextInterface* evnContext;
	std::shared_ptr<imp::FreeFlyCamera> camera;
	imp::Vector3 color;

	std::string filename;
	
	ViewerState()
	{	
		evnContext = new imp::SFMLContextInterface();
		evnContext->createWindow(800,600);
		evnContext->setCursorVisible(0, true);
		
		// screen render target
		screenTarget .reset( new imp::RenderTarget() );
		screenTarget->createScreenTarget(0);
		
		camera.reset( new imp::FreeFlyCamera(400, 300) );
		camera->initFrustum(800, 600, FRUSTUM_FOVY*3.14159f/180.f, FRUSTUM_NEAR, FRUSTUM_FAR);
		camera->setPosition(imp::Vector3(2.f, -3.f, 2.f));
		camera->setTarget(imp::Vector3(0.0f, 0.0f, 0.0f));
		
		renderer.reset(new imp::GraphicRenderer() );
		renderer->setCenterCursor(false);
	
		// screen render parameters
		screenParameters.reset(new imp::RenderParameters() );
		screenParameters->setPerspectiveProjection(FRUSTUM_FOVY, 4.f/3.f, FRUSTUM_NEAR, FRUSTUM_FAR);
		screenParameters->setClearColor(imp::Vector3(0.5f, 0.5f, 1.f));
		
		color.setXYZ(1.0, 0.0, 0.0);
		defaultShader.reset(new imp::BasicShader() );

		defaultShader->addVector3Parameter("u_color", &color);
	
		screen.reset( new ProceduralCube() );
		camera->getGraphicState()->setTarget(screenTarget);
		camera->getGraphicState()->setParameters(screenParameters);
		camera->getGraphicState()->setShader(defaultShader);
		
		renderer->setSceneRoot(camera);
		camera->addSubNode(screen);
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

int main(int argc, char* argv[])
{
	ViewerState v_state;
	
	while (v_state.evnContext->isOpen(0))
	{
	
		onEvent(*imp::EvnContextInterface::getInstance());
	
		// rendering
		v_state.renderer->renderScene(-1);

		// display
		v_state.evnContext->display(0);
	}

	delete imp::VBOManager::getInstance();

	exit(EXIT_SUCCESS);
}
