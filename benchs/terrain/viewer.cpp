#include <SceneGraph/Graph.h>
#include <SceneGraph/Camera.h>
#include <SceneGraph/GeoNode.h>
#include <Descriptors/ImageIO.h>
#include <Descriptors/FileInfo.h>
#include <Descriptors/JsonImageOp.h>

#include <Renderer/GlRenderer.h>
#include <Renderer/CpuRenderer.h>

#include <SFML/Graphics.hpp>
#include <filesystem>

using namespace imp;

Geometry generateTerrain(const ImageSampler::Ptr& hm)
{
    imp::Geometry geometry = imp::Geometry::cube();
	geometry = geometry.subdivise(10);
	geometry.generateColors(Vec3(1.0));
	geometry.generateTexCoords(Geometry::TexGenMode_Cubic,4.0);
	geometry.scale(Vec3(1.0,1.0,0.01));
	
	for(int k=0;k<geometry.size();++k)
	{
		Vec3 uv = geometry._vertices[k];
		uv = (uv * 0.5)  + 0.5;
		Vec3 v = geometry._vertices[k];
		if(v[2]>0.0) v[2] += hm->get(uv)[0] * 3.0;
		geometry._vertices[k] = v;
	}
	geometry.scale(Vec3(5.0,5.0,1.0));
	geometry.generateNormals(Geometry::NormalGenMode_PerFace);
	geometry.interpolateNormals();
       
    return geometry;
}

// -----------------------------------------------------------------------------------------------------------------------
void loadSamplers(ImageSampler::Ptr& sampler, ImageSampler::Ptr& color)
{
	if( !fileExists("./cache/scene_terrain.tga") || !fileExists("./cache/scene_color.tga"))
	{
		// c++17
		std::filesystem::create_directories("cache");
		generateImageFromJson("textures.json");
	}
	
	sampler =ImageSampler::create();
	sampler->setSource( ImageIO::load("./cache/scene_terrain.tga") );
	sampler->setMode(ImageSampler::Mode_Repeat);
	sampler->setInterpo(ImageSampler::Interpo_Linear);
	
	color =ImageSampler::create();
	color->setSource( ImageIO::load("./cache/scene_color.tga") );
	color->setMode(ImageSampler::Mode_Repeat);
	color->setInterpo(ImageSampler::Interpo_Linear);
}

int main(int argc, char* argv[])
{
	std::string arg = "";
	if(argc>1) arg = argv[1];
	
	ImageSampler::Ptr sampler, color;
	loadSamplers(sampler,color);
	Image::Ptr target;
	
	sf::Clock clock;
	
	sf::RenderWindow window(sf::VideoMode(512, 512), "My window", sf::Style::Default, sf::ContextSettings(24));
	window.setFramerateLimit(60);
	sf::Texture texture; texture.create(512, 512); // texture.setSmooth(true);
	sf::Sprite sprite(texture);
	
	Graph::Ptr graph = Graph::create();
	Node::Ptr root = ClearNode::create();
	
	Camera::Ptr camera = Camera::create();
	camera->setPosition(Vec3(10.0f, 10.0f, 10.0f));
	camera->setTarget(Vec3(0.0f, 0.0f, 0.0f));
	
	imp::ReflexionModel::Ptr r = imp::ReflexionModel::create(
		imp::ReflexionModel::Lighting_Phong,
		imp::ReflexionModel::Texturing_Samplers_CNE);
	
	Material::Ptr material = Material::create(Vec3(0.0,1.0,0.0), 1.0);
	material->_baseColor = color;
	
	Geometry::Ptr terrain = Geometry::create();
	*terrain = generateTerrain(sampler);
	GeoNode::Ptr geomush = GeoNode::create(terrain, false);
	// geomush->setScale(Vec3(0.3));
	geomush->setReflexion(r);
	geomush->setMaterial(material);
	
	LightNode::Ptr light = LightNode::create(Vec3(1.0),30.f);
	
	root->addNode(camera);
	root->addNode(geomush);
	root->addNode(light);
	graph->setRoot(root);
	
	int RES = 128;
	Vec4 viewport(0.0,0.0,512,512);
	SceneRenderer::Ptr renderer;
	if(arg != "-gpu")
		renderer = CpuRenderer::create();
	else
		renderer = GlRenderer::create();
	
	target = Image::create(RES,RES,4);
	// target->create(RES,RES,1,true);
	renderer->setTarget(target);
	
	if(arg != "-gpu")
	{
		viewport.set(0.0,0.0,RES,RES);
		renderer->setDirect(false);
	}
	else
	{
		GlRenderer& g = dynamic_cast<GlRenderer&>( *renderer );
		g.loadRenderPlugin("libglPlugin");
	}
	
	graph->getInitState()->setViewport( viewport );
	
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
		
		Vec3 lp(cos(t)*8.0,sin(t)*6.0,8.0);
		light->setPosition(lp);
		
		// rendering
		renderer->render( graph );
		if(arg != "-gpu") 
		{
			texture.update(target->data(),target->width(),target->height(),0,0);
			sprite.setScale( 512.0 / target->width(), -512.0 / target->height() );
			sprite.setPosition( 0, 512 );
			window.draw(sprite);
		}
		window.display();
    }
    
    // ImageIO::save(tgt->get(0)->getSource(),"test.tga");

	exit(EXIT_SUCCESS);
}
