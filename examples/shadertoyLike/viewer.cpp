// #include <SceneGraph/BmpLoader.h>
#include <Graphics/Uniform.h>
#include <SceneGraph/GraphRenderer.h>
// #include <SceneGraph/RenderTarget.h>
#include <SceneGraph/Camera.h>
#include <SceneGraph/Target.h>

#include <SFML/Graphics.hpp>
#include <SceneGraph/QuadNode.h>

#include <Utils/ImageIO.h>

#include <Utils/FileInfo.h>
#include <fstream>

std::vector<std::string> getarg(int argc, char* argv[])
{
	std::vector<std::string> result;
	result.resize(argc);
	for(int i=0;i<argc;++i) result[i] = std::string(argv[i]);
	return result;
}

int main(int argc, char* argv[])
{
	char default_shader[] = "defaultshader.frag";
	std::vector<std::string> arg = getarg(argc,argv);
	if(arg.size() < 2) arg.push_back(default_shader);
	
	sf::RenderWindow window(sf::VideoMode(500, 500), arg[1], sf::Style::Default, sf::ContextSettings(24));
	window.setFramerateLimit(60);

    time_t accessLast, modifLast, statusLast;
	imp::getTimeInfo(default_shader, accessLast, modifLast, statusLast);

	imp::GraphRenderer::Ptr renderer = imp::GraphRenderer::create();
	renderer->setClearColor(imp::Vec4(1.0,0.0,0.0,1.0));
	imp::State::Ptr state = renderer->getInitState();
	state->setOrthographicProjection(0.f, 1.f, 0.f, 1.f, 0.f, 1.f);
    state->setViewport(0.0, 0.0, 500.0, 500.0);

    std::cout << "defaultshader.vert + " << arg[1] << std::endl;
	std::string c_vert = imp::loadText("defaultshader.vert");
	std::string c_frag = imp::loadText(arg[1]);
	imp::ShaderDsc::Ptr shader = imp::ShaderDsc::create();
	shader->vertCode = c_vert;
	shader->fragCode = c_frag;
	
    imp::Uniform::Ptr u_time = imp::Uniform::create("u_timer",imp::Uniform::Type_1f);
	u_time->set(0.f);
	
	imp::SceneNode::Ptr screen = imp::QuadNode::create();
    state->setShader(shader);
	state->setUniform(u_time);
	
	// imp::Target::Ptr target = imp::Target::create();
	// target->create(500, 500,1);
	// state->setTarget(target);

    sf::Clock timer;
    
	bool breakLoop = false;
	while (window.isOpen())
	{
        sf::Event event;
        while (window.pollEvent(event)) if (event.type == sf::Event::Closed) {breakLoop=true; break;} //window.close();
		if(breakLoop) break;
        
		// reload if changed
		time_t access, modif, status;
		imp::getTimeInfo(default_shader, access, modif, status);
		if(modif != modifLast)
		{   
            std::cout << "[reload shaders] defaultshader.vert + " << arg[1] << std::endl;
			modifLast = modif;
			c_frag = imp::loadText(arg[1]);
			shader->_d = NULL;
			shader->vertCode = c_vert;
			shader->fragCode = c_frag;
            screen->getState()->setShader(shader);
		}
		
		float sec = timer.getElapsedTime().asMilliseconds() / 1000.0;
        u_time->set(sec);
        renderer->renderScene( screen );

		window.display();
	}
	
	
	// imp::Image::Ptr result = target->get(0);
	// imp::ImageIO::save(result, "test.tga");

	return EXIT_SUCCESS;
}
