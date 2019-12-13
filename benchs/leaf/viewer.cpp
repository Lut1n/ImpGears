#include <SceneGraph/Graph.h>
#include <SceneGraph/Camera.h>
#include <SceneGraph/GeoNode.h>
#include <SceneGraph/QuadNode.h>
#include <Descriptors/ImageIO.h>
#include <Descriptors/FileInfo.h>
#include <Descriptors/JsonImageOp.h>

#include <SFML/Graphics.hpp>

using namespace imp;

// common stuff
#include "../common/inc_experimental.h"
#define IMPLEMENT_RENDER_MODE_MANAGER
#include "../common/RenderModeManager.h"
#define IMPLEMENT_BASIC_GEOMETRIES
#include "../common/basic_geometries.h"
#include "../common/basic_skybox.h"

#define IMPLEMENT_LEAF_N_COORDS
#include "mGeometry.h"




int main(int argc, char* argv[])
{
    RenderModeManager renderModeMngr;
    renderModeMngr.setArgs(argc, argv);

    sf::Clock clock;

    sf::RenderWindow window(sf::VideoMode(512, 512), "Leaf generator", sf::Style::Default, sf::ContextSettings(24));
    window.setFramerateLimit(60);

    Graph::Ptr graph = Graph::create();
    ClearNode::Ptr cn = ClearNode::create();
    cn->setColor(Vec4(0.2,0.3,255.0,255.0));
    cn->enableColor(false);
    Node::Ptr root = cn;

    Camera::Ptr camera = Camera::create();
    LightNode::Ptr light = LightNode::create(Vec3(1.0),5.f);
    light->setPosition(Vec3(1.0,2.0,1.0));

    imp::ReflexionModel::Ptr reflexion = imp::ReflexionModel::create(
        imp::ReflexionModel::Lighting_Phong,
        imp::ReflexionModel::Texturing_Samplers_CNE,
        imp::ReflexionModel::MRT_2_Col_Emi);

    Material::Ptr material = Material::create(Vec3(1.0), 1.0);
    // material->_emissive = ImageSampler::create(8,8,3, Vec4(0.0,1.0,0.0,1.0));
    ImageSampler::Ptr emi_sampler = ImageSampler::create(8,8,3,Vec4(1.0));
    Material::Ptr light_material = Material::create(Vec3(1.0), 1.0);
    light_material->_emissive = emi_sampler;

    Geometry::Ptr pointGeo = Geometry::create();
    *pointGeo = Geometry::sphere(4, 0.1);
    pointGeo->setPrimitive(Geometry::Primitive_Triangles);
    pointGeo->generateColors(Vec3(0.5,0.5,1.0));
    pointGeo->generateNormals();
    pointGeo->generateTexCoords();
    GeoNode::Ptr pointNode = GeoNode::create(pointGeo, false);
    pointNode->setPosition(Vec3(0.0,0.2,0.0));
    pointNode->setReflexion(reflexion);
    pointNode->setMaterial(light_material);

    Geometry::Ptr coordGeo = buildCoord();
    GeoNode::Ptr coordNode = GeoNode::create(coordGeo, false);
    coordNode->setPosition(Vec3(0.0,0.0,0.0));
    coordNode->setReflexion(reflexion);
    coordNode->setScale(Vec3(2.0));
    coordNode->setMaterial(material);

    Geometry::Ptr leafGeo = generateLeaf();
    GeoNode::Ptr leafNode = GeoNode::create(leafGeo, false);
    leafNode->setScale(Vec3(0.3));
    leafNode->setReflexion(reflexion);
    leafNode->setMaterial(material);

    Node::Ptr vegetal = Node::create();
    vegetal->addNode(leafNode);
    vegetal->addNode(pointNode);
    vegetal->setPosition(Vec3(0.0,0.3,0.0));

    std::vector<std::string> box_filenames =
    {
        "./testbox/testbox_right.tga",
        "./testbox/testbox_left.tga",
        "./testbox/testbox_top.tga",
        "./testbox/testbox_bottom.tga",
        "./testbox/testbox_front.tga",
        "./testbox/testbox_back.tga",
    };
    SkyBox::Ptr sky = SkyBox::create(  );

    // setup scene
    root->addNode(camera);
    root->addNode(vegetal);
    root->addNode(coordNode);
    root->addNode(light);
    root->addNode(sky);
    graph->setRoot(root);

    RenderPass::Ptr rp_info = RenderPass::create();
    rp_info->enablePass(RenderPass::Pass_EnvironmentMapping);
    coordNode->setRenderPass(rp_info);
    sky->setRenderPass(rp_info);
    pointNode->setRenderPass(rp_info);

    SceneRenderer::Ptr renderer = renderModeMngr.loadRenderer();
    graph->getInitState()->setViewport( renderModeMngr.viewport );
    // graph->getInitState()->setPerspectiveProjection(90.0, 1.0, 0.1, 1024.0);

    renderer->enableFeature(SceneRenderer::Feature_Shadow, true);
    renderer->enableFeature(SceneRenderer::Feature_Environment, true);
    // renderer->setDirect(false);
    // renderer->setOutputFrame(SceneRenderer::RenderFrame_Emissive);
    // renderer->enableFeature(SceneRenderer::Feature_Bloom, false);
    // renderer->setOutputFrame(SceneRenderer::RenderFrame_ShadowMap);

    renderer->setOutputFrame(SceneRenderer::RenderFrame_Default);

    // int frame_index = 0;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed || event.type == sf::Event::KeyPressed)
                window.close();
        }

        if( !window.isOpen() ) break;

        double t = clock.getElapsedTime().asSeconds();

        // double duration = 3.0;
        // int frame_count = 7;
        // frame_index = int(t/duration) % frame_count;
        // if(frame_index == 0)
        //     renderer->setOutputFrame(SceneRenderer::RenderFrame_Lighting);
        // else if(frame_index == 1)
        //     renderer->setOutputFrame(SceneRenderer::RenderFrame_Emissive);
        // else if(frame_index == 2)
        //     renderer->setOutputFrame(SceneRenderer::RenderFrame_Normals);
        // else if(frame_index == 3)
        //     renderer->setOutputFrame(SceneRenderer::RenderFrame_ShadowMap);
        // else if(frame_index == 4)
        //     renderer->setOutputFrame(SceneRenderer::RenderFrame_Depth);
        // else if(frame_index == 5)
        //     renderer->setOutputFrame(SceneRenderer::RenderFrame_Bloom);
        // else if(frame_index == 6)
        //     renderer->setOutputFrame(SceneRenderer::RenderFrame_Default);

        
        camera->setPosition(Vec3(cos(t*0.2)*1.0,0.75,sin(t*.2)*1.0));
        camera->setTarget(Vec3(0.0));
        Vec3 lp(cos(t)*4.0,1.0,sin(t)*3.0);

        window.clear();

        // rendering
        renderer->render( graph );

        renderModeMngr.draw(window);
        window.display();
    }

    exit(EXIT_SUCCESS);
}
