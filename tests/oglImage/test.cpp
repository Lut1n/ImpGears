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

	imp::GraphRenderer::Ptr renderer = imp::GraphRenderer::create();
	renderer->setClearColor(imp::Vec4(1.0,0.0,0.0,1.0));
	imp::State::Ptr state = renderer->getInitState();
	state->setOrthographicProjection(0.f, 1.f, 0.f, 1.f, 0.f, 1.f);
    state->setViewport(0.0, 0.0, 500.0, 500.0);

	std::string c_vert = imp::loadText("defaultshader.vert");
	std::string c_frag = imp::loadText(arg[1]);
	std::string c_frag2 = imp::loadText("tex.frag");
	imp::ShaderDsc::Ptr shader = imp::ShaderDsc::create();
	shader->vertCode = c_vert;
	shader->fragCode = c_frag;
	
	imp::ShaderDsc::Ptr shader2 = imp::ShaderDsc::create();
	shader2->vertCode = c_vert;
	shader2->fragCode = c_frag2;
	
    imp::Uniform::Ptr u_time = imp::Uniform::create("u_timer",imp::Uniform::Type_1f);
	u_time->set(0.f);
	
	
	imp::Target::Ptr target = imp::Target::create();
	target->create(500, 500,1,false);
	
	imp::QuadNode::Ptr screen = imp::QuadNode::create();
	state = screen->getState();
    state->setShader(shader);
	state->setUniform(u_time);
	
	
	imp::Uniform::Ptr u_tex = imp::Uniform::create("u_sampler",imp::Uniform::Type_Sampler);
	u_tex->set(target->get(0));
	
	imp::QuadNode::Ptr screen2 = imp::QuadNode::create();
	state = screen2->getState();
	state->setShader(shader2);
	state->setUniform(u_tex);
    
	float sec = 0.0;
	bool breakLoop = false;
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event)) if (event.type == sf::Event::Closed) {breakLoop=true; break;} //window.close();
		if(breakLoop) break;

		u_time->set(sec+=0.01);

		renderer->setTarget(target);
		renderer->renderScene( screen );
		renderer->setDefaultTarget();
		renderer->renderScene( screen2 );

		window.display();
	}

	return EXIT_SUCCESS;
}
