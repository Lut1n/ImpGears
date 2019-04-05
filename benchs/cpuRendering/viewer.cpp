#include <Geometry/Geometry.h>
#include <SceneGraph/DefaultShader.h>
#include <SceneGraph/GraphRenderer.h>
#include <SceneGraph/RenderTarget.h>
#include <SceneGraph/QuadNode.h>
#include <SceneGraph/Camera.h>
#include <Graphics/Image.h>
#include <Graphics/Rasterizer.h>

#include <Core/Vec4.h>
#include <Core/Matrix4.h>
#include <Core/Perlin.h>
#include <fstream>

#include <SFML/Graphics.hpp>

#include <Utils/FileInfo.h>

#include <ctime>
#include <vector>
#include <map>


using namespace imp;


#define CONFIG_NEAR 0.1
#define CONFIG_FAR 100.0
#define CONFIG_FOV 60.0

#define SQ_RESOLUTION 128
#define GEO_SUBDIV 1
#define USE_BMPFORMAT false


#define CONFIG_WIDTH SQ_RESOLUTION
#define CONFIG_HEIGHT SQ_RESOLUTION

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

std::string Varying_Color("Color");
std::string Varying_Vert( "Vert");
std::string Varying_MVert( "MVert");
std::string Varying_MVVert( "MVVert");
std::string Varying_MVPVert( "MVPVert");
#define Varying_Count 5

imp::Rasterizer global_rast;

// -----------------------------------------------------------------------------------------------------------------------
 struct VertCallback
 {
    virtual void operator()(Vec3& vert, Vec3& col, Uniforms& uniforms) = 0;
 };
 
// -----------------------------------------------------------------------------------------------------------------------
void renderVertex(std::vector<Vec3>& buf, std::vector<Vec3>& col, Image::Ptr* targets, VertCallback& vertexOp, FragCallback::Ptr fragCallback)
{    
    global_rast.clearTarget();
    global_rast.addTarget(targets[0]);
    global_rast.addTarget(targets[1]);
    global_rast.setFragCallback(fragCallback);
	
	Uniforms uniforms[3];	
	Vec3 mvpVertex[3];
    
	// int i = 0;
	for(int i=0;i<(int)buf.size();i+=3)
	{
        for(int k=0;k<3;++k)
        {
            vertexOp(buf[i+k],col[i+k],uniforms[k]);
            mvpVertex[k] = uniforms[k].get(Varying_MVPVert);
        }
		
		Vec3 p1p2 = mvpVertex[1] - mvpVertex[0];
		Vec3 p1p3 = mvpVertex[2] - mvpVertex[0];
		Vec3 dir = p1p2.cross(p1p3);
		
		if(dir[2] > 0.0)
		{
            global_rast.setUniforms3(uniforms[0],uniforms[1],uniforms[2]);
			global_rast.triangle(mvpVertex[0],mvpVertex[1],mvpVertex[2]);
            // global_rast.wireTriangle(mvpVertex[0],mvpVertex[1],mvpVertex[2]);
		}
	}
}

#include "model.h"


struct Screen
{
	sf::RenderWindow* window;
	imp::GraphRenderer::Ptr renderer;
	imp::Texture::Ptr texture;
	imp::State::Ptr state;
	imp::Shader::Ptr defaultShader;
	imp::QuadNode::Ptr screen;
	imp::Uniform::Ptr u_tex;
    
	Screen()
	{
		window = new sf::RenderWindow(sf::VideoMode(500, 500), "My window", sf::Style::Default, sf::ContextSettings(24));
		window->setFramerateLimit(60);
		window->setTitle("Cpu Renderer");

		renderer = imp::GraphRenderer::create();

		texture = imp::Texture::create();
		texture->setSmooth(true);
		
		
		std::string c_vert = loadShader("defaultshader.vert");
		std::string c_frag = loadShader("defaultshader.frag");
		defaultShader = imp::Shader::create(c_vert.c_str(),c_frag.c_str());
		
		screen = imp::QuadNode::create();
		
		// screen render parameters
		state = screen->getState();
		state->setShader(defaultShader);
		state->setOrthographicProjection(0.f, 1.f, 0.f, 1.f, 0.f, 1.f);
		state->setViewport(0.0, 0.0, 500.0, 500.0);
		
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
        std::ifstream ifs(filename);
        std::string buffer( (std::istreambuf_iterator<char>(ifs) ),
                            (std::istreambuf_iterator<char>()    ) );
		return buffer;
	}


};

// -----------------------------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    Vec3 cam_position(10.0, 0.0, 3.0);
    Vec3 cam_target(0.0, 0.0, 0.0);
    Vec3 cam_up(0.0, 0.0, 1.0);
    Vec3 rock_center(0.0,0.0,0.0);
      
    state.viewport = Vec4(0.0,0.0,CONFIG_WIDTH,CONFIG_HEIGHT);
    state.projection = imp::Matrix4::perspectiveProj(CONFIG_FOV, CONFIG_WIDTH/CONFIG_HEIGHT, CONFIG_NEAR, CONFIG_FAR);
    state.view = imp::Matrix4::view(cam_position, cam_target, cam_up);
     
    light_1.position = Vec3(2.0,2.0,-1.0);
    light_1.color = Vec3(1.0,1.0,1.0);
    light_1.attn = 0.7;
    
	std::vector<Vec3> vertexQuad;
	std::vector<Vec3> colorQuad;
    for(int i=-1;i<2;i+=2) for(int j=-1;j<2;j+=2){ vertexQuad.push_back(Vec3(i,j,0)); }
    colorQuad.resize(6,Vec3(0.5));
	
	Image::Ptr targets[2];
    targets[0] = Image::create(CONFIG_WIDTH,CONFIG_HEIGHT,4);
    targets[1] = Image::create(CONFIG_WIDTH,CONFIG_HEIGHT,1);
    
	
    double a = 0.0;
	
	Screen* screen = new Screen();
 
    defaultFrag = DefaultRenderFrag::create();
    lightFrag = LightRenderFrag::create();
    terrFrag = TerrRenderFrag::create();
    clearFrag = ClearFragCallback::create();
    depthFrag = DepthFrag::create(&global_rast);
    
    
	std::vector<Vec3> vertexPlane = generatePlane(rock_center, 4.0, 2*GEO_SUBDIV);
	std::vector<Vec3> colorPlane = generateColors2(vertexPlane);
	
	std::vector<Vec3> vertexRock = generateRockHat(rock_center, 1.0, 2*GEO_SUBDIV);
	std::vector<Vec3> colorRock = generateColors(vertexRock);
	
	std::vector<Vec3> vertexBall = generateTorch(rock_center, 0.2, std::min(10,1*GEO_SUBDIV));
	std::vector<Vec3> colorBall = generateColors3(vertexBall);
	
	int frames = 0;
	sf::Clock c;
    
    Vec4 fillColor(100.0,100.0,255.0,255.0);
    Vec4 fillDepth(255.0,255.0,255.0,255.0);
	
	while (screen->window->isOpen())
	{
		sf::Event event;
		while (screen->window->pollEvent(event))
		{
			if (event.type == sf::Event::Closed) screen->window->close();
		}
		
		screen->texture->bind();
		SceneNode::Ptr toRender = screen->screen;
		screen->renderer->renderScene( toRender );

		screen->window->display();
		a += 0.02;
		if(a > 2.0*3.141592) a = 0.0;
		
		imp::Matrix4 rot = imp::Matrix4::rotationZ(a);
		Vec3 cam  = Vec3(Vec4(cam_position) * rot);
		state.view = imp::Matrix4::view(cam, cam_target, cam_up);
		
        targets[0]->fill(fillColor);
        targets[1]->fill(fillDepth);
        
		state.model = imp::Matrix4::translation(0.0, 0.0, -2.0);
		renderVertex(vertexPlane, colorPlane, targets,defaultVert, terrFrag);
		state.model = imp::Matrix4::translation(0.0, 0.0, 0.0);
		renderVertex(vertexRock, colorRock, targets,defaultVert, defaultFrag);
		// state.model = imp::Matrix4::translation(0.0, 0.0, 1.0);
		// renderVertex(vertexHat, colorHat, targets,defaultVert, defaultFrag);
		state.model = imp::Matrix4::translation(1.0, 1.0, 0.0);
		renderVertex(vertexBall, colorBall, targets,defaultVert, lightFrag);
		
        screen->texture->loadFromImage(targets[0]);
		frames++;
		
		if(c.getElapsedTime().asMilliseconds() > 1000.0)
		{
			std::cout << "FPS : " << frames << std::endl;
			frames = 0;
			c.restart();
		}
	}
   
    return 0;
}


