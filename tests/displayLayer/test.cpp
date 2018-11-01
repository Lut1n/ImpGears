#include <SceneGraph/BmpLoader.h>
#include <SceneGraph/DefaultShader.h>
#include <SceneGraph/GraphicRenderer.h>
#include <SceneGraph/RenderTarget.h>

#include <SFML/Graphics.hpp>
#include <SceneGraph/ScreenVertex.h>

#include <Utils/FileInfo.h>

std::string loadShader(const char* filename);

void drawMonster(imp::LayeredImage::Ptr data);

int main(int argc, char* argv[])
{
	sf::RenderWindow window(sf::VideoMode(500, 500), "My window", sf::Style::Default, sf::ContextSettings(24));
	window.setFramerateLimit(60);

	imp::GraphicRenderer::Ptr renderer = imp::GraphicRenderer::create();

    imp::LayeredImage::Ptr data = imp::LayeredImage::create();
    data->build(8,11);
	drawMonster(data);
	imp::Texture::Ptr texture = imp::Texture::create();
    texture->loadFromLayeredImage(data);
	window.setTitle("Monster Layer");

	
	{
		imp::ImageData::Ptr data = imp::ImageData::create();
		texture->getImageData(data.get());
		BmpLoader::saveToFile(data.get(), "imagedata.bmp");
	}
	
	// screen render parameters
	imp::RenderParameters::Ptr screenParameters = imp::RenderParameters::create();
	screenParameters->setOrthographicProjection(0.f, 1.f, 0.f, 1.f, 0.f, 1.f);
	screenParameters->setClearColor(imp::Vec3(0.f, 0.f, 1.f));
    screenParameters->setViewport(0.0, 0.0, 500.0, 500.0);

	imp::Matrix4 i4 = imp::Matrix4::getIdentityMat();
    
	imp::Shader::Ptr defaultShader = imp::Shader::create(loadShader("defaultshader.vert").c_str(), loadShader("defaultshader.frag").c_str());
	imp::ScreenVertex::Ptr screen = imp::ScreenVertex::create();
    
    renderer->setRenderParameters(screenParameters);
    
	imp::Camera::Ptr camera = imp::Camera::create();
    camera->getGraphicState()->setParameters(screenParameters);
    
    screen->getGraphicState()->setShader(defaultShader);
    
	imp::Uniform::Ptr u_tex = imp::Uniform::create("u_colorTexture", imp::Uniform::Type_1i);
	u_tex->set((int)texture->getVideoID());
	// defaultShader->addUniform(u_tex);
	
	
    renderer->setSceneRoot(screen);
    camera->addSubNode(screen);
    
	while (window.isOpen())
	{
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)// || event.type == sf::Event::KeyPressed)
                window.close();
        }
		
		defaultShader->enable();
		texture->bind();
		// defaultShader->setTextureUniform("u_colorTexture", texture.get());
        renderer->renderScene();
		// screen->render();

		window.display();
	}

	delete imp::VBOManager::getInstance();

	exit(EXIT_SUCCESS);
}




















void drawMonster(imp::LayeredImage::Ptr data)
{
	imp::Vec4 bg(50,50,200,255);
	data->fill(bg);
	 
	 imp::Vec4 monsterColor(255,100,20,255);
	 data->set(0,3,monsterColor);
	 data->set(0,4,monsterColor);
	 data->set(0,6,monsterColor);
	 data->set(0,7,monsterColor);
	 data->set(1,0,monsterColor);
	 data->set(1,2,monsterColor);
	 data->set(1,8,monsterColor);
	 data->set(1,10,monsterColor);
	 data->set(2,0,monsterColor);
	 data->set(2,2,monsterColor);
	 data->set(2,3,monsterColor);
	 data->set(2,4,monsterColor);
	 data->set(2,5,monsterColor);
	 data->set(2,6,monsterColor);
	 data->set(2,7,monsterColor);
	 data->set(2,8,monsterColor);
	 data->set(2,10,monsterColor);
	 data->set(3,0,monsterColor);
	 data->set(3,1,monsterColor);
	 data->set(3,2,monsterColor);
	 data->set(3,3,monsterColor);
	 data->set(3,4,monsterColor);
	 data->set(3,5,monsterColor);
	 data->set(3,6,monsterColor);
	 data->set(3,7,monsterColor);
	 data->set(3,8,monsterColor);
	 data->set(3,9,monsterColor);
	 data->set(3,10,monsterColor);
	 data->set(4,1,monsterColor);
	 data->set(4,2,monsterColor);
	 data->set(4,4,monsterColor);
	 data->set(4,5,monsterColor);
	 data->set(4,6,monsterColor);
	 data->set(4,8,monsterColor);
	 data->set(4,9,monsterColor);
	 data->set(5,2,monsterColor);
	 data->set(5,3,monsterColor);
	 data->set(5,4,monsterColor);
	 data->set(5,5,monsterColor);
	 data->set(5,6,monsterColor);
	 data->set(5,7,monsterColor);
	 data->set(5,8,monsterColor);
	 data->set(6,3,monsterColor);
	 data->set(6,7,monsterColor);
	 data->set(7,2,monsterColor);
	 data->set(7,8,monsterColor);
}



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
