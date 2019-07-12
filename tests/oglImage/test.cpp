#include <Graphics/Uniform.h>

#include <SceneGraph/Graph.h>
#include <SceneGraph/Camera.h>
#include <SceneGraph/Material.h>
#include <SceneGraph/QuadNode.h>

#include <Renderer/SceneRenderer.h>

#include <Plugins/RenderPlugin.h>

#include <SFML/Graphics.hpp>

using namespace imp;

void hsym(Image& image, const Vec4& col)
{
	for(int i=0;i<image.width();++i)
	{
		for(int j=0;j<image.height();++j)
		{
			Vec4 cc = image.getPixel(i,j); cc[3] = 255.0;
			if(cc == col)image.setPixel(image.width()-1-i,j,col);
		}
	}
}

Image::Ptr generateImage()
{
	Image::Ptr image = Image::create(12,12,3);

	Vec4 bgcol(0,0,0,255);
	Vec4 fgcol(255,255,0,255);

	Rasterizer rast;
	rast.setTarget(image);
	rast.setColor(fgcol);

	float offsetX = 6.0;
	float offsetY = 2.0;
	image->fill(bgcol);

	rast.square(Vec3(offsetX+2.0,offsetY+4.0,0.0),4.0);
	rast.grid(Vec3(offsetX+2.0,offsetY+0.0,0.0),Vec3(offsetX+3.0,offsetY+7.0,0.0),1);
	rast.rectangle(Vec3(offsetX+5.0,offsetY+1.0,0.0),Vec3(offsetX+5.0,offsetY+3.0,0.0));
	rast.rectangle(Vec3(offsetX+4.0,offsetY+3.0,0.0),Vec3(offsetX+4.0,offsetY+4.0,0.0));
	rast.dot(Vec3(offsetX+1.0,offsetY+0.0,0.0));

	rast.setColor(bgcol);
	rast.dot(Vec3(offsetX+2.0,offsetY+4.0,0.0));

	hsym(*image.get(),fgcol);

	ImageIO::save(image,"out.tga");

	return image;
}


int main(int argc, char* argv[])
{
	sf::RenderWindow window(sf::VideoMode(512, 512), "ImpGears test - OGL", sf::Style::Default, sf::ContextSettings(24));
	window.setFramerateLimit(60);

	Graph::Ptr graph = Graph::create();
	graph->setClearColor(Vec4(1.0,0.0,0.0,1.0));
	State::Ptr state = graph->getInitState();
	state->setOrthographicProjection(-1.f, 1.f, -1.f, 1.f, 0.f, 1.f);
	state->setViewport(0.0, 0.0, 512.0, 512.0);

	ReflexionModel::Ptr shader = ReflexionModel::create(ReflexionModel::Lighting_Phong, ReflexionModel::Texturing_Samplers_CNE);

	QuadNode::Ptr screen = QuadNode::create();
	screen->setReflexion(shader);
	state = screen->getState();

	Image::Ptr image = generateImage();
	Material::Ptr material = Material::create(Vec3(1.0),8.0);
	material->_baseColor = ImageSampler::create(image);

	LightNode::Ptr light = LightNode::create(Vec3(1.0),3.0);

	screen->setMaterial(material);
	screen->addNode(light);

	Node::Ptr root = screen;
	graph->setRoot(root);
	sf::Clock timer;

	RenderPlugin::Ptr plugin = PluginManager::open("libglPlugin");
	SceneRenderer::Ptr renderer = plugin->getRenderer();

	bool breakLoop = false;
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
			if (event.type == sf::Event::Closed) window.close();
		
		float sec = timer.getElapsedTime().asMilliseconds() / 1000.0;
		state->setUniform("u_timer", sec);
		light->setPosition( Vec3(std::sin(sec)*2.0,0.0,1.0) );
		
		renderer->render( graph );

		window.display();
	}

	return EXIT_SUCCESS;
}
