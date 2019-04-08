#include <Core/Matrix3.h>
#include <Graphics/GeometryRenderer.h>

#include <SFML/Graphics.hpp>

#include <ctime>

// -----------------------------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	imp::Vec3 initialPos(10,0,3);
	
	imp::GeometryRenderer renderer;
	imp::Vec4 vp = renderer.getViewport();
    
	imp::Image::Ptr rgb = imp::Image::create(vp[2],vp[3],4);
    imp::Image::Ptr depth = imp::Image::create(vp[2],vp[3],1);
	
	renderer.setTarget(0,rgb);
	renderer.setTarget(1,depth,imp::Vec4(255.0));
	
    imp::Geometry geometry = imp::Geometry::cube(); imp::Geometry::intoCCW(geometry);
	
	sf::RenderWindow* window = new sf::RenderWindow(sf::VideoMode(500, 500), "Cpu Renderer");
	sf::Texture texture; texture.create(rgb->width(), rgb->height());
	sf::Sprite sprite(texture);
	
	float target_fps = 60.0, current_fps = 60.0, acc = 1.0;
    double a = 0.0;
	
	int frames = 0;
	sf::Clock c;
	
	renderer.init();
	
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
		imp::Vec3 curr_pos = initialPos * imp::Matrix3::rotationZ(a);
		renderer.setView( imp::Matrix4::view(curr_pos, imp::Vec3(0.0), imp::Vec3(0,0,1)) );
		
        renderer.clearTargets();
		renderer.render(geometry);
		
        texture.update(rgb->data(),rgb->width(),rgb->height(),0,0);
		sprite.setScale( 500.0 / rgb->width(), 500.0 / rgb->height() );
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


