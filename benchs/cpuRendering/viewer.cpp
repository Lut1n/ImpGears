#include <Geometry/Geometry.h>
#include <SceneGraph/Camera.h>
#include <Graphics/Image.h>
#include <Utils/JsonImageOp.h>
#include <Graphics/GeometryRenderer.h>
#include <Graphics/CpuBlinnPhong.h>

#include <Core/Vec4.h>
#include <Core/Matrix3.h>
#include <Core/Matrix4.h>
#include <Core/Perlin.h>
#include <fstream>

#include <SFML/Graphics.hpp>

#include <Utils/FileInfo.h>
#include <Utils/ImageIO.h>

#include <ctime>
#include <vector>
#include <map>
#include <filesystem>

using namespace imp;

#define INTERN_RES 128
#define TEX_RES 256
#define GEO_SUBDIV 3

// -----------------------------------------------------------------------------------------------------------------------  
Geometry ball(const Vec3& center, float radius, const Vec3& color)
{
    Geometry geo = Geometry::sphere(GEO_SUBDIV,radius);
	Geometry::intoCCW( geo );
	geo.generateColors( color );
    return geo;
}

// -----------------------------------------------------------------------------------------------------------------------  
Geometry mushroom(const Vec3& center, float radius, int sub)
{
	// Geometry geo = Geometry::cylinder(sub, 1.5, radius*0.7);
	Geometry geo = Geometry::cylinder(sub, 2.0, radius*1.0);
	Geometry geo2 = Geometry::cone(sub, 1.5, radius*2.0, radius*0.3);
	geo2 += Vec3(0.0,0.0,1.5);
	
	geo += geo2;
	Geometry::intoCCW( geo );
	
    geo.generateColors( Vec3(0.0,0.5,1.0) );
	geo.generateNormals();
	geo.generateTexCoords(4.0);
       
    return geo;
}

// -----------------------------------------------------------------------------------------------------------------------  
Geometry ground(const Vec3& center, float radius, int sub)
{
    Geometry geo = Geometry::cylinder(sub*2.0, 1.0, radius*0.7);
	Geometry::intoCCW( geo );
	geo.generateColors( Vec3(0.3,1.0,0.3) );
	geo.generateNormals();
	geo.generateTexCoords(4.0);
    return geo;
}

// -----------------------------------------------------------------------------------------------------------------------
struct LightFrag : public FragCallback
{
    Meta_Class(LightFrag)
	virtual void exec(ImageBuf& targets, const Vec3& pt, const CnstUniforms& cu, Uniforms* uniforms = nullptr)
	{
		targets[0]->setPixel(pt[0],pt[1],Vec4(uniforms->get("color"),1.0) * 255.0);
	}
};

// -----------------------------------------------------------------------------------------------------------------------
void loadSamplers(TextureSampler::Ptr& colorSampler, TextureSampler::Ptr& normalSampler)
{
	if( !fileExists("./cache/scene_color.tga") || !fileExists("./cache/scene_normals.tga") )
	{
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

// -----------------------------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	Vec3 observer(20.0, 0.0, 3.0);
     
    Vec3 lightPosition(4.0,2.0,2.0);
    Vec3 lightColor(1.0,1.0,1.0);
    Vec3 lightAttn(30.0,4.0,0.0);
	
	TextureSampler::Ptr sampler, nsampler;
	loadSamplers(sampler,nsampler);
	Image::Ptr rgbtarget = Image::create(INTERN_RES,INTERN_RES,4);

	double a = 0.0;
	
	CpuBlinnPhong::Ptr phongFrag = CpuBlinnPhong::create();
	LightFrag::Ptr lightFrag = LightFrag::create();
    
	Geometry mushr = mushroom(Vec3(0.0), 1.0, 2*GEO_SUBDIV);
	Geometry torch = ball(Vec3(0.0), 0.2, lightColor);
	Geometry plane = ground(Vec3(0.0), 4.0, 2*GEO_SUBDIV);
	
	int frames = 0;
	sf::Clock c;
	
	sf::RenderWindow win(sf::VideoMode(500, 500), "CPU rendering");
	win.setFramerateLimit(60);
	win.setTitle("Cpu Renderer");
	sf::Texture texture;
	sf::Sprite sprite;
	texture.create(INTERN_RES,INTERN_RES);
	sprite.setTexture(texture);
	sprite.setPosition( 0, 500 );
	sprite.scale( 500.0/INTERN_RES, -500.0/INTERN_RES );
	
	Matrix4 model;
	Matrix4 proj = Matrix4::perspectiveProj(60.0, 1.0, 0.1, 10.0);
	Vec4 vp(0.0,0.0,INTERN_RES,INTERN_RES);
	Matrix4 view = Matrix4::view(observer, Vec3(0.0), Vec3::Z);
	
	GeometryRenderer georender;
	georender.setTarget(0, rgbtarget, Vec4(100.0,100.0,255.0,255.0) );
	// georender.setProj( model );
	georender.setViewport( vp );
	
	CnstUniforms uniforms;
	uniforms["u_proj"] = Uniform::create("u_proj",Uniform::Type_Mat4);
	uniforms["u_view"] = Uniform::create("u_view",Uniform::Type_Mat4);
	uniforms["u_model"] = Uniform::create("u_model",Uniform::Type_Mat4);
	uniforms["u_vp"] = Uniform::create("u_vp",Uniform::Type_4f);
	uniforms["u_sampler"] = Uniform::create("u_sampler",Uniform::Type_Sampler);
	uniforms["u_nsampler"] = Uniform::create("u_nsampler",Uniform::Type_Sampler);
	uniforms["u_lightPos"] = Uniform::create("u_lightPos",Uniform::Type_3f);
	uniforms["u_lightCol"] = Uniform::create("u_lightCol",Uniform::Type_3f);
	uniforms["u_lightAtt"] = Uniform::create("u_lightAtt",Uniform::Type_3f);
	georender.setUniforms(uniforms);
	
	uniforms["u_proj"]->set( proj );
	uniforms["u_view"]->set( view );
	uniforms["u_vp"]->set( vp );
	uniforms["u_sampler"]->set(sampler);
	uniforms["u_nsampler"]->set(nsampler);
	uniforms["u_lightCol"]->set(lightColor);
	uniforms["u_lightAtt"]->set(lightAttn);
	
	while (win.isOpen())
	{
		sf::Event ev;
		while (win.pollEvent(ev)) if (ev.type == sf::Event::Closed) win.close();
		
		a += 0.05; if(a > 6.28) a = 0.0;
		view = Matrix4::view(observer * Matrix3::rotationZ(a*2.0), Vec3(0.0), Vec3::Z);
		Vec3 lightPos = lightPosition;// * Matrix3::rotationZ(a*2.0);
		uniforms["u_view"]->set( view );
		uniforms["u_lightPos"]->set(lightPos);
		
		georender.clearTargets();
		
		georender.setFragCallback( phongFrag );
		
		uniforms["u_model"]->set( Matrix4::translation(0.0, 0.0, -1.0) );
		georender.render( plane );
		
		georender.setFragCallback( lightFrag );
		
		uniforms["u_model"]->set( Matrix4::translation(lightPos) );
		georender.render( torch );
		
		georender.setFragCallback( phongFrag );
		
		uniforms["u_model"]->set( Matrix4() );
		georender.render( mushr );
		
		texture.update(rgbtarget->data());
		// ImageIO::save(rgbtarget,"output.tga"); // debug
		// break;
		
		win.draw(sprite);
		win.display();
		
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


