#include <ImpGears/Graphics/Uniform.h>
#include <ImpGears/Renderer/SceneRenderer.h>
#include <ImpGears/SceneGraph/Graph.h>
#include <ImpGears/SceneGraph/Camera.h>
#include <ImpGears/SceneGraph/Material.h>
#include <ImpGears/SceneGraph/QuadNode.h>
#include <ImpGears/Descriptors/ImageIO.h>
#include <ImpGears/Descriptors/FileInfo.h>
#include <ImpGears/Plugins/RenderPlugin.h>

#include <fstream>

#define IMPLEMENT_APP_CONTEXT
#include "../utils/AppContext.h"

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

    Vec4 bgcol(0,0,0,255);
    Vec4 fgcol(255,255,0,255);

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

    ImageIO::save(image,"out.tga");

    return image;
}

std::vector<std::string> getarg(int argc, char* argv[])
{
    std::vector<std::string> result;
    result.resize(argc);
    for(int i=0;i<argc;++i) result[i] = std::string(argv[i]);
    return result;
}

int main(int argc, char* argv[])
{
    char default_shader[] = "./defaultshader.frag";
    std::vector<std::string> arg = getarg(argc,argv);
    if(arg.size() < 2) arg.push_back(default_shader);

    AppContext app;
    app.offscreen = false;
    SceneRenderer::Ptr renderer = app.loadRenderer("Example Cube");

    long accessLast, modifLast, statusLast;
    getTimeInfo(default_shader, accessLast, modifLast, statusLast);

    Graph::Ptr graph = Graph::create();
    graph->setClearColor(Vec4(1.0,0.0,0.0,1.0));
    State::Ptr state = graph->getInitState();
    state->setOrthographicProjection(-1.f, 1.f, -1.f, 1.f, 0.f, 1.f);
    state->setViewport(0.0, 0.0, 1024.0, 1024.0);


    // Target::Ptr target = Target::create();
    // target->create(512, 512,1);
    // state->setTarget(target);


    std::cout << "defaultshader.vert + " << arg[1] << std::endl;
    std::string c_vert = loadText("./defaultshader.vert");
    std::string c_frag = loadText(arg[1]);
    ReflexionModel::Ptr shader = ReflexionModel::create(ReflexionModel::Lighting_Phong, ReflexionModel::Texturing_Customized);
    // shader->_vertCode = c_vert;
    shader->_fragCode_texturing = c_frag;


    QuadNode::Ptr screen = QuadNode::create();
    screen->setReflexion(shader);
    state = screen->getState();

    Image::Ptr image = generateImage();
    Material::Ptr material = Material::create(Vec3(1.0),8.0);
    material->_baseColor = ImageSampler::create(image);

    LightNode::Ptr light = LightNode::create(Vec3(1.0),3.0);

    screen->setMaterial(material);
    screen->addNode(light);

    Node::Ptr root = screen;
    graph->setRoot(root);

    bool breakLoop = false;
    while (app.begin())
    {
        // reload if changed
        long access, modif, status;
        getTimeInfo(default_shader, access, modif, status);
        if(modif != modifLast)
        {
            std::cout << "[reload shaders] defaultshader.vert + " << arg[1] << std::endl;
            modifLast = modif;
            c_frag = loadText(arg[1]);
            shader->_d = -1;
            // shader->vertCode = c_vert;
            shader->_fragCode_texturing = c_frag;
            screen->getState()->setReflexion(shader);
        }
        
        float sec = app.elapsedTime();
        state->setUniform("u_timer", sec);
        light->setPosition( Vec3(std::sin(sec)*2.0,0.0,1.0) );
        
        renderer->render( graph );

        app.end();
    }

    return EXIT_SUCCESS;
}
