#include <SceneGraph/BmpLoader.h>
#include <SceneGraph/DefaultShader.h>
#include <SceneGraph/GraphicRenderer.h>
#include <SceneGraph/RenderTarget.h>

#include <SFML/Graphics.hpp>
#include <SceneGraph/ScreenVertex.h>

#include <Utils/FileInfo.h>


std::string loadShader(const char* filename)
{
	std::string buffer;
	std::ifstream is;
	is.open(filename);
	while( !is.eof() )
	{
		char c = is.get();
		
		if(c == '\n' || c == '\r')
			c = '\n';
		
		if(!is.eof())		
			buffer.push_back(c);
	}
	is.close();
	
	// std::cout << filename << " : \n " << buffer << std::endl;
	
	return buffer;
}

int main(int argc, char* argv[])
{
	if(argc<2)
		return 0;
	
	sf::RenderWindow window(sf::VideoMode(500, 500), "My window", sf::Style::Default, sf::ContextSettings(24));
	window.setFramerateLimit(60);

	imp::GraphicRenderer renderer;


    imp::ImageData data;
    data.create(500, 500, imp::PixelFormat_BGRA8);
    
	imp::Texture texture;
    texture.loadFromImageData(&data);


	time_t accessLast, modifLast, statusLast;
	imp::getTimeInfo(argv[1], accessLast, modifLast, statusLast);

	window.setTitle(argv[1]);

	// screen render parameters
	std::shared_ptr<imp::RenderParameters> screenParameters;
	screenParameters.reset(new imp::RenderParameters());
	screenParameters->setOrthographicProjection(0.f, 1.f, 0.f, 1.f, 0.f, 1.f);
	screenParameters->setClearColor(imp::Vector3(0.f, 0.f, 1.f));

	// screen render target
	imp::RenderTarget screenTarget;
	screenTarget.createScreenTarget(500,500);

	imp::Matrix4 i4 = imp::Matrix4::getIdentityMat();

    std::cout << "defaultshader.vert + " << argv[1] << std::endl;
    
	imp::Shader* defaultShader = new imp::Shader(loadShader("defaultshader.vert").c_str(), loadShader(argv[1]).c_str());
	imp::ScreenVertex screen;

    sf::Clock timer;
    
	while (window.isOpen())
	{
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed || event.type == sf::Event::KeyPressed)
                window.close();
        }

		// reload if changed
		time_t access, modif, status;
		imp::getTimeInfo(argv[1], access, modif, status);
		// std::cout << "MODIF time " << modif << "; " << modifLast << "\n";
		if(modif != modifLast)
		{
            if(defaultShader == IMP_NULL)
                delete defaultShader;
            
            std::cout << "[reload shaders] defaultshader.vert + " << argv[1] << std::endl;
			modifLast = modif;
			defaultShader = new imp::Shader(loadShader("defaultshader.vert").c_str(), loadShader(argv[1]).c_str());
		}

		renderer.setRenderParameters(screenParameters);
		screenTarget.bind();
		defaultShader->enable();
		defaultShader->setMatrix4Parameter("u_projection", renderer.getProjectionMatrix());
		defaultShader->setMatrix4Parameter("u_view", i4);
		defaultShader->setMatrix4Parameter("u_model", i4);
		defaultShader->setTextureParameter("u_colorTexture", &texture, 0);
        defaultShader->setFloatParameter("u_timer", timer.getElapsedTime().asMilliseconds());
		renderer.renderScene();
		screen.render();
		defaultShader->disable();
		screenTarget.unbind();

		window.display();
	}

	delete imp::VBOManager::getInstance();

	exit(EXIT_SUCCESS);
}
