#include <SceneGraph/Graph.h>
#include <SceneGraph/Camera.h>
#include <SceneGraph/GeoNode.h>
#include <SceneGraph/QuadNode.h>
#include <SceneGraph/RenderPass.h>
#include <Descriptors/ImageIO.h>
#include <Descriptors/FileInfo.h>
#include <Descriptors/JsonImageOp.h>

#include <Renderer/CpuRenderer.h>

#include <Plugins/RenderPlugin.h>

#include <Graphics/ImageOperation.h>

#include <SFML/Graphics.hpp>

using namespace imp;

// common stuff
#include "../common/inc_experimental.h"
#define IMPLEMENT_RENDER_MODE_MANAGER
#include "../common/RenderModeManager.h"
#define IMPLEMENT_BASIC_GEOMETRIES
#include "../common/basic_geometries.h"
#include "../common/basic_skybox.h"

Geometry generateTerrain(const ImageSampler::Ptr& hm)
{
    imp::Geometry geometry = imp::Geometry::cube();
    geometry = geometry.subdivise(10);
    Geometry::intoCCW(geometry);
    geometry.generateColors(Vec3(1.0));
    geometry.generateTexCoords(Geometry::TexGenMode_Cubic,1.0);

    for(int k=0;k<geometry.size();++k)
    {
        Vec3 uv = geometry._vertices[k];
        uv = Vec3(uv[0],uv[2],1.0);
        uv = (uv * 0.5)  + 0.5;
        Vec3 v = geometry._vertices[k];
        if(v[1]>0.0) v[1] = hm->get(uv)[0] * 5.0;
        geometry._vertices[k] = v;
    }
    geometry.scale(Vec3(4.0,1.0,4.0));
    geometry.generateNormals(Geometry::NormalGenMode_PerFace);
    // geometry.interpolateNormals();

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
    sampler->setWrapping(ImageSampler::Wrapping_Repeat);
    sampler->setFiltering(ImageSampler::Filtering_Linear);

    color =ImageSampler::create();
    color->setSource( ImageIO::load("./cache/scene_color.tga") );
    color->setWrapping(ImageSampler::Wrapping_Repeat);
    color->setFiltering(ImageSampler::Filtering_Linear);
}

int main(int argc, char* argv[])
{
RenderModeManager renderModeMngr;
renderModeMngr.setArgs(argc, argv);

    ImageSampler::Ptr sampler, color, emi, normals;
    loadSamplers(sampler,color);
    emi = ImageSampler::create(8, 8, 4, Vec4(1.0));

    ImageSampler::Ptr reflectivityMap = ImageSampler::create(8,8,3,Vec4(1.0));

    normals = ImageSampler::create(128.0,128.0,4,Vec4(0.0,0.0,1.0,1.0));
    normals->setWrapping(ImageSampler::Wrapping_Repeat);
    normals->setFiltering(ImageSampler::Filtering_Linear);
    BumpToNormalOperation op;
    op.setTarget(sampler->getSource());
    op.execute(normals->getSource());

    sf::Clock clock;

    sf::RenderWindow window(sf::VideoMode(512, 512), "My window", sf::Style::Default, sf::ContextSettings(24));
    window.setFramerateLimit(60);

    Graph::Ptr graph = Graph::create();
    Node::Ptr root = Node::create();

    Camera::Ptr camera = Camera::create(Vec3(0.0,1.0,0.0));
    camera->setPosition(Vec3(10.0f, 10.0f, 10.0f));
    camera->setTarget(Vec3(0.0f, 0.0f, 0.0f));

    imp::ReflexionModel::Ptr r = imp::ReflexionModel::create(
            imp::ReflexionModel::Lighting_Phong,
            imp::ReflexionModel::Texturing_Samplers_CNE,
            imp::ReflexionModel::MRT_2_Col_Emi, "shader 1");

    imp::ReflexionModel::Ptr r2 = imp::ReflexionModel::create(
            imp::ReflexionModel::Lighting_None,
            imp::ReflexionModel::Texturing_Samplers_CNE,
            imp::ReflexionModel::MRT_2_Col_Emi, "shader 2");

    Material::Ptr material = Material::create(Vec3(0.3,1.0,0.4), 1.0);
    material->_baseColor = color;
    // material->_emissive = color;
    material->_normalmap = normals;
    material->_reflectivity = reflectivityMap;

    Material::Ptr light_material = Material::create(Vec3(1.0), 1.0);
    light_material->_emissive = emi;

    Geometry::Ptr terrain = Geometry::create();
    *terrain = generateTerrain(sampler);
    GeoNode::Ptr terrainNode = GeoNode::create(terrain, false);
    terrainNode->setReflexion(r);
    terrainNode->setMaterial(material);
    //terrainNode->setPosition(Vec3(0.0,10.0,0.0));

    Geometry::Ptr coords = buildCoord();
    coords->scale(Vec3(30.0));
    GeoNode::Ptr coordsNode = GeoNode::create(coords, false);
    coordsNode->setReflexion(r2);
    coordsNode->setMaterial(material);
    coordsNode->setPosition(Vec3(0.0,15.0,0.0));

    LightNode::Ptr light = LightNode::create(Vec3(1.0),10.f);

    Geometry::Ptr pointGeo = Geometry::create();
    *pointGeo = Geometry::sphere(8, 0.4);
    pointGeo->setPrimitive(Geometry::Primitive_Triangles);
    pointGeo->generateColors(Vec3(0.3,0.3,1.0));
    pointGeo->generateNormals();
    pointGeo->generateTexCoords(Geometry::TexGenMode_Spheric);
    GeoNode::Ptr pointNode = GeoNode::create(pointGeo, false);
    pointNode->setPosition(Vec3(0.0,2,0.0));
    pointNode->setReflexion(r);
    pointNode->setMaterial(light_material);

    SkyBox::Ptr sky = SkyBox::create();

    root->addNode(camera);
    root->addNode(terrainNode);
    root->addNode(coordsNode);
    root->addNode(light);
    root->addNode(pointNode);
    root->addNode(sky);
    graph->setRoot(root);

    RenderPass::Ptr rp_info = RenderPass::create();
    rp_info->enablePass(RenderPass::Pass_EnvironmentMapping);
    RenderPass::Ptr rp_info2 = RenderPass::create();
    rp_info2->enablePass(RenderPass::Pass_ShadowMapping);
    coordsNode->setRenderPass(rp_info);
    sky->setRenderPass(rp_info);
    pointNode->setRenderPass(rp_info);

    terrainNode->setRenderPass(rp_info2);

    SceneRenderer::Ptr renderer = renderModeMngr.loadRenderer();
    renderer->enableFeature(SceneRenderer::Feature_Shadow, true);
    renderer->enableFeature(SceneRenderer::Feature_Environment, true);
    renderer->setDirect(true);
    // renderer->enableFeature(SceneRenderer::Feature_Bloom, false);
    graph->getInitState()->setViewport( renderModeMngr.viewport );
    graph->setClearColor(Vec4(0.0,0.0,1.0,1.0));

    renderer->setOutputFrame(SceneRenderer::RenderFrame_Default);

    bool generate_cubemap = false;

    Image::Ptr target;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed || event.type == sf::Event::KeyPressed)
                window.close();
        }
        if(!window.isOpen()) break;

        double t = clock.getElapsedTime().asSeconds();

        double duration = 3.0;
        int frame_count = 9;
        int frame_index = 0;//int(t/duration) % frame_count;
        if(frame_index == 0)
            renderer->setOutputFrame(SceneRenderer::RenderFrame_Default);
        else if(frame_index == 1)
            renderer->setOutputFrame(SceneRenderer::RenderFrame_Lighting);
        else if(frame_index == 2)
            renderer->setOutputFrame(SceneRenderer::RenderFrame_Emissive);
        else if(frame_index == 3)
            renderer->setOutputFrame(SceneRenderer::RenderFrame_Normals);
        else if(frame_index == 4)
            renderer->setOutputFrame(SceneRenderer::RenderFrame_ShadowMap);
        else if(frame_index == 5)
            renderer->setOutputFrame(SceneRenderer::RenderFrame_Environment);
        else if(frame_index == 6)
            renderer->setOutputFrame(SceneRenderer::RenderFrame_Bloom);
        else if(frame_index == 7)
            renderer->setOutputFrame(SceneRenderer::RenderFrame_Color);
        else if(frame_index == 8)
            renderer->setOutputFrame(SceneRenderer::RenderFrame_Reflectivity);


        Vec3 lp(cos(t)*10.0,7.0,sin(t)*10.0);
        light->setPosition(lp);
        pointNode->setPosition(lp);

        Vec3 lp2(cos(-t * 0.2),1.0,sin(-t * 0.2));
        camera->setPosition(lp2*Vec3(10.0,5.0,10.0));
        camera->setTarget(Vec3(0.0f, 0.f, 0.0f));


        if(generate_cubemap)
        {
            renderer->setDirect(false);
            // render to cubemap camera layout
            // positive X
            camera->setPosition( Vec3(0.0,3.0,0.0) );
            camera->setTarget( Vec3(1.0,3.0,0.0) );
            renderer->render( graph );
            target = renderer->getTarget(true, 0);
            ImageIO::save(target, "./cubemap/right.tga");
            // negative X
            camera->setPosition( Vec3(0.0,3.0,0.0) );
            camera->setTarget( Vec3(-1.0,3.0,0.0) );
            renderer->render( graph );
            target = renderer->getTarget(true, 0);
            ImageIO::save(target, "./cubemap/left.tga");
            // positive Y
            camera->setUpDir( Vec3(0.0,0.0,-1.0) );
            camera->setPosition( Vec3(0.0,3.0,0.0) );
            camera->setTarget( Vec3(0.0,4.0,0.0) );
            renderer->render( graph );
            target = renderer->getTarget(true, 0);
            ImageIO::save(target, "./cubemap/top.tga");
            // negative Y
            camera->setUpDir( Vec3(0.0,0.0,1.0) );
            camera->setPosition( Vec3(0.0,3.0,0.0) );
            camera->setTarget( Vec3(0.0,2.0,0.0) );
            renderer->render( graph );
            target = renderer->getTarget(true, 0);
            ImageIO::save(target, "./cubemap/bottom.tga");
            // positive Z
            camera->setUpDir( Vec3(0.0,1.0,0.0) );
            camera->setPosition( Vec3(0.0,3.0,0.0) );
            camera->setTarget( Vec3(0.0,3.0,1.0) );
            renderer->render( graph );
            target = renderer->getTarget(true, 0);
            ImageIO::save(target, "./cubemap/back.tga");
            // negative Z
            camera->setPosition( Vec3(0.0,3.0,0.0) );
            camera->setTarget( Vec3(0.0,3.0,-1.0) );
            renderer->render( graph );
            target = renderer->getTarget(true, 0);
            ImageIO::save(target, "./cubemap/front.tga");
            renderer->setDirect(true);

            generate_cubemap = false;
        }


        renderer->render( graph );

        renderModeMngr.draw(window);
        window.display();
        // break;
    }


    exit(EXIT_SUCCESS);
}
