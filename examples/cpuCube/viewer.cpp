#include <Core/Matrix3.h>
#include <SceneGraph/Graph.h>
#include <SceneGraph/GeoNode.h>
#include <Descriptors/ImageIO.h>

#include <SFML/Graphics.hpp>

#include <ctime>

#define RES 512

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
    
    Vec4 bgcol(0,128,200,255);
    Vec4 fgcol(255,128,200,255);
	
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
	
    imp::ImageIO::save(image,"out.tga");
    
    return image;
}

struct Custom : public ReflexionModel::TexturingCallback
{
	Meta_Class(Custom)
	
	Image::Ptr im;
	
	virtual Vec3 textureColor(const Vec2& uv, const UniformMap& uniforms, Varyings& varyings)
	{
		return Vec3(1.0);
	}
	
	virtual Vec3 textureNormal(const Vec2& uv, const UniformMap& uniforms, Varyings& varyings)
	{
		return Vec3(0.0,0.0,1.0);
		/*ImageSampler sampler(im);
		return sampler.get(uv) * 2.0 - 1.0;*/
	}
	
	virtual Vec3 textureEmissive(const Vec2& uv, const UniformMap& uniforms, Varyings& varyings)
	{
		// return Vec3(0.0);
		ImageSampler sampler(im);
		return sampler.get(uv) * Vec3(1.0,0.0,0.0);
	}
};

#define GLSL_CODE( code ) #code

/// =========== FRAGMENT SHADER SOURCE =====================
static std::string glsl_texturing = GLSL_CODE(

uniform sampler2D u_sampler_test;

vec3 textureColor(vec2 uv)
{
	return vec3(1.0);
}

vec3 textureNormal(vec2 uv)
{
	return vec3(0.0,0.0,1.0);
}

vec3 textureEmissive(vec2 uv)
{
	// return vec3(0.0);
	return texture2D(u_sampler_test,uv).xyz  * vec3(1.0,0.0,0.0);
}

);


struct IGStuff
{
	Graph::Ptr renderer;
	Node::Ptr graphRoot;
	Camera::Ptr camera;
	Target::Ptr target;
	
	Vec4 viewport;
	Vec3 initCamPos;
	
	IGStuff(const std::string& arg)
	{
		Custom::Ptr customTexturing = Custom::create();
		customTexturing->im = generateImage();
		ReflexionModel::Ptr model = ReflexionModel::create(ReflexionModel::Lighting_Phong, ReflexionModel::Texturing_Customized);
		model->_texturingCb = customTexturing;
		model->_fragCode_texturing = glsl_texturing;
		
		TextureSampler::Ptr sampler_test = TextureSampler::create();
		sampler_test->setSource( customTexturing->im );
		sampler_test->setMode(ImageSampler::Mode_Repeat);
		
		renderer = Graph::create();
		target = Target::create();
		
		viewport.set(0.0,0.0,512,512);
		
		if(arg != "-gpu")
		{
			viewport.set(0.0,0.0,RES,RES);
			target->create(RES,RES,1,true);
			renderer->setTarget(target);
		}
		
		renderer->getInitState()->setViewport( viewport );
	
		// Geometry cubeGeo = Geometry::sphere(8,1.0);
		Geometry cubeGeo = Geometry::cube();
		cubeGeo.generateColors(Vec3(1.0,1.0,1.0));
		cubeGeo.generateNormals();
		cubeGeo.generateTexCoords(1.0);
		Geometry::intoCCW( cubeGeo );
		
		GeoNode::Ptr cubeNode = GeoNode::create(cubeGeo);
		cubeNode->setShader(model);
		cubeNode->setColor(Vec3(1.0));
		
		initCamPos.set(10,0,3);
		
		Uniform::Ptr u_lightPos = Uniform::create("u_lightPos", Uniform::Type_3f);
		Uniform::Ptr u_lightCol = Uniform::create("u_lightCol", Uniform::Type_3f);
		Uniform::Ptr u_lightAtt = Uniform::create("u_lightAtt", Uniform::Type_3f);
		Uniform::Ptr u_sampler_test = Uniform::create("u_sampler_test", Uniform::Type_Sampler);
		// Uniform::Ptr u_sampler_color = Uniform::create("u_sampler_color", Uniform::Type_Sampler);
		// Uniform::Ptr u_sampler_normal = Uniform::create("u_sampler_normal", Uniform::Type_Sampler);
		u_lightPos->set(Vec3(10.0,5.0,5.0));
		u_lightCol->set(Vec3(1.0));
		u_lightAtt->set(Vec3(30.0,1.0,0.0));
		u_sampler_test->set(sampler_test);
		// u_sampler_color->set(sampler_test);
		// u_sampler_normal->set(sampler_test);
		cubeNode->getState()->setUniform(u_lightPos);
		cubeNode->getState()->setUniform(u_lightCol);
		cubeNode->getState()->setUniform(u_lightAtt);
		cubeNode->getState()->setUniform(u_sampler_test);
		// cubeNode->getState()->setUniform(u_sampler_color);
		// cubeNode->getState()->setUniform(u_sampler_normal);
		
		camera = Camera::create();
		camera->setPosition( initCamPos );
		
		graphRoot = Node::create();
		graphRoot->addNode(camera);
		graphRoot->addNode(cubeNode);
	};
	
	void updateCamera(float angle)
	{
		camera->setPosition( initCamPos * Matrix3::rotationZ(angle) );
	}
	
	void render()
	{
		renderer->renderScene( graphRoot );
	}
};


// -----------------------------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	int sfDepthBufferBits = 24;
	sf::RenderWindow window(sf::VideoMode(512, 512), "Example cube", sf::Style::Default, sf::ContextSettings(sfDepthBufferBits));
	sf::Texture texture; texture.create(512, 512);
	sf::Sprite sprite(texture);
	
	std::string engine_arg = ""; if(argc > 1) engine_arg = argv[1];
	IGStuff engine(engine_arg);
	
	float target_fps = 60.0, current_fps = 60.0, acc = 1.0;
    double a = 0.0;
	
	int frames = 0;
	sf::Clock c;
	
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed) window.close();
		};

		window.clear();
		
		a += 0.02 * acc; if(a > 2.0*3.141592) a = 0.0;
		engine.updateCamera(a);
		
		engine.render();

		if(engine_arg != "-gpu") 
		{
			Image::Ptr res = engine.target->get(0)->getSource();
			texture.update(res->data(),res->width(),res->height(),0,0);
			sprite.setScale( 512.0 / res->width(), -512.0 / res->height() );
			sprite.setPosition( 0, 512 );
			window.draw(sprite);
		}
		window.display();
		
		frames++;
		
		if(c.getElapsedTime().asMilliseconds() > 1000.0)
		{
			current_fps = frames;
			acc = target_fps / std::max(current_fps,1.f);
			std::cout << "FPS : " << std::endl;
			std::cout << "update : " << current_fps*acc << std::endl;
			std::cout << "render : " << current_fps << std::endl;
			frames = 0;
			c.restart();
		}
	}
   
    return 0;
}


