#include <SceneGraph/Graph.h>
#include <SceneGraph/Camera.h>
#include <SceneGraph/GeoNode.h>
#include <SceneGraph/QuadNode.h>
#include <Descriptors/ImageIO.h>
#include <Descriptors/FileInfo.h>
#include <Descriptors/JsonImageOp.h>

#include <Renderer/CpuRenderer.h>

#include <Plugins/RenderPlugin.h>

#include <SFML/Graphics.hpp>

using namespace imp;

// common stuff
#include "../common/inc_experimental.h"
#define IMPLEMENT_RENDER_MODE_MANAGER
#include "../common/RenderModeManager.h"

Geometry generateTerrain(const ImageSampler::Ptr& hm)
{
    imp::Geometry geometry = imp::Geometry::cube();
    geometry = geometry.subdivise(10);
    geometry.generateColors(Vec3(1.0));
    geometry.generateTexCoords(Geometry::TexGenMode_Cubic,4.0);
    geometry.scale(Vec3(1.0,1.0,0.01));

    for(int k=0;k<geometry.size();++k)
    {
        Vec3 uv = geometry._vertices[k];
        uv = (uv * 0.5)  + 0.5;
        Vec3 v = geometry._vertices[k];
        if(v[2]>0.0) v[2] += hm->get(uv)[0] * 3.0;
        geometry._vertices[k] = v;
    }
    geometry.scale(Vec3(5.0,5.0,1.0));
    geometry.generateNormals(Geometry::NormalGenMode_PerFace);
    geometry.interpolateNormals();

    return geometry;
}

// -----------------------------------------------------------------------------------------------------------------------
void loadSamplers(ImageSampler::Ptr& sampler, ImageSampler::Ptr& color)
{
    if( !fileExists("./cache/scene_terrain.tga") || !fileExists("./cache/scene_color.tga"))
    {
        std::filesystem::create_directories("cache");
        generateImageFromJson("textures.json");
    }

    sampler =ImageSampler::create();
    sampler->setSource( ImageIO::load("./cache/scene_terrain.tga") );
    sampler->setMode(ImageSampler::Mode_Repeat);
    sampler->setInterpo(ImageSampler::Interpo_Linear);

    color =ImageSampler::create();
    color->setSource( ImageIO::load("./cache/scene_color.tga") );
    color->setMode(ImageSampler::Mode_Repeat);
    color->setInterpo(ImageSampler::Interpo_Linear);
}

int main(int argc, char* argv[])
{
RenderModeManager renderModeMngr;
renderModeMngr.setArgs(argc, argv);

    ImageSampler::Ptr sampler, color, emi;
    loadSamplers(sampler,color);
    emi = ImageSampler::create(8, 8, 4, Vec4(0.3,0.3,1.0,1.0));

    sf::Clock clock;

    sf::RenderWindow window(sf::VideoMode(512, 512), "My window", sf::Style::Default, sf::ContextSettings(24));
    window.setFramerateLimit(60);

    Graph::Ptr graph = Graph::create();
    Node::Ptr root = ClearNode::create();

    Camera::Ptr camera = Camera::create();
    camera->setPosition(Vec3(10.0f, 10.0f, 10.0f));
    camera->setTarget(Vec3(0.0f, 0.0f, 0.0f));

    imp::ReflexionModel::Ptr r = imp::ReflexionModel::create(
            imp::ReflexionModel::Lighting_Phong,
            imp::ReflexionModel::Texturing_Samplers_CNE,
            imp::ReflexionModel::MRT_2_Col_Emi);

    Material::Ptr material = Material::create(Vec3(1.0,1.0,1.0), 1.0);
    material->_baseColor = color;

    Material::Ptr light_material = Material::create(Vec3(0.3,0.3,1.0), 1.0);
    light_material->_emissive = emi;

    Geometry::Ptr terrain = Geometry::create();
    *terrain = generateTerrain(sampler);
    GeoNode::Ptr geomush = GeoNode::create(terrain, false);
    geomush->setReflexion(r);
    geomush->setMaterial(material);

    LightNode::Ptr light = LightNode::create(Vec3(0.3,0.3,1.0),10.f);

    Geometry::Ptr pointGeo = Geometry::create();
    *pointGeo = Geometry::sphere(8, 0.4);
    pointGeo->setPrimitive(Geometry::Primitive_Triangles);
    pointGeo->generateColors(Vec3(0.3,0.3,1.0));
    pointGeo->generateNormals();
    pointGeo->generateTexCoords(Geometry::TexGenMode_Spheric);
    GeoNode::Ptr pointNode = GeoNode::create(pointGeo, false);
    pointNode->setPosition(Vec3(0.0,0.0,0.1));
    pointNode->setReflexion(r);
    pointNode->setMaterial(light_material);

    root->addNode(camera);
    root->addNode(geomush);
    root->addNode(light);
    root->addNode(pointNode);
    graph->setRoot(root);

    SceneRenderer::Ptr renderer = renderModeMngr.loadRenderer();
    graph->getInitState()->setViewport( renderModeMngr.viewport );

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed || event.type == sf::Event::KeyPressed)
                window.close();
        }


        if(clock.getElapsedTime().asSeconds() > 2.0*3.14) clock.restart();
        double t = clock.getElapsedTime().asMilliseconds() / 1000.0;

        Vec3 lp(cos(t)*5.0,sin(t)*4.0,5.0);
        light->setPosition(lp);
        pointNode->setPosition(lp);

        renderer->render( graph );
        // target = renderer->getTarget(true, 0);
        // ImageIO::save(target, "out.tga");

        renderModeMngr.draw(window);
        window.display();
        // break;
    }


    exit(EXIT_SUCCESS);
}
