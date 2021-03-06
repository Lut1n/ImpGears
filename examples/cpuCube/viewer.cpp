#include <Core/Matrix3.h>
#include <SceneGraph/Graph.h>
#include <SceneGraph/GeoNode.h>
#include <SceneGraph/Camera.h>
#include <SceneGraph/LightNode.h>
#include <Descriptors/ImageIO.h>

// #include <Renderer/GlRenderer.h>
#include <Renderer/CpuRenderer.h>
#include <Plugins/RenderPlugin.h>

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

    virtual Vec3 textureColor(const Vec2& uv, const UniformMap& uniforms, Varyings& varyings)
    {
        return Vec3(0.2,0.2,1.0);
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
        ImageSampler::Ptr sampler = uniforms.getSampler("u_sampler_color");
        return sampler->get(uv) * Vec3(1.0,0.0,0.0);
    }
};

#define GLSL_CODE( code ) #code

/// =========== FRAGMENT SHADER SOURCE =====================
static std::string glsl_texturing = GLSL_CODE(

uniform sampler2D u_sampler_color;

vec4 textureColor(vec2 uv)
{
    return vec4(0.2,0.2,1.0,1.0);
}

vec3 textureNormal(vec2 uv)
{
    return vec3(0.0,0.0,1.0);
}

vec4 textureEmissive(vec2 uv)
{
    // return vec3(0.0);
    return texture2D(u_sampler_color,uv).xyzw  * vec4(1.0,0.0,0.0,1.0);
}

);


struct IGStuff
{
    SceneRenderer::Ptr renderer;
    Graph::Ptr graph;
    Node::Ptr root;
    Camera::Ptr camera;
    LightNode::Ptr light;
    Image::Ptr target;

    Vec3 initCamPos;

    IGStuff(const std::string& arg)
    {
        Custom::Ptr customTexturing = Custom::create();
        Image::Ptr im = generateImage();
        ReflexionModel::Ptr model = ReflexionModel::create(ReflexionModel::Lighting_Phong, ReflexionModel::Texturing_Customized);
        model->_texturingCb = customTexturing;
        model->_fragCode_texturing = glsl_texturing;
        
        if(arg != "-gpu")
            renderer = CpuRenderer::create();
        else
        {
            std::string pluginName = "OGLPlugin";
            pluginName = "lib" + pluginName + "." + LIB_EXT;
            RenderPlugin::Ptr rp = PluginManager::open( pluginName ) ;
            renderer = rp->getRenderer();
            
        }

        target = Image::create(RES,RES,4);
        ImageSampler::Ptr sampler = ImageSampler::create(target);

        std::vector<ImageSampler::Ptr> samplers = {sampler};
        RenderTarget::Ptr rt = RenderTarget::create();
        rt->build(samplers, true);
        renderer->setTargets(rt);
        
        Vec4 viewport(0.0,0.0,512,512);
        graph = Graph::create();
        
        if(arg != "-gpu")
        {
            viewport.set(0.0,0.0,RES,RES);
            renderer->setDirect(false);
        }
        /*else
        {
            GlRenderer& g = dynamic_cast<GlRenderer&>( *renderer );
            g.loadRenderPlugin("libglPlugin");
        }*/
        
        graph->getInitState()->setViewport( viewport );

        // Geometry cubeGeo = Geometry::sphere(8,1.0);
        Geometry::Ptr cubeGeo = Geometry::create();
        *cubeGeo = Geometry::cube();
        cubeGeo->generateColors(Vec3(1.0,1.0,1.0));
        cubeGeo->generateNormals();
        cubeGeo->generateTexCoords(Geometry::TexGenMode_Cubic, 1.0);
        Geometry::intoCCW( *cubeGeo );
        
        Material::Ptr material = Material::create(Vec3(1.0),4.0);
        material->_baseColor = ImageSampler::create(im,ImageSampler::Wrapping_Repeat);
        
        GeoNode::Ptr cubeNode = GeoNode::create(cubeGeo);
        cubeNode->setReflexion(model);
        cubeNode->setMaterial(material);
        
        initCamPos.set(5,0,5);
        camera = Camera::create();
        camera->setPosition( initCamPos );
        
        light = LightNode::create(Vec3(1.0), 30.0);
        light->setPosition(Vec3(10.0,5.0,10.0));
        
        root = Node::create();
        root->addNode(camera);
        root->addNode(cubeNode);
        root->addNode(light);
        graph->setRoot(root);
        
    };

    void updateCamera(float angle)
    {
        camera->setPosition( initCamPos * Matrix3::rotationXYZ(Vec3(angle)) );
        // light->setPosition( Vec3(10.0,5.0,10.0) * Matrix3::rotationZ(angle) );
    }

    void render()
    {
        renderer->render( graph );
    }
};


// -----------------------------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    int sfDepthBufferBits = 24;
    sf::RenderWindow window(sf::VideoMode(512, 512), "Example cube", sf::Style::Default, sf::ContextSettings(sfDepthBufferBits));
    sf::Texture texture; texture.create(512, 512); // texture.setSmooth(true);
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
            texture.update(engine.target->data(),engine.target->width(),engine.target->height(),0,0);
            sprite.setScale( 512.0 / engine.target->width(), -512.0 / engine.target->height() );
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


