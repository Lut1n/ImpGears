#include <SceneGraph/GraphicRenderer.h>
#include <SceneGraph/Camera.h>
#include <SceneGraph/GeoNode.h>

using namespace imp;

#include <SFML/Graphics.hpp>

#include "shader.h"

int main(int argc, char* argv[])
{
	sf::Clock clock;
	
	sf::RenderWindow window(sf::VideoMode(500, 500), "My window", sf::Style::Default, sf::ContextSettings(24));
	window.setFramerateLimit(60);
	
	
	GraphicRenderer::Ptr graph = GraphicRenderer::create();
	SceneNode::Ptr root = GeoNode::create(Geometry(),false);
	graph->setSceneRoot(root);
	
	Camera::Ptr camera = Camera::create();
	camera->setPosition(Vec3(0.0f, 0.0f, 0.0f));
	camera->setTarget(Vec3(0.0f, 0.0f, 0.0f));
	root->addSubNode(camera);
	
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

	GeoNode::Ptr node1 = GeoNode::create(coords, false);
	node1->setColor(Vec3(1.0,1.0,0.0));
	node1->setPosition(Vec3(0.3,0.0,-0.2));

	GeoNode::Ptr node2 = GeoNode::create(coords, false);
	node2->setColor(Vec3(1.0,0.0,0.0));
	node2->setPosition(Vec3(0.0,0.0,0.0));
	root->addSubNode(node2);
	
	GeoNode::Ptr node3 = GeoNode::create(coords, false);
	node3->setColor(Vec3(0.0,1.0,0.0));
	node3->setPosition(Vec3(0.0,0.0,0.3));
	node3->setRotation(3.14 * 0.1, 0.0, 3.14 * 0.25);
	node2->addSubNode(node3);
	
	GeoNode::Ptr node4 = GeoNode::create(coords, false);
	node4->setColor(Vec3(0.0,0.0,1.0));
	node4->setPosition(Vec3(0.0,0.0,0.3));
	node4->setRotation(3.14 * 0.1, 0.0, 3.14 * 0.25);
	node3->addSubNode(node4);
	
	camera->addSubNode(node1);

	/*GeoNode::Ptr node3 = GeoNode::create(point, false);
	node3->setColor(Vec3(1.0,0.2,0.2));
	root->addSubNode(node3);*/
	
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed || event.type == sf::Event::KeyPressed)
                window.close();
        }
		
        if(clock.getElapsedTime().asSeconds() > 2.0*3.14) clock.restart();
		double t = clock.getElapsedTime().asMilliseconds() / 1000.0;
		
        camera->setPosition(Vec3(cos(-t)*2.0,sin(-t)*2.0,(sin(t))*0.5));
		camera->setTarget(Vec3(0.0,0.0,(sin(t))*0.5 ));
		node1->setPosition(Vec3(cos(t)*0.3,sin(t)*0.3,0.0));
		
		// rendering
		graph->renderScene();
		
		window.display();
    }

	delete VBOManager::getInstance();

	exit(EXIT_SUCCESS);
}
