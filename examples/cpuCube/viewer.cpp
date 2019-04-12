#include <Core/Matrix3.h>
#include <SceneGraph/GraphRenderer.h>
#include <SceneGraph/GeoNode.h>

#include <SFML/Graphics.hpp>

#include <ctime>


struct IGStuff
{
	imp::GraphRenderer::Ptr renderer;
	imp::SceneNode::Ptr graphRoot;
	imp::Camera::Ptr camera;
	imp::Target::Ptr target;
	
	imp::Vec4 viewport;
	imp::Vec3 initCamPos;
	
	IGStuff()
	{
		viewport.set(0.0,0.0,256.0,256.0);
		
		target = imp::Target::create();
		target->create((int)viewport.z(),(int)viewport.w(),1,true);
		
		renderer = imp::GraphRenderer::create();
		renderer->getInitState()->setTarget(target);
		renderer->getInitState()->setViewport( viewport );
	
		imp::Geometry cubeGeo = imp::Geometry::cube();
		imp::GeoNode::Ptr cubeNode = imp::GeoNode::create(cubeGeo);
		
		initCamPos.set(10,0,3);
		camera = imp::Camera::create();
		camera->setPosition( initCamPos );
		
		graphRoot = imp::SceneNode::create();
		graphRoot->addNode(camera);
		graphRoot->addNode(cubeNode);
	};
	
	void updateCamera(float angle)
	{
		camera->setPosition( initCamPos * imp::Matrix3::rotationZ(angle) );
	}
	
	void render()
	{
		renderer->renderScene( graphRoot );
	}
};


// -----------------------------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	sf::RenderWindow* window = new sf::RenderWindow(sf::VideoMode(500, 500), "Cpu Renderer");
	sf::Texture texture; texture.create(256, 256);
	sf::Sprite sprite(texture);
	
	IGStuff engine;
	
	float target_fps = 60.0, current_fps = 60.0, acc = 1.0;
    double a = 0.0;
	
	int frames = 0;
	sf::Clock c;
	
	while (window->isOpen())
	{
		sf::Event event;
		while (window->pollEvent(event))
		{
			if (event.type == sf::Event::Closed) window->close();
		};

		window->clear();
		window->draw(sprite);
		window->display();
		
		a += 0.02 * acc; if(a > 2.0*3.141592) a = 0.0;
		engine.updateCamera(a);
		
		engine.render();
		
		imp::Image::Ptr res = engine.target->get(0);
        texture.update(res->data(),res->width(),res->height(),0,0);
		sprite.setScale( 500.0 / res->width(), 500.0 / res->height() );
		frames++;
		
		if(c.getElapsedTime().asMilliseconds() > 1000.0)
		{
			current_fps = frames;
			acc = target_fps / std::max(current_fps,1.f);
			std::cout << "FPS : " << std::endl;
			std::cout << "update : " << current_fps*acc << std::endl;
			std::cout << "render : " << current_fps << std::endl;
			frames = 0;
			c.restart();
		}
	}
   
    return 0;
}


