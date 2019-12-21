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
// #include "../common/basic_skybox.h"


Geometry::Ptr generateCase()
{
    Geometry::Ptr geometry = Geometry::create();
    *geometry = imp::Geometry::cube();
    *geometry = geometry->subdivise(10);
    Geometry::intoCCW(*geometry,true);
    geometry->generateColors(Vec3(0.8, 0.5, 0.3));
    geometry->generateTexCoords(Geometry::TexGenMode_Cubic,1.0);
    // geometry->scale(Vec3(1.0,1.0,1.0));
    geometry->generateNormals(Geometry::NormalGenMode_PerFace);
    // geometry->interpolateNormals();

    return geometry;
}

#define GLSL_CODE( code ) #code

static std::string glsl_phong_bis = GLSL_CODE(
uniform mat4 u_model;
uniform mat4 u_view;
uniform vec3 u_color;
uniform float u_shininess;

varying vec2 v_texCoord;
varying vec3 v_m;
varying vec3 v_mv;
varying vec3 v_n;
varying vec3 a_n;
varying vec3 v_vertex;

mat3 build_tbn(vec3 n_ref)
{
    vec3 n_z = n_ref; // normalize();
    vec3 n_x = vec3(1.0,0.0,0.0);
    vec3 n_y = cross(n_z,n_x);
    vec3 n_x2 = vec3(0.0,1.0,0.0);
    vec3 n_y_2 = cross(n_z,n_x2);
    if(length(n_y) < length(n_y_2)) n_y = n_y_2;

    n_y = normalize(n_y);
    n_x = cross(n_y,n_z); n_x=normalize(n_x);
    mat3 tbn = mat3(n_x,n_y,n_z);
    return tbn;
}

void lighting(out vec4 out_color,
              out vec4 out_emissive,
              out vec3 out_normal,
              out float out_reflectivity,
              out float out_shininess,
              out float out_depth)
{
    out_color = vec4(1.0,0.0,1.0, 1.0);
    out_emissive = vec4(1.0,0.0,1.0, 1.0);
    out_normal = vec3(0.0);
    out_reflectivity = 0.0;
    out_shininess = 0.0;
    out_depth = 1.0;
}
);


Geometry::Ptr generateCase2()
{
    Geometry::Ptr geometry = generateCase();
    // geometry->sphericalNormalization(0.7);
    *geometry += Vec3(0.0, 1.0, 0.0);
    // geometry->interpolateNormals();

    return geometry;
}

Geometry::Ptr generateRoom()
{
    Geometry::Ptr geometry = Geometry::create();
    *geometry = imp::Geometry::cube();
    Geometry::intoCCW(*geometry,false);
    geometry->generateColors(Vec3(0.3,0.5,1.0));
    geometry->generateTexCoords(Geometry::TexGenMode_Cubic,10.0);
    geometry->scale(Vec3(50.0));
    geometry->generateNormals(Geometry::NormalGenMode_PerFace);
    *geometry += Vec3(0.0, 50.0, 0.0);

    return geometry;
}


Geometry::Ptr generateRoom2()
{
    Geometry::Ptr wall_01 = generateCase();
    Geometry::Ptr wall_02 = generateCase();
    Geometry::Ptr wall_03 = generateCase();
    Geometry::Ptr wall_04 = generateCase();
    Geometry::Ptr ground = generateCase();
    Geometry::Ptr woof = generateCase();

    wall_01->scale( Vec3(1.0, 50.0, 50.0) );
    wall_02->scale( Vec3(50.0, 50.0, 1.0) );
    wall_03->scale( Vec3(1.0, 50.0, 50.0) );
    wall_04->scale( Vec3(50.0, 50.0, 1.0) );
    ground->scale( Vec3(50.0, 1.0, 50.0) );
    woof->scale( Vec3(50.0, 1.0, 50.0) );

    *wall_01 += Vec3(50.0, 0.0, 0.0);
    *wall_02 += Vec3(0.0, 0.0, 50.0);
    *wall_03 += Vec3(-50.0, 0.0, 0.0);
    *wall_04 += Vec3(0.0, 0.0, -50.0);
    *woof += Vec3(0.0, 49.0, 0.0);
    *ground += Vec3(0.0, -51.0, 0.0);

    Geometry::Ptr geometry = Geometry::create();
    *geometry = *wall_01;
    *geometry += *wall_02;
    *geometry += *wall_03;
    *geometry += *wall_04;
    *geometry += *ground;
    *geometry += *woof;

    *geometry += Vec3(0.0, 50.0, 0.0);
    // geometry->generateColors(Vec3(0.3,0.7,0.5));
    geometry->generateColors(Vec3(0.5,0.5,1.0));

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

    normals = ImageSampler::create(128.0,128.0,4,Vec4(0.0,0.0,1.0,1.0));
    normals->setWrapping(ImageSampler::Wrapping_Repeat);
    normals->setFiltering(ImageSampler::Filtering_Linear);
    BumpToNormalOperation op;
    op.setTarget(sampler->getSource());
    op.execute(normals->getSource());


    ImageSampler::Ptr reflectivityMap = ImageSampler::create(32,32,3,Vec4(1.0));
    for(float u=0.1;u<0.9;u+=0.03125) for(float v=0.1;v<0.9;v+=0.03125) reflectivityMap->set(u,v,Vec4(0.0));

    ImageSampler::Ptr emissiveMap = ImageSampler::create(16,16,3,Vec4(0.0));
    for(float u=0.4;u<0.6;u+=0.125) for(float v=0.4;v<0.6;v+=0.125) emissiveMap->set(u,v,Vec4(1.0,1.0,1.0,1.0));

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
    // r->_fragCode_lighting = glsl_phong_bis;

    imp::ReflexionModel::Ptr r2 = imp::ReflexionModel::create(
            imp::ReflexionModel::Lighting_None,
            imp::ReflexionModel::Texturing_Samplers_CNE,
            imp::ReflexionModel::MRT_1_Col,
            "glsl for coords");

    Material::Ptr case_material = Material::create(Vec3(1.0), 32.0);
    case_material->_emissive = emissiveMap;
    case_material->_normalmap = normals;
    case_material->_reflectivity = reflectivityMap;

    Material::Ptr material = Material::create(Vec3(1.0), 32.0);
    // material->_baseColor = color;
    // material->_emissive = color;
    material->_normalmap = normals;

    Material::Ptr light_material = Material::create(Vec3(1.0), 1.0);
    light_material->_emissive = emi;

    GeoNode::Ptr roomNode = GeoNode::create(generateRoom2(), false);
    roomNode->setReflexion(r);
    roomNode->setMaterial(material);
    roomNode->setPosition(Vec3(0.0,-50.0,0.0));

    Geometry::Ptr coords = buildCoord();
    coords->scale(Vec3(100.0));
    GeoNode::Ptr coordsNode = GeoNode::create(coords, false);
    coordsNode->setReflexion(r2);
    coordsNode->setMaterial(material);
    coordsNode->setPosition(Vec3(0.0,10.0,0.0));

    LightNode::Ptr light = LightNode::create(Vec3(1.0),100.f);

    Geometry::Ptr pointGeo = Geometry::create();
    *pointGeo = Geometry::sphere(8, 0.4);
    pointGeo->setPrimitive(Geometry::Primitive_Triangles);
    pointGeo->generateColors(Vec3(0.3,0.3,1.0));
    pointGeo->generateNormals();
    pointGeo->generateTexCoords(Geometry::TexGenMode_Spheric);
    GeoNode::Ptr pointNode = GeoNode::create(pointGeo, false);
    pointNode->setPosition(Vec3(0.0,-50.0+10.0,0.0));
    pointNode->setReflexion(r);
    pointNode->setMaterial(light_material);

    GeoNode::Ptr case01_node = GeoNode::create(generateCase2(), false);
    case01_node->setReflexion(r);
    case01_node->setMaterial(case_material);
    case01_node->setScale(Vec3(10.0));
    case01_node->setPosition(Vec3(20.0, -50.0, 20.0));
    GeoNode::Ptr case02_node = GeoNode::create(generateCase2(), false);
    case02_node->setReflexion(r);
    case02_node->setMaterial(case_material);
    case02_node->setScale(Vec3(5.0));
    case02_node->setPosition(Vec3(-10.0, -50.0, 10.0));
    GeoNode::Ptr case03_node = GeoNode::create(generateCase2(), false);
    case03_node->setReflexion(r);
    case03_node->setMaterial(case_material);
    case03_node->setScale(Vec3(2.0));
    case03_node->setPosition(Vec3(0.0, -50.0, 0.0));

    root->addNode(camera);
    root->addNode(roomNode);
    root->addNode(coordsNode);
    root->addNode(light);
    root->addNode(pointNode);
    root->addNode(case01_node);
    root->addNode(case02_node);
    root->addNode(case03_node);
    graph->setRoot(root);

    roomNode->getState()->getRenderPass()->enablePass(RenderPass::Pass_ShadowMapping);
    coordsNode->getState()->getRenderPass()->enablePass(RenderPass::Pass_ShadowMapping);
    case01_node->getState()->getRenderPass()->enablePass(RenderPass::Pass_ShadowMapping);
    case02_node->getState()->getRenderPass()->enablePass(RenderPass::Pass_ShadowMapping);
    case03_node->getState()->getRenderPass()->enablePass(RenderPass::Pass_ShadowMapping);

    // coordsNode->getState()->getRenderPass()->enablePass(RenderPass::Pass_EnvironmentMapping);
    roomNode->getState()->getRenderPass()->enablePass(RenderPass::Pass_EnvironmentMapping);
    case01_node->getState()->getRenderPass()->enablePass(RenderPass::Pass_EnvironmentMapping);
    case02_node->getState()->getRenderPass()->enablePass(RenderPass::Pass_EnvironmentMapping);
    // case03_node->getState()->getRenderPass()->enablePass(RenderPass::Pass_EnvironmentMapping);

    SceneRenderer::Ptr renderer = renderModeMngr.loadRenderer();
    renderer->enableFeature(SceneRenderer::Feature_Shadow, true);
    renderer->enableFeature(SceneRenderer::Feature_Environment, true);
    renderer->enableFeature(SceneRenderer::Feature_Bloom, true);
    renderer->enableFeature(SceneRenderer::Feature_SSAO, true);
    // renderer->setDirect(false);
    graph->getInitState()->setViewport( renderModeMngr.viewport );
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

        Vec3 lp(cos(t)*20.0,20.0+cos(t)*10.0-50.0,sin(t)*20.0);
        light->setPosition(lp);
        pointNode->setPosition(lp);

        Vec3 oft(0.0,0.0,0.0);
        Vec3 lp2(cos(-t * 0.2),1.0,sin(-t * 0.2));
        camera->setPosition(lp2*Vec3(40.0,25.0-50.0,40.0));
        camera->setTarget(oft+Vec3(0.0f, 10.f-50.0, 0.0f));


        window.clear();
        renderer->render( graph );

        renderModeMngr.draw(window);
        window.display();
        // break;
    }


    exit(EXIT_SUCCESS);
}
