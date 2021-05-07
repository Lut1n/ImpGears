#include <ImpGears/Geometry/Geometry.h>
#include <ImpGears/SceneGraph/Camera.h>
#include <ImpGears/Graphics/Image.h>
#include <ImpGears/Descriptors/JsonImageOp.h>
#include <ImpGears/Renderer/GeometryRenderer.h>
#include <ImpGears/Renderer/CpuBlinnPhong.h>

#include <ImpGears/Core/Vec4.h>
#include <ImpGears/Core/Matrix3.h>
#include <ImpGears/Core/Matrix4.h>
#include <ImpGears/Core/Noise.h>
#include <fstream>

// #include <SFML/Graphics.hpp>

#include <ImpGears/Descriptors/FileInfo.h>
#include <ImpGears/Descriptors/ImageIO.h>
#include <ImpGears/Descriptors/JsonGeometryOp.h>

#include <ctime>
#include <vector>
#include <map>

using namespace imp;

// common stuff
#include "../utils/inc_experimental.h"

#define IMPLEMENT_APP_CONTEXT
#include "../utils/AppContext.h"


#define INTERN_RES 64

// -----------------------------------------------------------------------------------------------------------------------
struct LightFrag : public FragCallback
{
    Meta_Class(LightFrag)
	virtual void exec(ImageBuf& targets, const Vec3& pt, const UniformMap& uniforms, Varyings& varyings)
	{
		targets[0]->setPixel(pt[0],pt[1],Vec4(varyings.get("color"),1.0) * 255.0);
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
                sampler.setFiltering(ImageSampler::Filtering_Linear);
                sampler2.setFiltering(ImageSampler::Filtering_Linear);
		Vec3 col = sampler.get(uv);
		Vec3 col2 = sampler2.get(uv);
		
		col += col2; col *= 0.5;
		
		outColor = Vec4(col,1.0);
	};
};

// -----------------------------------------------------------------------------------------------------------------------
void loadSamplers(ImageSampler::Ptr& colorSampler, ImageSampler::Ptr& illuSampler, ImageSampler::Ptr& normalSampler)
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
	
	colorSampler =ImageSampler::create();
	colorSampler->setSource( ImageIO::load("./cache/scene_color.tga") );
        colorSampler->setWrapping(ImageSampler::Wrapping_Repeat);
        colorSampler->setFiltering(ImageSampler::Filtering_Linear);
	
	normalSampler =ImageSampler::create();
	normalSampler->setSource( ImageIO::load("./cache/scene_normals.tga") );
        normalSampler->setWrapping(ImageSampler::Wrapping_Repeat);
        normalSampler->setFiltering(ImageSampler::Filtering_Linear);
	
	illuSampler =ImageSampler::create();
	illuSampler->setSource( ImageIO::load("./cache/scene_illu.tga") );
        illuSampler->setWrapping(ImageSampler::Wrapping_Repeat);
        illuSampler->setFiltering(ImageSampler::Filtering_Linear);
}

// -----------------------------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	AppContext app;
	app.setArgs(1, argv);
	app.internalWidth = INTERN_RES;
	app.internalHeight = INTERN_RES;
	SceneRenderer::Ptr renderer = app.loadRenderer("CPU rendering");

	Vec3 observer(10.0, 0.0, 2.5);
     
    Vec3 lightPosition(4.0,2.0,3.0);
    Vec3 lightColor(0.3,1.0,0.3);
    Vec3 lightAttn(30.0,4.0,0.0);
	
	ImageSampler::Ptr sampler, nsampler, illusampler;
	loadSamplers(sampler,illusampler, nsampler);

	Image::Ptr rgbtarget = app.target;
	
	FragCallback::Ptr phongFrag = CpuBlinnPhong::create();
	LightFrag::Ptr lightFrag = LightFrag::create();
    
	std::map<std::string,Geometry> geo_load;
	imp::generateGeometryFromJson("geometries.json", geo_load);
	Geometry mushr = geo_load["mush_04"]; Geometry::intoCCW(mushr);
	Geometry torch = geo_load["lightball_02"]; Geometry::intoCCW(torch);
	Geometry plane = geo_load["ground_04"]; Geometry::intoCCW(plane);
	
	int frames = 0;

	Matrix4 model;
	Matrix4 proj = Matrix4::perspectiveProj(60.0, 1.0, 0.1, 10.0);
	Vec4 vp(0.0,0.0,INTERN_RES,INTERN_RES);
	Matrix4 view = Matrix4::view(observer, Vec3(0.0), Vec3::Z);
	
	GeometryRenderer georender;
	georender.setTarget(0, rgbtarget, Vec4(100.0,100.0,255.0,255.0) );
	georender.setViewport( vp );
	
	UniformMap uniforms;
	uniforms.set( Uniform::create("u_proj",Uniform::Type_Mat4) );
	uniforms.set( Uniform::create("u_view",Uniform::Type_Mat4) );
	uniforms.set( Uniform::create("u_model",Uniform::Type_Mat4) );
	uniforms.set( Uniform::create("u_sampler_color",Uniform::Type_Sampler) );
	uniforms.set( Uniform::create("u_sampler_normal",Uniform::Type_Sampler) );
	uniforms.set( Uniform::create("u_sampler_emissive",Uniform::Type_Sampler) );
	uniforms.set( Uniform::create("u_lightPos",Uniform::Type_3f) );
	uniforms.set( Uniform::create("u_lightCol",Uniform::Type_3f) );
	uniforms.set( Uniform::create("u_lightAtt",Uniform::Type_3f) );
	uniforms.set( Uniform::create("u_color",Uniform::Type_3f) );
	georender.setUniforms(uniforms);
	
	uniforms.set("u_proj", proj );
	uniforms.set("u_view", view );
	uniforms.set("u_sampler_color",sampler);
	uniforms.set("u_sampler_normal",nsampler);
	uniforms.set("u_lightCol",lightColor);
	uniforms.set("u_lightAtt",lightAttn);
	uniforms.set("u_color",Vec3(1.0));


	double a = 0.0;
	double lastElapsedTime = 0.0;

	while (app.begin())
	{
		a += 0.05; if(a > 6.28) a = 0.0;
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

		app.end();
		
		frames++;
		
		double delta = app.elapsedTime() - lastElapsedTime;
		if(delta > 1.0)
		{
			std::cout << "FPS : " << frames << std::endl;
			frames = 0;
			lastElapsedTime = app.elapsedTime();
		}
	}
   
    return 0;
}


