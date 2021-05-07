#include <ImpGears/Core/Matrix3.h>
#include <ImpGears/SceneGraph/Graph.h>
#include <ImpGears/SceneGraph/GeoNode.h>
#include <ImpGears/SceneGraph/Camera.h>
#include <ImpGears/SceneGraph/LightNode.h>
#include <ImpGears/Descriptors/ImageIO.h>

// #include <ImpGears/Renderer/GlRenderer.h>
#include <ImpGears/Renderer/CpuRenderer.h>
#include <ImpGears/Plugins/RenderPlugin.h>

#define IMPLEMENT_APP_CONTEXT
#include "../utils/AppContext.h"

#include <ctime>

#define RES 200

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
        return Vec3(sampler->get(uv)) * Vec3(1.0,0.0,0.0);
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

float textureReflectivity(vec2 uv) { return 0.0; }

);


struct IGStuff
{
    AppContext app;
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

        app.offscreen = (arg != "-gpu");
        renderer = app.loadRenderer("Example Cube");
        target = app.target;

        renderer->enableFeature(SceneRenderer::Feature_Shadow, false);
        renderer->enableFeature(SceneRenderer::Feature_Environment, false);
        renderer->enableFeature(SceneRenderer::Feature_Bloom, false);
        renderer->enableFeature(SceneRenderer::Feature_SSAO, false);
        renderer->enableFeature(SceneRenderer::Feature_Phong, true);
        renderer->setOutputFrame(SceneRenderer::RenderFrame_Default);
        // renderer->setDirect(false);
        
        graph = Graph::create();
        graph->getInitState()->setViewport( app.viewport );

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
    std::string engine_arg = ""; if(argc > 1) engine_arg = argv[1];
    IGStuff engine(engine_arg);

    float target_fps = 60.0, current_fps = 60.0, acc = 1.0;
    double a = 0.0;

    int frames = 0;
    double lastElapsed = 0.0;

    while (engine.app.begin())
    {
        a += 0.02 * acc; if(a > 2.0*3.141592) a = 0.0;
        engine.updateCamera(a);
        
        engine.render();

        engine.app.end();

        frames++;
        
        if(engine.app.elapsedTime() - lastElapsed > 1.0)
        {
            current_fps = frames;
            acc = target_fps / (current_fps>1.f ? current_fps:1.f);
            std::cout << "FPS : " << std::endl;
            std::cout << "update : " << current_fps*acc << std::endl;
            std::cout << "render : " << current_fps << std::endl;
            frames = 0;
            lastElapsed = engine.app.elapsedTime();
        }
    }

    return 0;
}


