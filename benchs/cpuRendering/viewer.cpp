#include <Geometry/Geometry.h>
#include <SceneGraph/Camera.h>
#include <Graphics/Image.h>
#include <Graphics/ImageOperation.h>
#include <Graphics/GeometryRenderer.h>

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

using namespace imp;

#define INTERN_RES 128
#define GEO_SUBDIV 3
#define USE_BMPFORMAT false
 
struct Light
{
    Vec3 position;
    Vec3 color;
    float attn;
};

Light light_1;

// -----------------------------------------------------------------------------------------------------------------------  
Geometry ball(const Vec3& center, float radius)
{
    Geometry geo = Geometry::sphere(GEO_SUBDIV,radius);
	Geometry::intoCCW( geo );
	geo.setColors( std::vector<Vec3>(geo.size(),Vec3(1.0)) );
    return geo;
}

// -----------------------------------------------------------------------------------------------------------------------  
Geometry mushroom(const Vec3& center, float radius, int sub)
{
	Geometry geo = Geometry::cylinder(sub, 1.5, radius*0.7);
	Geometry geo2 = Geometry::cone(sub, 1.5, radius*2.0, radius*0.3);
	geo2 += Vec3(0.0,0.0,1.5);
	
	geo += geo2;
	Geometry::intoCCW( geo );
	
    geo.setColors( std::vector<Vec3>(geo.size(),Vec3(0.0,1.0,0.0)) );
	
	std::vector<Geometry::TexCoord> tex(geo.size());
	std::vector<Vec3> normals(geo.size());
	for(int i=0;i<geo.size();++i)
	{
		float u = std::atan2(geo[i].y(),geo[i].x()) / (3.141592) * 0.5 + 0.5; u *= 4.0;
		float v = geo[i].z();
		
		tex[i] = Geometry::TexCoord(u,v);
		normals[i] = geo[i]; normals[i].normalize();
	}
	geo.setTexCoords( tex );
	geo.setNormals( normals );
       
    return geo;
}

// -----------------------------------------------------------------------------------------------------------------------  
Geometry ground(const Vec3& center, float radius, int sub)
{
    Geometry geo = Geometry::cylinder(sub*2.0, 1.0, radius*0.7);
	Geometry::intoCCW( geo );
	geo.setColors( std::vector<Vec3>(geo.size(),Vec3(1.0)) );
	std::vector<Vec3> normals(geo.size());
	for(int i=0;i<geo.size();++i) {normals[i]=geo[i]; normals[i].normalize(); }
	geo.setNormals( normals );
    return geo;
}
 
// -----------------------------------------------------------------------------------------------------------------------
struct PhongFragCallback : public FragCallback
{
    Meta_Class(PhongFragCallback)
	virtual void exec(ImageBuf& targets, const Vec3& pt, const CnstUniforms& cu, Uniforms* uniforms = nullptr)
	{
		const Matrix4& view = *(cu.at("u_view")->getValue().value_mat4v);
		
		Vec3 light_pos = light_1.position;
		Vec3 frag_pos = uniforms->get("m_vert");
		const float* md = view.data();
		Vec3 cam_pos(md[12],md[13],md[14]);
		
		Vec3 light_dir = light_pos - frag_pos;
		Vec3 cam_dir = cam_pos - frag_pos;
		light_dir.normalize();
		cam_dir.normalize();
		
		Vec3 normal = uniforms->get("normal");
		 
		float a = clamp( light_dir.dot(normal) );
		Vec3 reflection = (normal * 2.0 * a) - light_dir;
		float s = clamp( reflection.dot(cam_dir) );
		
		Vec4 color = uniforms->get("color");
		Vec3 tex = uniforms->get("texUV");
		TextureSampler::Ptr sampler = cu.at("u_sampler")->getSampler();
		if(sampler) color *= sampler->get(tex);
		
		float light_lvl = clamp( 0.2 + 0.6*a + pow(s, 8) );
		Vec4 base_color = Vec4(dotClamp( Vec3(light_lvl) * color ));
        base_color *= 255.f;
		targets[1]->setPixel(pt[0],pt[1],base_color);
	}
};

// -----------------------------------------------------------------------------------------------------------------------
struct LightFrag : public FragCallback
{
    Meta_Class(LightFrag)
	virtual void exec(ImageBuf& targets, const Vec3& pt, const CnstUniforms& cu, Uniforms* uniforms = nullptr)
	{
		targets[1]->setPixel(pt[0],pt[1],Vec4(255.0));
	}
};

// -----------------------------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	Vec3 observer(10.0, 0.0, 3.0);
     
    light_1.position = Vec3(2.0,1.0,1.0);
    light_1.color = Vec3(1.0,1.0,1.0);
    light_1.attn = 0.7;
	
	// texture
	Image::Ptr hash = Image::create(INTERN_RES,INTERN_RES,3);
	HashOperation hash_op;
	hash_op.setSeed(76482.264);
	hash_op.execute(hash);
	
	Image::Ptr fbm = Image::create(INTERN_RES,INTERN_RES,3);
	FbmOperation fbm_op;
	fbm_op.setType(1);
	fbm_op.setHashmap(hash);
	fbm_op.setFreq(4.0);
	fbm_op.setOctaves(2);
	fbm_op.setPersist(0.7);
	fbm_op.execute(fbm);
	
	Image::Ptr tex = Image::create(INTERN_RES,INTERN_RES,3);
	ColorMixOperation color_op;
	color_op.setTarget(fbm);
	color_op.setColor1( Vec4(0.8,0.8,0.8,1.0) );
	color_op.setColor2( Vec4(0.2,0.2,1.0,1.0) );
	color_op.execute(tex);
	
	TextureSampler::Ptr sampler =TextureSampler::create();
	sampler->setSource(tex);
	sampler->setMode(TextureSampler::Mode_Repeat);
	sampler->setInterpo(TextureSampler::Interpo_Linear);
	
	Image::Ptr rgbtarget = Image::create(INTERN_RES,INTERN_RES,4);

	double a = 0.0;
	
	PhongFragCallback::Ptr phongFrag = PhongFragCallback::create();
	LightFrag::Ptr lightFrag = LightFrag::create();
    
	Geometry mushr = mushroom(Vec3(0.0), 1.0, 2*GEO_SUBDIV);
	Geometry torch = ball(Vec3(0.0), 0.2);
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
	
	Matrix4 model, view;
	Matrix4 proj = Matrix4::perspectiveProj(60.0, 1.0, 0.1, 10.0);
	Vec4 vp(0.0,0.0,INTERN_RES,INTERN_RES);
	
	GeometryRenderer georender;
	georender.setTarget(0, rgbtarget, Vec4(100.0,100.0,255.0,255.0) );
	georender.setProj( model );
	georender.setViewport( vp );
	
	Uniform::Ptr u_proj = Uniform::create("u_proj",Uniform::Type_Mat4v);
	Uniform::Ptr u_view = Uniform::create("u_view",Uniform::Type_Mat4v);
	Uniform::Ptr u_model = Uniform::create("u_model",Uniform::Type_Mat4v);
	Uniform::Ptr u_vp = Uniform::create("u_vp",Uniform::Type_4f);
	Uniform::Ptr u_sampler = Uniform::create("u_sampler",Uniform::Type_Sampler);
	CnstUniforms uniforms;
	uniforms["u_proj"] = u_proj;
	uniforms["u_view"] = u_view;
	uniforms["u_model"] = u_model;
	uniforms["u_vp"] = u_vp;
	uniforms["u_sampler"] = u_sampler;
	georender.setUniforms(uniforms);
	
	u_proj->set( &proj );
	u_vp->set( &vp );
	u_sampler->set(sampler);
	
	while (win.isOpen())
	{
		sf::Event ev;
		while (win.pollEvent(ev)) if (ev.type == sf::Event::Closed) win.close();
		
		a += 0.05; if(a > 6.28) a = 0.0;
		view = Matrix4::view(observer * Matrix3::rotationZ(a), Vec3(0.0), Vec3::Z);
		u_view->set( &view );
		
		georender.clearTargets();
		
		// georender.setFragCallback( phongFrag );
		georender.setDefaultFragCallback();
		
		model = Matrix4::translation(0.0, 0.0, -1.0);
		u_model->set( &model );
		georender.render( plane );
		
		georender.setFragCallback( lightFrag );
		
		model = Matrix4::translation(light_1.position);
		u_model->set( &model );
		georender.render( torch );
		
		georender.setFragCallback( phongFrag );
		
		model = Matrix4();
		u_model->set( &model );
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


