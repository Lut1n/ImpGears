#include <ImpGears/SceneGraph/Graph.h>
#include <ImpGears/SceneGraph/Camera.h>
#include <ImpGears/SceneGraph/GeoNode.h>
#include <ImpGears/SceneGraph/QuadNode.h>
#include <ImpGears/SceneGraph/RenderPass.h>
#include <ImpGears/Descriptors/ImageIO.h>
#include <ImpGears/Descriptors/FileInfo.h>
#include <ImpGears/Descriptors/JsonImageOp.h>

#include <ImpGears/Renderer/CpuRenderer.h>

#include <ImpGears/Plugins/RenderPlugin.h>

#include <ImpGears/Graphics/ImageOperation.h>

#include <SFML/Graphics.hpp>

using namespace imp;

// common stuff
#include "../common/inc_experimental.h"
#define IMPLEMENT_RENDER_MODE_MANAGER
#include "../common/RenderModeManager.h"
#define IMPLEMENT_BASIC_GEOMETRIES
#include "../common/basic_geometries.h"

#include "render_skybox.h"


Geometry generateTerrain(const ImageSampler::Ptr& hm)
{
    imp::Geometry geometry = imp::Geometry::cube();
    geometry = geometry.subdivise(100);
    Geometry::intoCCW(geometry, true);
    geometry.generateColors(Vec3(1.0));
    geometry.generateTexCoords(Geometry::TexGenMode_Cubic,100.0);

    for(int k=0;k<geometry.size();++k)
    {
        Vec3 v = geometry._vertices[k];
        Vec2 uv = Vec2(v[0],v[2])*0.5 + 0.5;
        if(v[1] > 0.0) v[1] += (hm->get(uv)[0] * 2.0 - 1.0);
        geometry._vertices[k] = v;
    }
    geometry.scale(Vec3(300.0,50.0,300.0));
    geometry.generateNormals(Geometry::NormalGenMode_PerFace);
    // geometry.interpolateNormals();

    return geometry;
}

Geometry::Ptr generateNormalView(Geometry::Ptr src)
{
    Geometry::Ptr view = Geometry::create();
    view->setPrimitive(Geometry::Primitive_Lines);
    for(int k=0;k<src->size();k+=3)
    {
        Vec3 v = src->_vertices[k+0] + src->_vertices[k+1] + src->_vertices[k+2];
        Vec3 n = src->_normals[k+0] + src->_normals[k+1] + src->_normals[k+2];
        v = v*0.33;
        n = n*0.33;
        view->add(v);
        view->add(v+n * 1.0);
    }
    view->generateColors(Vec3(0.0,0.0,1.0));
    return view;
}

Geometry::Ptr generateBox()
{
    Geometry::Ptr geometry = Geometry::create();
    *geometry = imp::Geometry::cube();
    // *geometry = geometry->subdivise(10);
    Geometry::intoCCW(*geometry,true);
    geometry->generateColors(Vec3(1.0));
    geometry->generateTexCoords(Geometry::TexGenMode_Cubic,1.0);
    // geometry->scale(Vec3(1.0,1.0,1.0));
    geometry->generateNormals(Geometry::NormalGenMode_PerFace);
    // geometry->interpolateNormals();
    *geometry += Vec3(0.0, 1.0, 0.0);

    return geometry;
}


Geometry::Ptr generateSphere()
{
    Geometry::Ptr geometry = Geometry::create();
    *geometry = imp::Geometry::sphere(8.0, 1.0);
    // *geometry = geometry->subdivise(10);
    Geometry::intoCCW(*geometry,true);
    geometry->generateColors(Vec3(1.0));
    geometry->generateTexCoords(Geometry::TexGenMode_Spheric,1.0);
    // geometry->scale(Vec3(1.0,1.0,1.0));
    geometry->generateNormals(Geometry::NormalGenMode_PerFace);
    geometry->interpolateNormals();
    *geometry += Vec3(0.0, 0.0, 0.0);

    return geometry;
}

Geometry::Ptr generateRoom()
{
    Geometry::Ptr geometry = Geometry::create();
    *geometry = imp::Geometry::cube();
    Geometry::intoCCW(*geometry,false);
    geometry->generateColors(Vec3(0.5,0.7,1.0));
    geometry->generateTexCoords(Geometry::TexGenMode_Cubic,10.0);
    geometry->scale(Vec3(50.0));
    geometry->generateNormals(Geometry::NormalGenMode_PerFace);
    *geometry += Vec3(0.0, 50.0, 0.0);

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
    sampler->_mipmap = true;

    color =ImageSampler::create();
    color->setSource( ImageIO::load("./cache/scene_color.tga") );
    color->setWrapping(ImageSampler::Wrapping_Repeat);
    color->setFiltering(ImageSampler::Filtering_Linear);
    color->_mipmap = true;
}

int main(int argc, char* argv[])
{
    RenderModeManager renderModeMngr;
    renderModeMngr.setArgs(argc, argv);

    ImageSampler::Ptr sampler, color, emi, normals;
    loadSamplers(sampler,color);
    emi = ImageSampler::create(8, 8, 4, Vec4(1.0));

    normals = ImageSampler::create(128.0,128.0,4,Vec4(0.0,0.0,1.0,1.0));
    normals->setWrapping(ImageSampler::Wrapping_Repeat);
    normals->setFiltering(ImageSampler::Filtering_Linear);
    BumpToNormalOperation op;
    op.setTarget(sampler->getSource());
    op.execute(normals->getSource());


    ImageSampler::Ptr emissiveMap = ImageSampler::create(32,32,3,Vec4(1.0));
    // emissiveMap->setWrapping(ImageSampler::Wrapping_Repeat);
    emissiveMap->setFiltering(ImageSampler::Filtering_Linear);
    emissiveMap->_mipmap = true;
    float texelS = 1.0/32.0;
    for(float u=texelS;u<1.0-texelS*2.0;u+=texelS) for(float v=texelS;v<1.0-texelS*2.0;v+=texelS) emissiveMap->set(u,v,Vec4(0.0));

    ImageSampler::Ptr reflectivityMap = ImageSampler::create(16,16,3,Vec4(0.0));
    reflectivityMap->_mipmap = true;
    for(float u=0.4;u<0.6;u+=0.125) for(float v=0.4;v<0.6;v+=0.125) reflectivityMap->set(u,v,Vec4(1.0,1.0,1.0,1.0));

    sf::Clock clock;

    sf::RenderWindow window(sf::VideoMode(512, 512), "Warehouse scene", sf::Style::Default, sf::ContextSettings(24));
    window.setFramerateLimit(60);

    Graph::Ptr graph = Graph::create();
    Node::Ptr root = Node::create();

    Camera::Ptr camera = Camera::create(Vec3(0.0,1.0,0.0));
    camera->setPosition(Vec3(10.0f, 10.0f, 10.0f));
    camera->setTarget(Vec3(0.0f, 0.0f, 0.0f));

    imp::ReflexionModel::Ptr r = imp::ReflexionModel::create(
            imp::ReflexionModel::Lighting_Phong,
            imp::ReflexionModel::Texturing_Samplers_CNE,
            imp::ReflexionModel::MRT_1_Col,
            "glsl for scene object");

    imp::ReflexionModel::Ptr r2 = imp::ReflexionModel::create(
            imp::ReflexionModel::Lighting_None,
            imp::ReflexionModel::Texturing_Samplers_CNE,
            imp::ReflexionModel::MRT_1_Col,
            "glsl for coords");

    Material::Ptr case_material = Material::create(Vec3(1.0), 128.0);
    // case_material->_baseColor = color;
    case_material->_emissive = emissiveMap;
    case_material->_normalmap = normals;
    case_material->_reflectivity = reflectivityMap;

    Material::Ptr material = Material::create(Vec3(1.0), 128.0);
    // material->_baseColor = color;
    // material->_emissive = color;
    material->_normalmap = normals;

    Material::Ptr light_material = Material::create(Vec3(1.0), 1.0);
    light_material->_emissive = emi;

    GeoNode::Ptr roomNode = GeoNode::create(generateRoom(), false);
    roomNode->setReflexion(r);
    roomNode->setMaterial(material);
    roomNode->setPosition(Vec3(0.0,-50.0,0.0));
    
    
    Geometry::Ptr geosphere = generateSphere();
    Geometry::Ptr geobox = generateBox();
    
    GeoNode::Ptr sphereNode = GeoNode::create(geosphere, false);
    sphereNode->setReflexion(r);
    // sphereNode->setMaterial(material);
    sphereNode->setPosition(Vec3(0.0,-20.0,0.0));
    
    GeoNode::Ptr cubeNode = GeoNode::create(geobox, false);
    cubeNode->setReflexion(r);
    // sphereNode->setMaterial(material);
    cubeNode->setPosition(Vec3(10.0,-20.0,0.0));

    Geometry::Ptr coords = buildCoord();
    coords->scale(Vec3(100.0));
    GeoNode::Ptr coordsNode = GeoNode::create(coords, false);
    coordsNode->setReflexion(r2);
    coordsNode->setMaterial(material);
    coordsNode->setPosition(Vec3(0.0,10.0,0.0));

    LightNode::Ptr light = LightNode::create(Vec3(1.0),100.f);

    Geometry::Ptr pointGeo = Geometry::create();
    *pointGeo = Geometry::sphere(8, 0.4);
    pointGeo->generateColors(Vec3(1.0));
    pointGeo->generateNormals();
    pointGeo->generateTexCoords(Geometry::TexGenMode_Spheric);
    GeoNode::Ptr pointNode = GeoNode::create(pointGeo, false);
    pointNode->setPosition(Vec3(0.0,-50.0+10.0,0.0));
    pointNode->setReflexion(r);
    pointNode->setMaterial(light_material);
    
    
    Geometry::Ptr terrain = Geometry::create();
    *terrain = generateTerrain(sampler);
    GeoNode::Ptr terrainNode = GeoNode::create(terrain, false);
    terrainNode->setReflexion(r);
    terrainNode->setPosition(Vec3(0.0,-100.0,0.0));
    terrainNode->setMaterial(material);
    
    
    Geometry::Ptr geonormal = generateNormalView(terrain);
    GeoNode::Ptr normalview = GeoNode::create(geonormal);
    normalview->setReflexion(r);
    normalview->setPosition(Vec3(00.0,-100.0,0.0));
        
    
    // =====  classic geometry ========
    Geometry::Ptr box_geometry = generateBox();
    GeoNode::Ptr case01_node = GeoNode::create(box_geometry);
    case01_node->setReflexion(r);
    case01_node->setMaterial(case_material);
    case01_node->setScale(Vec3(10.0));
    case01_node->setPosition(Vec3(20.0, -50.0, 20.0));
    GeoNode::Ptr case02_node = GeoNode::create(box_geometry);
    case02_node->setReflexion(r);
    case02_node->setMaterial(case_material);
    case02_node->setScale(Vec3(5.0));
    case02_node->setRotation(Vec3(0.0,2.0,0.0));
    case02_node->setPosition(Vec3(-10.0, -50.0, 10.0));
    GeoNode::Ptr case03_node = GeoNode::create(box_geometry);
    case03_node->setReflexion(r);
    case03_node->setMaterial(case_material);
    case03_node->setScale(Vec3(2.0));
    case03_node->setRotation(Vec3(0.0,1.0,0.0));
    case03_node->setPosition(Vec3(0.0, -50.0, 0.0));
    // =====  classic geometry ========
    
    
    SkyBox::Ptr sky = SkyBox::create();
    
    
    root->addNode(case01_node);
    root->addNode(case02_node);
    root->addNode(case03_node);
    root->addNode(sphereNode);
    root->addNode(normalview);
    root->addNode(cubeNode);
    root->addNode(sky);
    
    
    

    root->addNode(camera);
    // root->addNode(roomNode);
    root->addNode(coordsNode);
    root->addNode(light);
    root->addNode(pointNode);
    root->addNode(terrainNode);
    graph->setRoot(root);
    
    RenderPass::Ptr env_sha = RenderPass::create();
    RenderPass::Ptr env = RenderPass::create();
    RenderPass::Ptr sha = RenderPass::create();
    
    env->enablePass(RenderPass::Pass_EnvironmentMapping);
    env_sha->enablePass(RenderPass::Pass_EnvironmentMapping);
    env_sha->enablePass(RenderPass::Pass_ShadowMapping);
    sha->enablePass(RenderPass::Pass_ShadowMapping);

    light->getState()->setRenderPass(env);
    roomNode->getState()->setRenderPass(env);
    sky->getState()->setRenderPass(env);
    coordsNode->getState()->setRenderPass(sha);
    case01_node->getState()->setRenderPass(env_sha);
    case02_node->getState()->setRenderPass(env_sha);
    case03_node->getState()->setRenderPass(env_sha);
    sphereNode->getState()->setRenderPass(env_sha);
    cubeNode->getState()->setRenderPass(env_sha);
    terrainNode->getState()->setRenderPass(env_sha);

    SceneRenderer::Ptr renderer = renderModeMngr.loadRenderer();
    renderer->enableFeature(SceneRenderer::Feature_Shadow, true);
    renderer->enableFeature(SceneRenderer::Feature_Environment, false);
    renderer->enableFeature(SceneRenderer::Feature_Bloom, true);
    renderer->enableFeature(SceneRenderer::Feature_SSAO, false);
    renderer->enableFeature(SceneRenderer::Feature_Phong, true);
    // renderer->setDirect(false);
    graph->getInitState()->setViewport( renderModeMngr.viewport );
    graph->getInitState()->setLineWidth( 5.0 );
    // graph->setClearColor(Vec4(0.0,1.0,0.0,1.0));

    renderer->setOutputFrame(SceneRenderer::RenderFrame_Default);

    // renderer->setOutputFrame(SceneRenderer::RenderFrame_Depth);
    // renderer->setOutputFrame(SceneRenderer::RenderFrame_Normals);
    // renderer->setOutputFrame(SceneRenderer::RenderFrame_Bloom);
    // renderer->setOutputFrame(SceneRenderer::RenderFrame_Emissive);
    // renderer->setOutputFrame(SceneRenderer::RenderFrame_Environment);
    // renderer->setOutputFrame(SceneRenderer::RenderFrame_Lighting);
    // renderer->setOutputFrame(SceneRenderer::RenderFrame_ShadowMap);
    // renderer->setOutputFrame(SceneRenderer::RenderFrame_Color);
    // renderer->setOutputFrame(SceneRenderer::RenderFrame_Reflectivity);

    Image::Ptr target;
    
    float rotH = 0.0, rotV = 0.0;
    Vec3 campos(0.0,0.0,0.0);
    Vec3 start_forward(0.0,0.0,-1.0);
    Vec3 forward = start_forward;
    Vec3 up(0.0,1.0,0.0);
    Vec3 side = up.cross(forward);
    
    double lastt = clock.getElapsedTime().asSeconds();
    
    sf::Mouse::setPosition(sf::Vector2i(256, 256), window);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed /* || event.type == sf::Event::KeyPressed*/)
                window.close();
            // if(event.type == sf::Event::KeyPressed)
            // {
            //     
            // }
        }
        if(!window.isOpen()) break;

        double t = clock.getElapsedTime().asSeconds();
        double difft = t - lastt;
        
        double speed = 50.0;
        
        
        
        
                
        sf::Vector2i center(256, 256);
        // get global mouse position
        sf::Vector2i position = sf::Mouse::getPosition(window);
        // set mouse position relative to a window
        sf::Mouse::setPosition(center, window);
        
        sf::Vector2i diff = position - center;
        Vec2 move( float(diff.x)/256.0,  float(diff.y)/256.0);

            float sensibility = 0.3;
        rotH += -move[0] * sensibility;
        rotV += -move[1] * sensibility;
        
        rotV = clamp(rotV, -0.45f, 0.45f);

        forward = start_forward * Matrix3::rotationX(rotV * 3.141592) * Matrix3::rotationY(rotH * 3.141592);
        side = camera->getUpDir().cross(forward);
        
        
        double speed_bonus = 0.0;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
        {
            // move left...
            speed_bonus = 20.0;
        }
        
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
        {
            // move left...
            campos += side * (speed+speed_bonus) * difft;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            // move right...
            campos -= side * (speed+speed_bonus) * difft;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
        {
            // move right...
            campos += forward * (speed+speed_bonus) * difft;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            // move right...
            campos -= forward * (speed+speed_bonus) * difft;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        {
            // move right...
            campos += up * (speed+speed_bonus) * difft;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
        {
            // move right...
            campos -= up * (speed+speed_bonus) * difft;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        {
            return 0;
        }


        Vec3 lp(cos(t)*20.0,20.0+cos(t)*10.0-50.0,sin(t)*20.0);
        light->setPosition(lp);
        pointNode->setPosition(lp);

        // Vec3 oft(0.0,0.0,0.0);
        // Vec3 lp2(cos(-t * 0.2),1.0,sin(-t * 0.2));
        camera->setPosition(campos);
        camera->setTarget(campos+forward);


        window.clear();
        renderer->render( graph );

        renderModeMngr.draw(window);
        window.display();
        // break;
        
        lastt = t;
    }


    exit(EXIT_SUCCESS);
}
