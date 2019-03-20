#include <SceneGraph/GraphicRenderer.h>
#include <SceneGraph/Camera.h>
#include <SceneGraph/GeoNode.h>

using namespace imp;

#include <SFML/Graphics.hpp>

#include "shader.h"

void snake(Geometry& geo)
{
	for(int i=0;i<(int)geo.size();++i)
	{
		Vec3& vertex = geo[i];
		vertex.x() += std::cos(vertex.z() * 3.141592) * 0.1;
		vertex.y() += std::sin(vertex.z() * 3.141592) * 0.1;
	}
}

struct Graph
{
	GraphicRenderer::Ptr renderer;
	Camera::Ptr camera;
	SceneNode::Ptr root;
	
	Graph()
	{
		renderer = GraphicRenderer::create();
		root = GeoNode::create(Geometry(),false);
		
		camera = Camera::create();
		camera->setPosition(Vec3(1.5f, 0.0f, 0.0f));
		camera->setTarget(Vec3(0.0f, 0.0f, 0.0f));
		
		// root = camera;
		add(camera);
		
		renderer->setSceneRoot(root);
		renderer->getRenderParameters()->setFaceCullingMode(RenderParameters::FaceCullingMode_None);
		renderer->getRenderParameters()->setPerspectiveProjection(60.0, 1.0, 0.1, 128.0);
		renderer->getRenderParameters()->setViewport(0.0,0.0,500.0,500.0);
		renderer->getRenderParameters()->setLineWidth(1.0);
		renderer->getRenderParameters()->setClearColor(Vec3(0.2,0.2,0.2));
	}
	
	void add(SceneNode::Ptr node)
	{
		root->addSubNode(node);
	}
	
	void sphere(const Geometry& geo, Vec3 color = Vec3(1.0), bool wireframe = false)
	{
		GeoNode::Ptr ph = GeoNode::create(geo,wireframe);
		ph->setColor(color);
		add(ph);
	}
	
	void wire(const Vec3& p1, const Vec3& p2, const Vec3& color)
	{
		Geometry geo; geo.add( {p1,p2} ); geo.setPrimitive(Geometry::Primitive_Lines);
		GeoNode::Ptr solid = GeoNode::create(geo,true);
		solid->setColor(color);
		add(solid);
	}
	
	void point(const Vec3& position, const Vec3& color)
	{
		Geometry geo = Geometry::sphere(2, 0.02); geo += position;
		GeoNode::Ptr solid = GeoNode::create(geo,false);
		solid->setColor(color);
		add(solid);
	}
};

int main(int argc, char* argv[])
{
	sf::Clock clock;
	
	sf::RenderWindow window(sf::VideoMode(500, 500), "My window", sf::Style::Default, sf::ContextSettings(24));
	window.setFramerateLimit(60);
	
	Graph graph;
	#include "initGraph.cpp"
	
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
		
        graph.camera->setPosition(Vec3(cos(t)*2.0,sin(t)*2.0,1.0));
		graph.camera->setTarget(Vec3(0.0f, 0.0f, 0.0f));
		
		// rendering
		graph.renderer->renderScene();
		
		window.display();
    }

	delete VBOManager::getInstance();

	exit(EXIT_SUCCESS);
}
