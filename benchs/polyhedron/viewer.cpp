#include <SceneGraph/Graph.h>
#include <SceneGraph/Camera.h>
#include <SceneGraph/GeoNode.h>
#include <Descriptors/ImageIO.h>
#include <Descriptors/FileInfo.h>
#include <Descriptors/JsonImageOp.h>

#include <Renderer/SceneRenderer.h>

#include <SFML/Graphics.hpp>
#include <filesystem>

using namespace imp;

#include "shader.h"

Geometry generateRockHat(float radius, int sub)
{
    imp::Geometry geometry = imp::Geometry::sphere(sub,radius);
	geometry *= 0.5;
    geometry.scale(Vec3(0.7,0.7,1.5));
	geometry.generateNormals();
	geometry.generateColors(Vec3(1.0,0.3,1.0));
	geometry.generateTexCoords(2.0);
	
	
    imp::Geometry geo2 = imp::Geometry::sphere(sub,radius);
	geo2 *= 0.5; // side size = one
	geo2 *= Vec3(1.5,1.5,0.5);
    geo2.scale(Vec3(2.0,2.0,2.0));
	geo2 += Vec3(0.0,0.0,radius);
	geo2.generateNormals();
	geo2.generateColors(Vec3(1.0,0.3,0.4));
	geo2.generateTexCoords(2.0);
	
	geometry += geo2;
	Geometry::intoCCW( geometry );
       
    return geometry;
}

// -----------------------------------------------------------------------------------------------------------------------
void loadSamplers(TextureSampler::Ptr& colorSampler, TextureSampler::Ptr& normalSampler)
{
	if( !fileExists("./cache/scene_color.tga") || !fileExists("./cache/scene_normals.tga") )
	{
		// c++17
		std::filesystem::create_directories("cache");
		generateImageFromJson("textures.json");
	}
	
	colorSampler =TextureSampler::create();
	colorSampler->setSource( ImageIO::load("./cache/scene_color.tga") );
	colorSampler->setMode(TextureSampler::Mode_Repeat);
	colorSampler->setInterpo(TextureSampler::Interpo_Linear);
	
	normalSampler =TextureSampler::create();
	normalSampler->setSource( ImageIO::load("./cache/scene_normals.tga") );
	normalSampler->setMode(TextureSampler::Mode_Repeat);
	normalSampler->setInterpo(TextureSampler::Interpo_Linear);
}

int main(int argc, char* argv[])
{
	
	TextureSampler::Ptr colorSampler, normalSampler;
	loadSamplers(colorSampler, normalSampler);
	
	Uniform::Ptr u_sampler_color = Uniform::create("u_sampler_color",Uniform::Type_Sampler);
	Uniform::Ptr u_sampler_normal = Uniform::create("u_sampler_normal",Uniform::Type_Sampler);
	Uniform::Ptr u_lightPos = Uniform::create("u_lightPos",Uniform::Type_3f);
	Uniform::Ptr u_lightCol = Uniform::create("u_lightCol",Uniform::Type_3f);
	Uniform::Ptr u_lightAtt = Uniform::create("u_lightAtt",Uniform::Type_3f);
	
	u_sampler_color->set(colorSampler, 0);
	u_sampler_normal->set(normalSampler, 1);
	u_lightPos->set( Vec3(4.0,2.0,3.0) );
	u_lightCol->set( Vec3(1.0) );
	u_lightAtt->set( Vec3(30.0,1.0,0.0) );
	
	sf::Clock clock;
	
	sf::RenderWindow window(sf::VideoMode(512, 512), "My window", sf::Style::Default, sf::ContextSettings(24));
	window.setFramerateLimit(60);
	
	Graph::Ptr graph = Graph::create();
	Node::Ptr root = ClearNode::create();
	
	Camera::Ptr camera = Camera::create();
	camera->setPosition(Vec3(3.0f, 0.0f, 0.0f));
	camera->setTarget(Vec3(0.0f, 0.0f, 0.0f));
	
	Geometry lineVol = Geometry::cylinder(10,1.0,0.02);
	Geometry arrowVol = Geometry::cone(10,0.1,0.05,0.0);
	arrowVol += Vec3::Z;
	Geometry zAxe = arrowVol + lineVol;
	Geometry xAxe = zAxe; xAxe.rotY(1.57);
	Geometry yAxe = zAxe; yAxe.rotX(-1.57);
	Geometry coords = xAxe + yAxe + zAxe;
	coords.setPrimitive(Geometry::Primitive_Triangles);
	coords *= 0.2;
	coords.generateColors(Vec3(1.0));

	Geometry point = Geometry::sphere(4, 0.1);
	point.setPrimitive(Geometry::Primitive_Triangles);
	point.generateColors(Vec3(1.0));

	GeoNode::Ptr node2 = GeoNode::create(coords, false);
	node2->setColor(Vec3(1.0,0.0,0.0));
	node2->setPosition(Vec3(0.0,0.0,0.0));
	
	GeoNode::Ptr node3 = GeoNode::create(coords, false);
	node3->setColor(Vec3(0.0,1.0,0.0));
	node3->setPosition(Vec3(0.0,0.0,0.3));
	node3->setRotation(Vec3(3.14 * 0.1, 0.0, 3.14 * 0.25));
	
	GeoNode::Ptr node4 = GeoNode::create(coords, false);
	node4->setColor(Vec3(0.0,0.0,1.0));
	node4->setPosition(Vec3(0.0,0.0,0.3));
	node4->setRotation(Vec3(3.14 * 0.1, 0.0, 3.14 * 0.25));
	

	GeoNode::Ptr node1 = GeoNode::create(coords, true);
	node1->setColor(Vec3(1.0,1.0,0.0));
	node1->setPosition(Vec3(0.3,0.0,-0.2));
	
	imp::ReflexionModel::Ptr s = imp::ReflexionModel::create(
		imp::ReflexionModel::Lighting_Phong,
		imp::ReflexionModel::Texturing_Customized);
	
	s->_fragCode_texturing = fragSimple;
	Geometry mush = generateRockHat(1.0, 4.0);
	GeoNode::Ptr geomush = GeoNode::create(mush, false);
	geomush->setColor(Vec3(1.0,1.0,1.0));
	geomush->setPosition(Vec3(1.0,0.0,0.0));
	geomush->setScale(Vec3(0.3));
	geomush->setShader(s);
	geomush->getState()->setUniform(u_sampler_color);
	geomush->getState()->setUniform(u_sampler_normal);
	geomush->getState()->setUniform(u_lightPos);
	geomush->getState()->setUniform(u_lightCol);
	geomush->getState()->setUniform(u_lightAtt);
	
	root->addNode(camera);
	root->addNode(geomush);
	root->addNode(node1);
	root->addNode(node2);
	node2->addNode(node3);
	node3->addNode(node4);
	graph->setRoot(root);
	
	SceneRenderer::Ptr renderer = SceneRenderer::create();
	
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
		
        // camera->setPosition(Vec3(cos(-t)*3.0,sin(-t)*3.0,0.0));
		// camera->setTarget(Vec3(0.0));
		Vec3 lp(cos(t)*4.0,sin(t)*3.0,1.0);
		node1->setPosition(lp);
		u_lightPos->set(lp);
		geomush->getState()->setUniform(u_lightPos);
		// geomush->setPosition(Vec3(cos(t+3.14)*5.0,sin(t+3.14)*5.0,0.0));
		// geomush->setRotation(Vec3(t * 0.1, 0.0, t * 0.25));
		
		// rendering
		renderer->render( graph );
		window.display();
    }
    
    // ImageIO::save(tgt->get(0)->getSource(),"test.tga");

	exit(EXIT_SUCCESS);
}
