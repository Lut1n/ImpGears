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

void F1(Geometry& geo)
{
	float freq = 0.5;
	float ampl = 0.2;
	
	for(auto& v : geo._vertices)
	{
		float d = Vec3(v[0],v[1],0.0).length();
		v.z() += std::sin( d * 2.0 * 3.141592 * freq ) * ampl;
	}
}

Geometry generateRockHat(float radius, int sub)
{
    imp::Geometry geometry = imp::Geometry::sphere(sub,radius);
	geometry *= 0.5;
    geometry.scale(Vec3(1.0,1.0,1.5));
	geometry.generateNormals();
	geometry.generateColors(Vec3(1.0,0.3,1.0));
	geometry.generateTexCoords( Geometry::TexGenMode_Cubic, 2.0);
	
	
    imp::Geometry geo2 = imp::Geometry::sphere(sub,radius);
	geo2 *= 0.5; // side size = one
	geo2 *= Vec3(1.5,1.5,0.5);
    geo2.scale(Vec3(2.0,2.0,2.0));
	geo2 += Vec3(0.0,0.0,radius);
	geo2.generateNormals();
	geo2.generateColors(Vec3(1.0,0.3,0.4));
	geo2.generateTexCoords( Geometry::TexGenMode_Cubic,2.0);
	F1(geo2);
	
	geometry += geo2;
	Geometry::intoCCW( geometry );
       
    return geometry;
}


Geometry generateLeaf()
{
	float freq = 10.0;
	float ampl = 0.5;
	
	Path path = Path::circle(40,100);
	
	for(int i=0;i<path.count();++i)
	{
		Vec3 p = path.vertex(i); p.normalize();
		// float r = std::atan2(p[1],p[0]);
		float r = p.angleFrom(Vec3::Y);
		float v = std::sin(r * freq);// if(v<0.0)v=0.0;else v=1.0;
		path.vertex(i) = p * (1.0+v*ampl);
		
		float h = p.y();
		path.vertex(i) *= Vec3(0.7,1.5-h,1.0);
	}
	
    imp::Geometry geometry = imp::Geometry::extrude(path, 0.1);
	geometry.generateColors(Vec3(0.2,1.0,0.3));
	Geometry::intoCCW( geometry );
	
	float freq2 = 0.2;
	float ampl2 = 0.2;
	for(int i=0;i<geometry.size();++i)
	{
		Vec3 p = geometry._vertices[i];
		float d = Vec3(p[0],p[1],0.0).length();
		geometry._vertices[i] += Vec3::Z * std::sin(d * 2.0 * 3.1415 * freq2) * ampl2;
	}
	geometry.generateNormals();
	geometry.generateTexCoords(Geometry::TexGenMode_Cubic,2.0);
	
    return geometry;
}

// -----------------------------------------------------------------------------------------------------------------------
void loadSamplers(ImageSampler::Ptr& colorSampler, ImageSampler::Ptr& normalSampler)
{
	if( !fileExists("./cache/scene_color.tga") || !fileExists("./cache/scene_normals.tga") )
	{
		// c++17
		std::filesystem::create_directories("cache");
		generateImageFromJson("textures.json");
	}
	
	colorSampler =ImageSampler::create();
	colorSampler->setSource( ImageIO::load("./cache/scene_color.tga") );
	colorSampler->setMode(ImageSampler::Mode_Repeat);
	colorSampler->setInterpo(ImageSampler::Interpo_Linear);
	
	normalSampler =ImageSampler::create();
	normalSampler->setSource( ImageIO::load("./cache/scene_normals.tga") );
	normalSampler->setMode(ImageSampler::Mode_Repeat);
	normalSampler->setInterpo(ImageSampler::Interpo_Linear);
}

int main(int argc, char* argv[])
{
	
	ImageSampler::Ptr colorSampler, normalSampler;
	loadSamplers(colorSampler, normalSampler);
	
	sf::Clock clock;
	
	sf::RenderWindow window(sf::VideoMode(512, 512), "My window", sf::Style::Default, sf::ContextSettings(24));
	window.setFramerateLimit(60);
	
	Graph::Ptr graph = Graph::create();
	Node::Ptr root = ClearNode::create();
	
	Camera::Ptr camera = Camera::create();
	camera->setPosition(Vec3(3.0f, 0.0f, 0.0f));
	camera->setTarget(Vec3(0.0f, 0.0f, 0.0f));
	
	Geometry lineVol = Geometry::cylinder(10,1.0,0.02);
	lineVol.generateNormals();
	lineVol.generateTexCoords();
	Geometry arrowVol = Geometry::cone(10,0.1,0.05,0.0);
	arrowVol.generateNormals();
	arrowVol.generateTexCoords();
	arrowVol += Vec3::Z;
	Geometry zAxe = arrowVol + lineVol;
	Geometry xAxe = zAxe; xAxe.rotY(1.57);
	Geometry yAxe = zAxe; yAxe.rotX(-1.57);
	xAxe.generateColors(Vec3(1.0,0.0,0.0));
	yAxe.generateColors(Vec3(0.0,1.0,0.0));
	zAxe.generateColors(Vec3(0.0,0.0,1.0));
	Geometry::Ptr coords = Geometry::create();
	*coords = xAxe + yAxe + zAxe;
	coords->setPrimitive(Geometry::Primitive_Triangles);
	(*coords) *= 0.2;

	Geometry point = Geometry::sphere(4, 0.1);
	point.setPrimitive(Geometry::Primitive_Triangles);
	point.generateColors(Vec3(1.0));

	GeoNode::Ptr node2 = GeoNode::create(coords, false);
	node2->setPosition(Vec3(0.0,0.0,0.0));
	
	GeoNode::Ptr node3 = GeoNode::create(coords, false);
	node3->setPosition(Vec3(0.0,0.0,0.3));
	node3->setRotation(Vec3(3.14 * 0.1, 0.0, 3.14 * 0.25));
	
	GeoNode::Ptr node4 = GeoNode::create(coords, false);
	node4->setPosition(Vec3(0.0,0.0,0.3));
	node4->setRotation(Vec3(3.14 * 0.1, 0.0, 3.14 * 0.25));
	

	GeoNode::Ptr node1 = GeoNode::create(coords, true);
	node1->setPosition(Vec3(0.3,0.0,-0.2));
	
	imp::ReflexionModel::Ptr r = imp::ReflexionModel::create(
		imp::ReflexionModel::Lighting_Phong,
		imp::ReflexionModel::Texturing_Customized);
	
	Material::Ptr material = Material::create(Vec3(1.0), 1.0);
	material->_baseColor = colorSampler;
	material->_normalmap = normalSampler;
	
	r->_fragCode_texturing = fragSimple;
	Geometry::Ptr mush = Geometry::create();
	*mush = generateLeaf();
	// Geometry mush = generateRockHat(1.0, 8.0);
	GeoNode::Ptr geomush = GeoNode::create(mush, false);
	geomush->setPosition(Vec3(1.0,0.0,0.0));
	geomush->setRotation(Vec3(0.0,90,0.0));
	geomush->setScale(Vec3(0.3));
	geomush->setReflexion(r);
	geomush->setMaterial(material);
	
	LightNode::Ptr light = LightNode::create(Vec3(1.0),30.f);
	
	root->addNode(camera);
	root->addNode(geomush);
	root->addNode(node1);
	root->addNode(node2);
	node1->addNode(light);
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
		// geomush->setPosition(Vec3(cos(t+3.14)*5.0,sin(t+3.14)*5.0,0.0));
		// geomush->setRotation(Vec3(t * 0.1, 0.0, t * 0.25));
		
		// rendering
		renderer->render( graph );
		window.display();
    }
    
    // ImageIO::save(tgt->get(0)->getSource(),"test.tga");

	exit(EXIT_SUCCESS);
}
