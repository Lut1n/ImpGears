#include <Geometry/ProceduralGeometry.h>
// #include <SceneGraph/BmpLoader.h>
#include <SceneGraph/DefaultShader.h>
#include <SceneGraph/GraphicRenderer.h>
#include <SceneGraph/RenderTarget.h>
#include <SceneGraph/ScreenVertex.h>
#include <Core/Vec4.h>
#include <Core/Matrix4.h>
#include <Core/Perlin.h>


#include <SFML/Graphics.hpp>

#include <Utils/FileInfo.h>
  
#include <ctime>
#include <vector>
#include <map>

using Vec4 = imp::Vec4;
using Vec3 = imp::Vec3;

#include "Core.h"
  
#define CONFIG_NEAR 0.1
#define CONFIG_FAR 100.0
#define CONFIG_FOV 60.0
#define CONFIG_WIDTH 500.0
#define CONFIG_HEIGHT 500.0
  
  
  
  
  
struct RenderState
{
    imp::Matrix4 projection;
    imp::Matrix4 view;
    imp::Matrix4 model;
    Vec4 viewport;
};
  
RenderState state;
 
struct Light
{
    Vec3 position;
    Vec3 color;
    float attn;
};

Light light_1;

#define Varying_Color 0
#define Varying_Vert 1
#define Varying_MVert 2
#define Varying_MVVert 3
#define Varying_MVPVert 4
// #define Varying_Count 5

#include "engine.h"
#include "model.h"


struct Screen
{
	sf::RenderWindow* window;
	imp::GraphicRenderer::Ptr renderer;
	imp::LayeredImage::Ptr data;
	imp::Texture::Ptr texture;
	imp::RenderParameters::Ptr screenParameters;
	imp::Shader::Ptr defaultShader;
	imp::ScreenVertex::Ptr screen;
	imp::Camera::Ptr camera;
	imp::Uniform::Ptr u_tex;
	
	Screen()
	{
		window = new sf::RenderWindow(sf::VideoMode(500, 500), "My window", sf::Style::Default, sf::ContextSettings(24));
		window->setFramerateLimit(60);
		window->setTitle("Cpu Renderer");

		renderer = imp::GraphicRenderer::create();

		data = imp::LayeredImage::create();
		data->build(500,500,4);
		texture = imp::Texture::create();
		texture->setSmooth(true);
		texture->loadFromLayeredImage(data);
		
		// screen render parameters
		screenParameters = imp::RenderParameters::create();
		screenParameters->setOrthographicProjection(0.f, 1.f, 0.f, 1.f, 0.f, 1.f);
		screenParameters->setClearColor(imp::Vec3(0.f, 0.f, 1.f));
		screenParameters->setViewport(0.0, 0.0, 500.0, 500.0);
		renderer->setRenderParameters(screenParameters);
		
		std::string c_vert = loadShader("defaultshader.vert");
		std::string c_frag = loadShader("defaultshader.frag");
		defaultShader = imp::Shader::create(c_vert.c_str(),c_frag.c_str());
		screen = imp::ScreenVertex::create();
		screen->getGraphicState()->setShader(defaultShader);
		renderer->setSceneRoot(screen);
		
		u_tex = imp::Uniform::create("u_colorTexture", imp::Uniform::Type_1i);
		u_tex->set(0);
		defaultShader->addUniform(u_tex);
	}
	
	virtual ~Screen()
	{
		delete imp::VBOManager::getInstance();
	}
	
	
	std::string loadShader(const char* filename)
	{
		std::string buffer;
		std::ifstream is;
		is.open(filename);
		while( !is.eof() )
		{
			char c = is.get();
			
			if(c == '\n' || c == '\r')
				c = '\n';
			
			if(!is.eof())		
				buffer.push_back(c);
		}
		is.close();
		
		// std::cout << filename << " : \n " << buffer << std::endl;
		
		return buffer;
	}


};

// -----------------------------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    Vec3 cam_position(0.0, 20.0, 0.0);
    Vec3 cam_target(0.0, 0.0, 0.0);
    Vec3 cam_up(0.0, 0.0, 1.0);
    Vec3 rock_center(0.0,0.0,0.0);
      
    state.viewport = Vec4(0.0,0.0,CONFIG_WIDTH,CONFIG_HEIGHT);
    state.projection = imp::Matrix4::getPerspectiveProjectionMat(CONFIG_FOV, CONFIG_WIDTH/CONFIG_HEIGHT, CONFIG_NEAR, CONFIG_FAR);
    state.view = imp::Matrix4::getViewMat(cam_position, cam_target, cam_up);
     
    light_1.position = Vec3(2.0,2.0,-1.0);
    light_1.color = Vec3(1.0,1.0,1.0);
    light_1.attn = 0.7;
    
    std::vector<float> quad;
    quad.push_back(-1.0); quad.push_back(-1.0); quad.push_back(0.0);
    quad.push_back(1.0); quad.push_back(-1.0); quad.push_back(0.0);
    quad.push_back(1.0); quad.push_back(1.0); quad.push_back(0.0);
    quad.push_back(1.0); quad.push_back(1.0); quad.push_back(0.0);
    quad.push_back(-1.0); quad.push_back(1.0); quad.push_back(0.0);
    quad.push_back(-1.0); quad.push_back(-1.0); quad.push_back(0.0);
    std::vector<float> qcolor; qcolor.resize(18, 0.5);
       
    std::vector<float> planeGeo = generatePlane(rock_center, 10.0, 30);
    std::vector<float> planeCol = generateColors2(planeGeo);
     
    std::vector<float> rockGeo = generateRock(rock_center, 3.0, 100);
    std::vector<float> rockCol = generateColors(rockGeo);
     
    //std::vector<float> hatGeo = generateHat(rock_center, 4.0, 100);
    //std::vector<float> hatCol = generateColors4(hatGeo);
    
    std::vector<float> torchGeo = generateTorch(rock_center, 0.2, 10);
    std::vector<float> torchCol = generateColors3(torchGeo);
       
    imp::ImageData target, backbuffer;
    target.build(CONFIG_WIDTH, CONFIG_HEIGHT, imp::PixelFormat_BGR8, nullptr);
    backbuffer.build(CONFIG_WIDTH, CONFIG_HEIGHT, imp::PixelFormat_BGR8, nullptr);
       
    double a = 0.0;
    // std::time_t start = time(nullptr);
	
	Screen* screen = new Screen();

	while (screen->window->isOpen())
	{
		sf::Event event;
		while (screen->window->pollEvent(event))
		{
			if (event.type == sf::Event::Closed) screen->window->close();
		}
		
		screen->texture->bind();
		screen->renderer->renderScene();

		screen->window->display();
		a += 0.1;
		
		imp::Matrix4 rot = imp::Matrix4::getRotationMat(0.0, 0.0, a);
		cam_position = (Vec3)(Vec4(0.0, 10.0, 5.0,1.0) * rot);
		state.view = imp::Matrix4::getViewMat(cam_position, cam_target, cam_up);
		
		renderVertex(quad,qcolor,target,backbuffer, clearVert, clearFrag);
			
		state.model = imp::Matrix4::getTranslationMat(0.0, 0.0, -2.0);
		renderVertex(planeGeo, planeCol, target, backbuffer,defaultVert, terrFrag);
		state.model = imp::Matrix4::getTranslationMat(0.0, 0.0, 1.0);
		renderVertex(rockGeo, rockCol, target, backbuffer,defaultVert, defaultFrag);
		// state.model = imp::Matrix4::getTranslationMat(0.0, 0.0, 3.0);
		// renderVertex(hatGeo, hatCol, target, backbuffer,defaultVert, defaultFrag);
		state.model = imp::Matrix4::getTranslationMat(2.0, 2.0, -1.0);
		renderVertex(torchGeo, torchCol, target, backbuffer,defaultVert, lightFrag);
			
		screen->texture->loadFromImageData(&target);
		// BmpLoader::saveToFile(&target, "test.bmp");
		// start = time(nullptr);
	}
   
    return 0;
}


