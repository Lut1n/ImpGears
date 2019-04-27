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
Geometry neocube(float radius)
{
    Geometry geo = Geometry::cylinder(10, 1.0, 1.0);
	Geometry::intoCCW( geo );
	std::vector<Geometry::TexCoord> tex(geo.size());
	for(int i=0;i<geo.size();++i)
	{
		float u = std::atan2(geo[i].y(),geo[i].x()) / (3.141592) * 0.5 + 0.5;
		float v = geo[i].z() * 0.5 + 0.5;
		tex[i] = Geometry::TexCoord(u,v);
	}
	geo.scale(Vec3(radius));
	geo.setTexCoords( tex );
    return geo;
}

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
	
    geo.setColors( std::vector<Vec3>(geo.size(),Vec3(0.0,1.0,0.0)) );;
	
	std::vector<Geometry::TexCoord> tex(geo.size());
	for(int i=0;i<geo.size();++i)
	{
		float u = std::atan2(geo[i].y(),geo[i].x()) / (3.141592) * 0.5 + 0.5;
		float v = geo[i].z() * 0.5 + 0.5;
		tex[i] = Geometry::TexCoord(u,v);
	}
	geo.setTexCoords( tex );
       
    return geo;
}

// -----------------------------------------------------------------------------------------------------------------------  
Geometry ground(const Vec3& center, float radius, int sub)
{
    Geometry geo = Geometry::cylinder(sub*2.0, 1.0, radius*0.7);
	Geometry::intoCCW( geo );
	geo.setColors( std::vector<Vec3>(geo.size(),Vec3(1.0)) );
    return geo;
}
 
// -----------------------------------------------------------------------------------------------------------------------
struct DepthTestFrag : public FragCallback
{
    Meta_Class(DepthTestFrag)
	virtual void apply(int x, int y, const CnstUniforms& cu, Uniforms& uniforms, Image::Ptr* targets) = 0;
	
	void phong(int x, int y, const CnstUniforms& cu, Uniforms& uniforms, Image::Ptr* targets, const Vec3& normal)
	{
		const Matrix4& view = *(cu.at("u_view")->getValue().value_mat4v);
		
		Vec3 light_pos = light_1.position;
		Vec3 frag_pos = uniforms.get("m_vert");
		const float* md = view.data();
		Vec3 cam_pos(md[12],md[13],md[14]);
		
		Vec3 light_dir = light_pos - frag_pos;
		Vec3 cam_dir = cam_pos - frag_pos;
		light_dir.normalize();
		cam_dir.normalize();
		 
		float a = clamp( light_dir.dot(normal) );
		Vec3 reflection = (normal * 2.0 * a) - light_dir;
		float s = clamp( reflection.dot(cam_dir) );
		
		float light_lvl = clamp( 0.2 + 0.6*a + pow(s, 8) );
		Vec4 base_color = Vec4(dotClamp( Vec3(light_lvl) * uniforms.get("col_vert") ));
        base_color *= 255.f;
		targets[0]->setPixel(x,y,base_color);
	}
	
	virtual void exec(ImageBuf& targets, const Vec3& pt, const CnstUniforms& cu, Uniforms* uniforms = nullptr)
    {
        depthTest(pt[0],pt[1],cu, *uniforms,targets.data());
    }
	
	virtual void depthTest(int x, int y, const CnstUniforms& cu, Uniforms& uniforms, Image::Ptr* targets)
	{
		// depth test
		float depth = -uniforms.get("mv_vert").z();
		
		float depthPx = clamp( linearstep(0.1f, 20.f, depth )  )* 255.0;
        Vec4 depthV = targets[1]->getPixel(x, y);
		float curr_depth = depthV[0];
		if( depthPx < curr_depth)
		{
			apply(x, y, cu, uniforms, targets);
			
			Vec4 depth(depthPx);
			targets[1]->setPixel(x,y,depth);
		}
	}
};
 
// -----------------------------------------------------------------------------------------------------------------------
struct MyFragCallback : public DepthTestFrag
{
    Meta_Class(MyFragCallback)
	virtual void apply(int x, int y, const CnstUniforms& cu, Uniforms& uniforms, Image::Ptr* targets)
	{
		Vec3 normal = uniforms.get("v_vert"); normal.normalize();
		phong(x,y,cu,uniforms, targets, normal);
	}
};

// -----------------------------------------------------------------------------------------------------------------------
struct TexFragCallback : public DepthTestFrag
{
	Image::Ptr src;
	
	TexFragCallback()
		: DepthTestFrag()
	{
		// {"op":"hash", "id":"hash_01", "seed":76482.264},
		// {"op":"fbm", "noise_type":"voronoi", "input_id":"hash_01", "freq":4.0, "octaves":2.0, "persist":0.7, "id":"noise_01"},
		// {"op":"colorMix", "input_id":"noise_01", "r1":0.8, "g1":0.8, "b1":0.8, "r2":0.2, "g2":0.2, "b2":1.0, "id":"color_01"},
		
		Image::Ptr hash = Image::create(128,128,3);
		HashOperation hash_op;
		hash_op.setSeed(76482.264);
		hash_op.execute(hash);
		
		Image::Ptr fbm = Image::create(128,128,3);
		FbmOperation fbm_op;
		fbm_op.setType(1);
		fbm_op.setHashmap(hash);
		fbm_op.setFreq(4.0);
		fbm_op.setOctaves(2);
		fbm_op.setPersist(0.7);
		fbm_op.execute(fbm);
		
		src = Image::create(128,128,3);
		ColorMixOperation color_op;
		color_op.setTarget(fbm);
		color_op.setColor1( Vec4(0.8,0.8,0.8,1.0) );
		color_op.setColor2( Vec4(0.2,0.2,1.0,1.0) );
		color_op.execute(src);
	}
	
    Meta_Class(TexFragCallback)
	virtual void apply(int x, int y, const CnstUniforms& cu, Uniforms& uniforms, Image::Ptr* targets)
	{
		Vec3 tex = uniforms.get("tex_vert");
		ImageSampler sampler(src, ImageSampler::Mode_Repeat);
		sampler.setInterpo(ImageSampler::Interpo_Linear);
		Vec4 color = sampler(tex);
		
		uniforms.set("col_vert",color);
		
		Vec3 normal = uniforms.get("v_vert"); normal.normalize();
		phong(x,y,cu,uniforms, targets, normal);
	}
};

// -----------------------------------------------------------------------------------------------------------------------
struct LightFrag : public DepthTestFrag
{
    Meta_Class(LightFrag)
	virtual void apply(int x, int y, const CnstUniforms& cu, Uniforms& uniforms, Image::Ptr* targets)
	{
		targets[0]->setPixel(x,y,Vec4(255.0));
	}
};

// -----------------------------------------------------------------------------------------------------------------------
 struct MyVertCallback : public GeometryRenderer::VertCallback
 {
	Meta_Class(MyVertCallback)
	
    virtual void exec(const Vec3& vert, const Vec3& col, const Vec3& normal, const Vec3& tex, const CnstUniforms& cu, Uniforms& uniforms, const GeometryRenderer* rder)
    {
		const Matrix4& model = *(cu.at("u_model")->getValue().value_mat4v);
		const Matrix4& view = *(cu.at("u_view")->getValue().value_mat4v);
		const Matrix4& proj = *(cu.at("u_proj")->getValue().value_mat4v);
		const Vec4& vp = *(cu.at("u_vp")->getValue().value_4f);
		Vec4 win1(vp[2]*0.5, vp[3]*0.5, 1.0, 1.0);
		Vec4 win2(vp[2]*0.5, vp[3]*0.5, 0.0, 0.0);
		
		Vec4 vertex = vert;
		Vec4 mvertex = vertex * model;
		Vec4 mvvertex = mvertex * view;
		Vec4 mvpvertex = mvvertex * proj;
		
		mvertex /= mvertex.w();
		mvvertex /= mvvertex.w();
		mvpvertex /= mvpvertex.w();
		
		mvpvertex *= win1; mvpvertex += win2;
		
		uniforms.set("v_vert",vert);
		uniforms.set("m_vert",mvertex);
		uniforms.set("mv_vert",mvvertex);
		uniforms.set("mvp_vert",mvpvertex);
		uniforms.set("col_vert",col);
		uniforms.set("normal_vert",normal);
		uniforms.set("tex_vert",tex);
    }
 };

// -----------------------------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	Vec3 observer(10.0, 0.0, 3.0);
     
    light_1.position = Vec3(2.0,1.0,1.0);
    light_1.color = Vec3(1.0,1.0,1.0);
    light_1.attn = 0.7;
	
	
	Image::Ptr targets[2];
    targets[0] = Image::create(INTERN_RES,INTERN_RES,4);
    targets[1] = Image::create(INTERN_RES,INTERN_RES,1);

	double a = 0.0;
	
	MyVertCallback::Ptr defaultVert = MyVertCallback::create();
	MyFragCallback::Ptr defaultFrag = MyFragCallback::create();
	LightFrag::Ptr lightFrag = LightFrag::create();
	TexFragCallback::Ptr texFrag = TexFragCallback::create();
    
	Geometry mushr = mushroom(Vec3(0.0), 1.0, 2*GEO_SUBDIV);
	Geometry torch = ball(Vec3(0.0), 0.2);
	Geometry testcube = neocube(1.0);
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
	georender.setTarget(0, targets[0], Vec4(100.0,100.0,255.0,255.0) );
	georender.setTarget(1, targets[1], Vec4(255.0,255.0,255.0,255.0) );
	georender.setProj( model );
	georender.setViewport( vp );
	
	Uniform::Ptr u_proj = Uniform::create("u_proj",Uniform::Type_Mat4v);
	Uniform::Ptr u_view = Uniform::create("u_view",Uniform::Type_Mat4v);
	Uniform::Ptr u_model = Uniform::create("u_model",Uniform::Type_Mat4v);
	Uniform::Ptr u_vp = Uniform::create("u_vp",Uniform::Type_4f);
	CnstUniforms uniforms;
	uniforms["u_proj"] = u_proj;
	uniforms["u_view"] = u_view;
	uniforms["u_model"] = u_model;
	uniforms["u_vp"] = u_vp;
	georender.setUniforms(uniforms);
	
	u_proj->set( &proj );
	u_vp->set( &vp );
	
	while (win.isOpen())
	{
		sf::Event ev;
		while (win.pollEvent(ev)) if (ev.type == sf::Event::Closed) win.close();
		
		a += 0.05; if(a > 6.28) a = 0.0;
		view = Matrix4::view(observer * Matrix3::rotationZ(a), Vec3(0.0), Vec3::Z);
		u_view->set( &view );
        georender.setView( view );
		
		georender.clearTargets();
		
		georender.setVertCallback( defaultVert );
		georender.setFragCallback( defaultFrag );
		
		model = Matrix4::translation(0.0, 0.0, -1.0);
		u_model->set( &model );
        georender.setModel( model );
		georender.render( plane );
		
		georender.setVertCallback( defaultVert );
		georender.setFragCallback( lightFrag );
		
		model = Matrix4::translation(light_1.position);
		u_model->set( &model );
		georender.setModel( model );
		georender.render( torch );
		
		georender.setVertCallback( defaultVert );
		georender.setFragCallback( texFrag );
		
		model = Matrix4();
		u_model->set( &model );
        georender.setModel( model );
		georender.render( mushr );
		
		/*georender.setVertCallback( defaultVert );
		georender.setFragCallback( texFrag );
		
		model = Matrix4::translation(Vec3(-2.0,-1.0,1.0));
		u_model->set( &model );
        georender.setModel( model );
		georender.render( testcube );*/
		
		texture.update(targets[0]->data());
		// ImageIO::save(targets[0],"output.tga"); // debug
		
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


