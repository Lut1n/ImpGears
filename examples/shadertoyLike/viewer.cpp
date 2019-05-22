#include <Renderer/Uniform.h>
#include <Renderer/SceneRenderer.h>
#include <SceneGraph/Graph.h>
#include <SceneGraph/Camera.h>
#include <Renderer/Target.h>

#include <SFML/Graphics.hpp>
#include <SceneGraph/QuadNode.h>

#include <Descriptors/ImageIO.h>

#include <Descriptors/FileInfo.h>
#include <fstream>



void hsym(imp::Image& image, const imp::Vec4& col)
{
    for(int i=0;i<image.width();++i)
    {
        for(int j=0;j<image.height();++j)
        {
            imp::Vec4 cc = image.getPixel(i,j); cc[3] = 255.0;
            if(cc == col)image.setPixel(image.width()-1-i,j,col);
        }
    }
}

imp::Image::Ptr generateImage()
{
    imp::Image::Ptr image = imp::Image::create(12,12,3);
    
    imp::Vec4 bgcol(0,0,0,255);
    imp::Vec4 fgcol(255,255,0,255);
	
    imp::Rasterizer rast;
    rast.setTarget(image);
    rast.setColor(fgcol);
    
    float offsetX = 6.0;
    float offsetY = 2.0;
    image->fill(bgcol);
    
    rast.square(imp::Vec3(offsetX+2.0,offsetY+4.0,0.0),4.0);
    rast.grid(imp::Vec3(offsetX+2.0,offsetY+0.0,0.0),imp::Vec3(offsetX+3.0,offsetY+7.0,0.0),1);
    rast.rectangle(imp::Vec3(offsetX+5.0,offsetY+1.0,0.0),imp::Vec3(offsetX+5.0,offsetY+3.0,0.0));
    rast.rectangle(imp::Vec3(offsetX+4.0,offsetY+3.0,0.0),imp::Vec3(offsetX+4.0,offsetY+4.0,0.0));
    rast.dot(imp::Vec3(offsetX+1.0,offsetY+0.0,0.0));
    
    rast.setColor(bgcol);
    rast.dot(imp::Vec3(offsetX+2.0,offsetY+4.0,0.0));
    
    hsym(*image.get(),fgcol);
	
    imp::ImageIO::save(image,"out.tga");
    
    return image;
}

std::vector<std::string> getarg(int argc, char* argv[])
{
	std::vector<std::string> result;
	result.resize(argc);
	for(int i=0;i<argc;++i) result[i] = std::string(argv[i]);
	return result;
}

int main(int argc, char* argv[])
{
	char default_shader[] = "defaultshader.frag";
	std::vector<std::string> arg = getarg(argc,argv);
	if(arg.size() < 2) arg.push_back(default_shader);
	
	sf::RenderWindow window(sf::VideoMode(512, 512), arg[1], sf::Style::Default, sf::ContextSettings(24));
	window.setFramerateLimit(60);

    time_t accessLast, modifLast, statusLast;
	imp::getTimeInfo(default_shader, accessLast, modifLast, statusLast);

	imp::Graph::Ptr graph = imp::Graph::create();
	graph->setClearColor(imp::Vec4(1.0,0.0,0.0,1.0));
	imp::State::Ptr state = graph->getInitState();
	state->setOrthographicProjection(0.f, 1.f, 0.f, 1.f, 0.f, 1.f);
    state->setViewport(0.0, 0.0, 512.0, 512.0);

    std::cout << "defaultshader.vert + " << arg[1] << std::endl;
	std::string c_vert = imp::loadText("defaultshader.vert");
	std::string c_frag = imp::loadText(arg[1]);
	imp::ReflexionModel::Ptr shader = imp::ReflexionModel::create(imp::ReflexionModel::Lighting_Phong, imp::ReflexionModel::Texturing_Customized);
	// shader->_vertCode = c_vert;
	shader->_fragCode_texturing = c_frag;
	
    imp::Uniform::Ptr u_time = imp::Uniform::create("u_timer",imp::Uniform::Type_1f);
	u_time->set(0.f);
	
	imp::Image::Ptr image = generateImage();
	imp::TextureSampler::Ptr sampler = imp::TextureSampler::create();
	sampler->setSource(image);
	imp::Uniform::Ptr u_sampler = imp::Uniform::create("u_test",imp::Uniform::Type_Sampler);
	u_sampler->set(sampler);
	
	imp::Uniform::Ptr u_color = imp::Uniform::create("u_color",imp::Uniform::Type_3f);
	imp::Uniform::Ptr u_lightPos = imp::Uniform::create("u_lightPos",imp::Uniform::Type_3f);
	imp::Uniform::Ptr u_lightCol = imp::Uniform::create("u_lightCol",imp::Uniform::Type_3f);
	imp::Uniform::Ptr u_lightAtt = imp::Uniform::create("u_lightAtt",imp::Uniform::Type_3f);
	u_color->set(imp::Vec3(1.0));
	u_lightPos->set(imp::Vec3(0.0,0.0,1.0));
	u_lightCol->set(imp::Vec3(1.0,1.0,1.0));
	u_lightAtt->set(imp::Vec3(3.0,8.0,0.0));
	
	imp::Node::Ptr screen = imp::QuadNode::create();
    state->setShader(shader);
	state->setUniform(u_color);
	state->setUniform(u_time);
	state->setUniform(u_sampler);
	state->setUniform(u_lightPos);
	state->setUniform(u_lightCol);
	state->setUniform(u_lightAtt);
	graph->setRoot(screen);
	
	// imp::Target::Ptr target = imp::Target::create();
	// target->create(512, 512,1);
	// state->setTarget(target);

    sf::Clock timer;
	
	imp::SceneRenderer::Ptr renderer = imp::SceneRenderer::create();
    
	bool breakLoop = false;
	while (window.isOpen())
	{
        sf::Event event;
        while (window.pollEvent(event)) if (event.type == sf::Event::Closed) {breakLoop=true; break;} //window.close();
		if(breakLoop) break;
        
		// reload if changed
		time_t access, modif, status;
		imp::getTimeInfo(default_shader, access, modif, status);
		if(modif != modifLast)
		{   
            std::cout << "[reload shaders] defaultshader.vert + " << arg[1] << std::endl;
			modifLast = modif;
			c_frag = imp::loadText(arg[1]);
			shader->_d = NULL;
			// shader->vertCode = c_vert;
			shader->_fragCode_texturing = c_frag;
            screen->getState()->setShader(shader);
		}
		
		float sec = timer.getElapsedTime().asMilliseconds() / 1000.0;
        u_time->set(sec);
		u_lightPos->set(imp::Vec3(std::sin(sec)*2.0,0.0,1.0));
		state->setUniform(u_time);
		state->setUniform(u_lightPos);
		
        renderer->render( graph );

		window.display();
	}
	
	
	// imp::Image::Ptr result = target->get(0);
	// imp::ImageIO::save(result, "test.tga");

	return EXIT_SUCCESS;
}
