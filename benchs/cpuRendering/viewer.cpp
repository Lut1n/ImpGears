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
#include <Utils/JsonGeometryOp.h>

#include <ctime>
#include <vector>
#include <map>
#include <filesystem>

using namespace imp;

#define INTERN_RES 128

struct Phong : public FragCallback
{
	Meta_Class(Phong)
	void exec(ImageBuf& targets, const Vec3& pt, const UniformMap& uniforms, Varyings& varyings)
	{
		const Matrix4& view = uniforms.getMat4("u_view");
		const Vec3& lightPos = uniforms.getVec3("u_lightPos");
		const Vec3& lightCol = uniforms.getVec3("u_lightCol");
		const Vec3& lightAtt = uniforms.getVec3("u_lightAtt");
		float lightPower = lightAtt[0];
		float shininess = lightAtt[1];
		
		
		TextureSampler::Ptr normal_spl = uniforms.getSampler("u_sampler_normal");
		TextureSampler::Ptr color_spl = uniforms.getSampler("u_sampler_color");
		TextureSampler::Ptr illu_spl = uniforms.getSampler("u_sampler_illu");
		
		Vec3 tex = varyings.get("texUV");
		Vec3 color = varyings.get("color");
		Vec3 frag_pos = varyings.get("m_vert");
		Vec3 normal = varyings.get("normal");
		Vec3 light_dir = lightPos - frag_pos;
		float distance = light_dir.length();
		distance = distance * distance;
		light_dir.normalize();
		
		// TBN Matrix for computation in tangent space
		Vec3 n_z = normal;
		Vec3 n_x = Vec3::X;
		Vec3 n_y = n_z.cross(n_x); n_y.normalize();
		n_x = n_y.cross(n_z); n_x.normalize();
		Matrix3 tbn(n_x,n_y,n_z); tbn = tbn.inverse();
		
		if(normal_spl)
		{
			Vec4 frag_n = normal_spl->get(tex);
			normal = Vec3(frag_n[0],frag_n[1],frag_n[2]) * 2.0 - 1.0;
		}
		
		float lambertian = std::max(light_dir.dot(normal),0.f);
		float specular = 0.0;
		
		if(lambertian > 0.0)
		{
			
			// view dir
			Vec3 view_pos = Vec3(view(3,0),view(3,1),view(3,2)) * tbn;
			Vec3 view_dir = view_pos - frag_pos;
			view_dir.normalize();
			
			// blinn phong
			light_dir = light_dir * tbn; 
			Vec3 halfDir = light_dir + view_dir;
			float specAngle = std::max( halfDir.dot(normal), 0.f );
			specular = std::pow(specAngle, shininess);
		}
		
		// Color model :
		// ambiantColor
		// + diffColor * lambertian * lightColor * lightPower / distance
		// + specColor * specular * lightColor * lightPower / distance
		
		if(color_spl)
		{
			Vec4 frag_c = color_spl->get(tex);
			color *= Vec3(frag_c[0],frag_c[1],frag_c[2]);
		}
		
		Vec3 colModelRes = color*0.01
		+ color*0.7 * lambertian * lightCol * lightPower / distance
		+ color*0.3 * specular * lightCol * lightPower / distance;

		if(illu_spl)
		{
			Vec3 illu = illu_spl->get(tex);
			colModelRes = dotMax(illu,colModelRes);
		}
		
		Vec4 px(colModelRes,1.0); px = dotClamp( px );
		targets[0]->setPixel(pt[0], pt[1], px * 255.0 );
	}
};

// -----------------------------------------------------------------------------------------------------------------------
struct LightFrag : public FragCallback
{
    Meta_Class(LightFrag)
	virtual void exec(ImageBuf& targets, const Vec3& pt, const UniformMap& uniforms, Varyings& varyings)
	{
		targets[0]->setPixel(pt[0],pt[1],Vec4(varyings.get("color"),1.0) * 255.0);
		// targets[1]->setPixel(pt[0],pt[1],Vec4(varyings.get("color"),1.0) * 255.0);
	}
};

struct IlluOp : public ImageOperation
{
	Image::Ptr src;
	virtual void apply(const Vec2& uv, Vec4& outColor)
	{
		ImageSampler sampler(src);
		Vec4 col = sampler.get(uv);
		
		float l = imp::max3(col[0],col[1],col[2]);
		
		l = 1.f - linearstep(0.3f, 1.0f, l);
		l *= 2.0;
		l = imp::pow( 0.4+l, 16 );
		outColor = Vec4(l,l,l,1.0);
	};
};


struct BlurOp : public ImageOperation
{
	Image::Ptr src;
	Matrix<9,9,float> mat9;
	Vec2 dims;
	
	BlurOp(Image::Ptr src)
		: src(src)
	{
		std::vector<float> d(81,0.012345679);
		mat9 = Matrix<9,9,float>(d.data());
		dims = Vec2(src->width(),src->height());
	}
	
	virtual void apply(const Vec2& uv, Vec4& outColor)
	{
		int D = 9; int DD = D/2;
		ImageSampler sampler(src);
		
		Vec4 acc;
		for(int i=0;i<D;++i) for(int j=0;j<D;++j)
		{
			Vec2 uv2 = uv + Vec2(i-DD,j-DD) / dims;
			
			acc += sampler.get(uv2) * mat9(i,j) * 10.0;
		}
		
		acc = dotClamp(acc);
		acc[3] = 1.0;
		
		outColor = acc;
	};
};

struct CopyOp : public ImageOperation
{
	Image::Ptr src,src2;
	
	virtual void apply(const Vec2& uv, Vec4& outColor)
	{
		ImageSampler sampler(src), sampler2(src2);
		sampler.setInterpo(TextureSampler::Interpo_Linear);
		sampler2.setInterpo(TextureSampler::Interpo_Linear);
		Vec3 col = sampler.get(uv);
		Vec3 col2 = sampler2.get(uv);
		
		col += col2; col *= 0.5;
		
		outColor = Vec4(col,1.0);
	};
};

// -----------------------------------------------------------------------------------------------------------------------
void loadSamplers(TextureSampler::Ptr& colorSampler, TextureSampler::Ptr& illuSampler, TextureSampler::Ptr& normalSampler)
{
	if( !fileExists("./cache/scene_color.tga") || !fileExists("./cache/scene_normals.tga") )
	{
		// c++17
		std::filesystem::create_directories("cache");
		generateImageFromJson("textures.json");
		
		IlluOp op;
		op.src = ImageIO::load("./cache/scene_color.tga");
		Image::Ptr dst = Image::create(128,128,3);
		op.execute(dst);
		ImageIO::save(dst,"./cache/scene_illu.tga");
		
	}
	
	colorSampler =TextureSampler::create();
	colorSampler->setSource( ImageIO::load("./cache/scene_color.tga") );
	colorSampler->setMode(TextureSampler::Mode_Repeat);
	colorSampler->setInterpo(TextureSampler::Interpo_Linear);
	
	normalSampler =TextureSampler::create();
	normalSampler->setSource( ImageIO::load("./cache/scene_normals.tga") );
	normalSampler->setMode(TextureSampler::Mode_Repeat);
	normalSampler->setInterpo(TextureSampler::Interpo_Linear);
	
	illuSampler =TextureSampler::create();
	illuSampler->setSource( ImageIO::load("./cache/scene_illu.tga") );
	illuSampler->setMode(TextureSampler::Mode_Repeat);
	illuSampler->setInterpo(TextureSampler::Interpo_Linear);
}

// -----------------------------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	Vec3 observer(10.0, 0.0, 2.5);
	// Vec3 observer(20.0, 0.0, 4.5);
     
    Vec3 lightPosition(4.0,2.0,3.0);
    Vec3 lightColor(0.3,1.0,0.3);
    Vec3 lightAttn(30.0,4.0,0.0);
	
	TextureSampler::Ptr sampler, nsampler, illusampler;
	loadSamplers(sampler,illusampler, nsampler);
	Image::Ptr rgbtarget = Image::create(INTERN_RES,INTERN_RES,4);
	
	double a = 0.0;
	
	FragCallback::Ptr phongFrag = Phong::create();
	LightFrag::Ptr lightFrag = LightFrag::create();
    
	std::map<std::string,Geometry> geo_load;
	imp::generateGeometryFromJson("geometries.json", geo_load);
	Geometry mushr = geo_load["mush_04"]; Geometry::intoCCW(mushr);
	Geometry torch = geo_load["lightball_02"]; Geometry::intoCCW(torch);
	Geometry plane = geo_load["ground_04"]; Geometry::intoCCW(plane);
	
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
	// georender.setTarget(1, rgbtarget2, Vec4(0.0,0.0,0.0,255.0) );
	georender.setViewport( vp );
	
	UniformMap uniforms;
	uniforms.set( Uniform::create("u_proj",Uniform::Type_Mat4) );
	uniforms.set( Uniform::create("u_view",Uniform::Type_Mat4) );
	uniforms.set( Uniform::create("u_model",Uniform::Type_Mat4) );
	uniforms.set( Uniform::create("u_sampler_color",Uniform::Type_Sampler) );
	uniforms.set( Uniform::create("u_sampler_normal",Uniform::Type_Sampler) );
	uniforms.set( Uniform::create("u_sampler_illu",Uniform::Type_Sampler) );
	uniforms.set( Uniform::create("u_lightPos",Uniform::Type_3f) );
	uniforms.set( Uniform::create("u_lightCol",Uniform::Type_3f) );
	uniforms.set( Uniform::create("u_lightAtt",Uniform::Type_3f) );
	georender.setUniforms(uniforms);
	
	uniforms.set("u_proj", proj );
	uniforms.set("u_view", view );
	uniforms.set("u_sampler_color",sampler);
	uniforms.set("u_sampler_normal",nsampler);
	// uniforms.set("u_sampler_illu",illusampler);
	uniforms.set("u_lightCol",lightColor);
	uniforms.set("u_lightAtt",lightAttn);
	
	while (win.isOpen())
	{
		sf::Event ev;
		while (win.pollEvent(ev)) if (ev.type == sf::Event::Closed) win.close();
		
		a += 0.05; if(a > 6.28) a = 0.0;
		// view = Matrix4::view(observer * Matrix3::rotationZ(a*2.0), Vec3(0.0), Vec3::Z);
		Vec3 lightPos = lightPosition * Matrix3::rotationZ(a*2.0);
		uniforms.set("u_view", view );
		uniforms.set("u_lightPos", lightPos);
		
		georender.clearTargets();
		
		georender.setFragCallback( phongFrag );
		uniforms.set("u_model", Matrix4::translation(0.0, 0.0, -1.0) );
		georender.render(plane);
		
		georender.setFragCallback( lightFrag );
		
		uniforms.set("u_model", Matrix4::translation(lightPos) );
		georender.render(torch);
		
		georender.setFragCallback( phongFrag );
		
		uniforms.set("u_model", Matrix4() );
		georender.render(mushr);
		
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


