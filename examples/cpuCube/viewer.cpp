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
	
	IGStuff(const std::string& arg)
	{
		imp::LightModel::Ptr model = imp::LightModel::create(imp::LightModel::Model_Phong_NoTex);
		
		viewport.set(0.0,0.0,512.0,512.0);
		
		target = imp::Target::create();
		target->create(512,512,1,true);
		
		renderer = imp::GraphRenderer::create();
		if(arg != "-gpu") renderer->setTarget(target);
		renderer->getInitState()->setViewport( viewport );
	
		imp::Geometry cubeGeo = imp::Geometry::cube();
		cubeGeo.generateColors(imp::Vec3(1.0,1.0,1.0));
		cubeGeo.generateNormals();
		cubeGeo.generateTexCoords(2.0);
		imp::Geometry::intoCCW( cubeGeo );
		
		imp::GeoNode::Ptr cubeNode = imp::GeoNode::create(cubeGeo);
		cubeNode->setShader(model);
		cubeNode->setColor(imp::Vec3(1.0));
		
		initCamPos.set(10,0,3);
		
		imp::Uniform::Ptr u_lightPos = imp::Uniform::create("u_lightPos", imp::Uniform::Type_3f);
		imp::Uniform::Ptr u_lightCol = imp::Uniform::create("u_lightCol", imp::Uniform::Type_3f);
		imp::Uniform::Ptr u_lightAtt = imp::Uniform::create("u_lightAtt", imp::Uniform::Type_3f);
		u_lightPos->set(imp::Vec3(10.0,5.0,5.0));
		u_lightCol->set(imp::Vec3(1.0));
		u_lightAtt->set(imp::Vec3(30.0,1.0,0.0));
		cubeNode->getState()->setUniform(u_lightPos);
		cubeNode->getState()->setUniform(u_lightCol);
		cubeNode->getState()->setUniform(u_lightAtt);
		
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
	int sfDepthBufferBits = 24;
	sf::RenderWindow window(sf::VideoMode(512, 512), "Example cube", sf::Style::Default, sf::ContextSettings(sfDepthBufferBits));
	sf::Texture texture; texture.create(512, 512);
	sf::Sprite sprite(texture);
	
	std::string engine_arg = ""; if(argc > 1) engine_arg = argv[1];
	IGStuff engine(engine_arg);
	
	float target_fps = 60.0, current_fps = 60.0, acc = 1.0;
    double a = 0.0;
	
	int frames = 0;
	sf::Clock c;
	
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) window.close();
		};

		window.clear();
		
		a += 0.02 * acc; if(a > 2.0*3.141592) a = 0.0;
		engine.updateCamera(a);
		
		engine.render();

		if(engine_arg != "-gpu") 
		{
			imp::Image::Ptr res = engine.target->get(0)->getSource();
			texture.update(res->data(),res->width(),res->height(),0,0);
			sprite.setScale( 512.0 / res->width(), -512.0 / res->height() );
			sprite.setPosition( 0, 512 );
			window.draw(sprite);
		}
		window.display();
		
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


