#include <SceneGraph/GraphRenderer.h>
#include <SceneGraph/Camera.h>
#include <SceneGraph/GeoNode.h>

#include <Utils/ImageIO.h>

using namespace imp;

#include <SFML/Graphics.hpp>

#include "shader.h"

Geometry generateRockHat(const Vec3& center, float radius, int sub)
{
    imp::Geometry geometry = imp::Geometry::cube();
	geometry *= 0.5;
	geometry = geometry.subdivise(sub);
    geometry.sphericalNormalization(0.5);
    geometry.scale(Vec3(radius * 0.7,radius * 0.7,radius * 1.5));
	
	
    imp::Geometry geo2 = imp::Geometry::cube();
	geo2 *= 0.5; // side size = one
	geo2 *= Vec3(1.0,1.0,0.5); /* h = 0.25*/ geo2 += Vec3(0.0,0.0,0.25); // demi-cube
	geo2 = geo2.subdivise(sub*4);
    geo2.sphericalNormalization(0.7);
    geo2.scale(Vec3(radius*2.0,radius*2.0,radius*2.0));
	geo2 += Vec3(0.0,0.0,radius * 1.5);
	
	geometry += geo2;
	Geometry::intoCCW( geometry );
       
    return geometry;
}  

int main(int argc, char* argv[])
{
	sf::Clock clock;
	
	sf::RenderWindow window(sf::VideoMode(512, 512), "My window", sf::Style::Default, sf::ContextSettings(24));
	window.setFramerateLimit(60);
	sf::Texture texture; texture.create(512, 512);
	sf::Sprite sprite(texture);
	sprite.setTextureRect( sf::IntRect(0,512,512,-512) );
	
	
	GraphRenderer::Ptr graph = GraphRenderer::create();
	SceneNode::Ptr root = GeoNode::create(Geometry(),false);

	imp::Target::Ptr tgt = imp::Target::create();
	tgt->create(512,512,1,true);
	// graph->getInitState()->setTarget(tgt);
	// graph->setClearColor( imp::Vec4(1.0,0.0,0.0,1.0) );
	// graph->setClearDepth( 1.0 );
	
	Camera::Ptr camera = Camera::create();
	camera->setPosition(Vec3(0.0f, 0.0f, 0.0f));
	camera->setTarget(Vec3(0.0f, 0.0f, 0.0f));
	root->addNode(camera);
	
	Geometry lineVol = Geometry::cylinder(10,1.0,0.02);
	Geometry arrowVol = Geometry::cone(10,0.1,0.05,0.0);
	arrowVol += Vec3::Z;
	Geometry zAxe = arrowVol + lineVol;
	Geometry xAxe = zAxe; xAxe.rotY(1.57);
	Geometry yAxe = zAxe; yAxe.rotX(-1.57);
	Geometry coords = xAxe + yAxe + zAxe;
	coords.setPrimitive(Geometry::Primitive_Triangles);
	coords *= 0.2;

	Geometry point = Geometry::sphere(4, 0.1);
	point.setPrimitive(Geometry::Primitive_Triangles);

	GeoNode::Ptr node1 = GeoNode::create(coords, true);
	node1->setColor(Vec3(1.0,1.0,0.0));
	node1->setPosition(Vec3(0.3,0.0,-0.2));

	GeoNode::Ptr node2 = GeoNode::create(coords, false);
	node2->setColor(Vec3(1.0,0.0,0.0));
	node2->setPosition(Vec3(0.0,0.0,0.0));
	root->addNode(node2);
	
	GeoNode::Ptr node3 = GeoNode::create(coords, false);
	node3->setColor(Vec3(0.0,1.0,0.0));
	node3->setPosition(Vec3(0.0,0.0,0.3));
	node3->setRotation(Vec3(3.14 * 0.1, 0.0, 3.14 * 0.25));
	node2->addNode(node3);
	
	GeoNode::Ptr node4 = GeoNode::create(coords, false);
	node4->setColor(Vec3(0.0,0.0,1.0));
	node4->setPosition(Vec3(0.0,0.0,0.3));
	node4->setRotation(Vec3(3.14 * 0.1, 0.0, 3.14 * 0.25));
	node3->addNode(node4);
	
	// camera->addNode(node1);
	
	imp::ShaderDsc::Ptr s = imp::ShaderDsc::create();
	s->vertCode = vertexSimple;
	s->fragCode = fragSimple;
	Geometry mush = generateRockHat(Vec3(0.0,0.0,0.0), 0.3, 1.0);
	GeoNode::Ptr geomush = GeoNode::create(mush, false);
	geomush->setColor(Vec3(1.0,1.0,1.0));
	geomush->setPosition(Vec3(0.0,1.0,0.0));
	geomush->setRotation(Vec3(3.14 * 0.1, 0.0, 3.14 * 0.25));
	geomush->setShader(s);
	node2->addNode(geomush);
	
	
	bool stopLoop = false;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed || event.type == sf::Event::KeyPressed)
                stopLoop=true;// window.close();
        }
		
		if(stopLoop)break;
		
        if(clock.getElapsedTime().asSeconds() > 2.0*3.14) clock.restart();
		double t = clock.getElapsedTime().asMilliseconds() / 1000.0;
		
        camera->setPosition(Vec3(cos(-t)*2.0,sin(-t)*2.0,(sin(t))*0.5));
		camera->setTarget(Vec3(0.0,0.0,(sin(t))*0.5 ));
		node1->setPosition(Vec3(cos(t)*0.3,sin(t)*0.3,0.0));
		
		// rendering
		graph->renderScene( root );
		// imp::GraphRenderer::s_interface->unbind(tgt.get());
		// imp::Image::Ptr res = tgt->get(0);
        // texture.update(res->data(),res->width(),res->height(),0,0);
		
		// window.clear();
		// window.draw(sprite);
		window.display();
		
		// std::cout << "ite" << std::endl;
    }

	exit(EXIT_SUCCESS);
}
