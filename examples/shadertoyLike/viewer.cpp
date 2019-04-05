// #include <SceneGraph/BmpLoader.h>
#include <SceneGraph/DefaultShader.h>
#include <SceneGraph/GraphRenderer.h>
#include <SceneGraph/RenderTarget.h>
#include <SceneGraph/Camera.h>

#include <SFML/Graphics.hpp>
#include <SceneGraph/QuadNode.h>

#include <Utils/FileInfo.h>
#include <fstream>

std::string loadShader(const char* filename)
{
	std::ifstream ifs(filename);
	std::string buffer( (std::istreambuf_iterator<char>(ifs) ),
						(std::istreambuf_iterator<char>()    ) );
	return buffer;
}

int main(int argc, char* argv[])
{
	if(argc<2) return 0;
	
	sf::RenderWindow window(sf::VideoMode(500, 500), argv[1], sf::Style::Default, sf::ContextSettings(24));
	window.setFramerateLimit(60);

    time_t accessLast, modifLast, statusLast;
	imp::getTimeInfo(argv[1], accessLast, modifLast, statusLast);

	imp::GraphRenderer::Ptr renderer = imp::GraphRenderer::create();
	imp::State::Ptr state = renderer->getInitState();
	state->setOrthographicProjection(0.f, 1.f, 0.f, 1.f, 0.f, 1.f);
    state->setViewport(0.0, 0.0, 500.0, 500.0);

    std::cout << "defaultshader.vert + " << argv[1] << std::endl;
	std::string c_vert = loadShader("defaultshader.vert");
	std::string c_frag = loadShader(argv[1]);
	imp::Shader::Ptr shader = imp::Shader::create(c_vert.c_str(), c_frag.c_str());
	
    imp::Uniform::Ptr u_time = imp::Uniform::create("u_timer",imp::Uniform::Type_1f);
	shader->addUniform(u_time);
	u_time->set(0.f);
	
	imp::SceneNode::Ptr screen = imp::QuadNode::create();
    screen->getState()->setShader(shader);

    sf::Clock timer;
    
	while (window.isOpen())
	{
        sf::Event event;
        while (window.pollEvent(event)) if (event.type == sf::Event::Closed) window.close();

		// reload if changed
		time_t access, modif, status;
		imp::getTimeInfo(argv[1], access, modif, status);
		if(modif != modifLast)
		{   
            std::cout << "[reload shaders] defaultshader.vert + " << argv[1] << std::endl;
			modifLast = modif;
			c_frag = loadShader(argv[1]);
			shader = imp::Shader::create(c_vert.c_str(), c_frag.c_str());
			shader->addUniform(u_time);
            screen->getState()->setShader(shader);
		}
		
		float sec = timer.getElapsedTime().asMilliseconds() / 1000.0;
        u_time->set(sec);
        renderer->renderScene( screen );

		window.display();
	}

	delete imp::VBOManager::getInstance();

	exit(EXIT_SUCCESS);
}
