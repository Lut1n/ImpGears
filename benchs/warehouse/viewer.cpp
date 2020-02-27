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

#include <Geometry/InstancedGeometry.h>

using namespace imp;

// common stuff
#include "../common/inc_experimental.h"
#define IMPLEMENT_RENDER_MODE_MANAGER
#include "../common/RenderModeManager.h"
#define IMPLEMENT_BASIC_GEOMETRIES
#include "../common/basic_geometries.h"
// #include "../common/basic_skybox.h"

#define USE_INSTANCING

Geometry::Ptr generateCase()
{
    Geometry::Ptr geometry = Geometry::create();
    *geometry = imp::Geometry::cube();
    // *geometry = geometry->subdivise(10);
    Geometry::intoCCW(*geometry,true);
    geometry->generateColors(Vec3(0.8, 0.5, 0.3));
    geometry->generateTexCoords(Geometry::TexGenMode_Cubic,1.0);
    // geometry->scale(Vec3(1.0,1.0,1.0));
    geometry->generateNormals(Geometry::NormalGenMode_PerFace);
    // geometry->interpolateNormals();

    return geometry;
}


#define GLSL_CODE( code ) #code

static std::string glsl_instancing_vert = GLSL_CODE(

// vertex attributes ( @see glBindAttribLocation )
in vec3 a_vertex;   // location 0
in vec3 a_color;    // location 1
in vec3 a_normal;   // location 2
in vec2 a_texcoord; // location 3

in mat4 a_instTransform; // location 4,5,6,7

// uniforms
uniform mat4 u_proj;
uniform mat4 u_view;
uniform mat4 u_model;
uniform mat3 u_normal;

// fragment shader stage input
out vec2 v_texCoord;
out vec3 v_m;
out vec3 v_mv;
out vec3 v_n;
out vec3 v_vertex;
out vec3 v_color;
out mat4 v_model;

void main()
{
    mat4 _model = u_model * a_instTransform;
    vec4 mv_pos = u_view * _model * vec4(a_vertex,1.0);
    gl_Position = u_proj * mv_pos;
    v_n = normalize(a_normal);
    v_color = a_color;

    v_texCoord = a_texcoord;
    v_m = (_model * vec4(a_vertex,1.0)).xyz;
    v_mv = mv_pos.xyz;
    v_vertex = a_vertex;
    v_model = _model;
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

    GeoNode::Ptr roomNode = GeoNode::create(generateRoom(), false);
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
    
    
    
        
    
    // =====  classic geometry ========
    GeoNode::Ptr case01_node = GeoNode::create(generateCase2(), false);
    case01_node->setReflexion(r);
    case01_node->setMaterial(case_material);
    case01_node->setScale(Vec3(10.0));
    case01_node->setPosition(Vec3(20.0, -50.0, 20.0));
    GeoNode::Ptr case02_node = GeoNode::create(generateCase2(), false);
    case02_node->setReflexion(r);
    case02_node->setMaterial(case_material);
    case02_node->setScale(Vec3(5.0));
    case02_node->setRotation(Vec3(0.0,2.0,0.0));
    case02_node->setPosition(Vec3(-10.0, -50.0, 10.0));
    GeoNode::Ptr case03_node = GeoNode::create(generateCase2(), false);
    case03_node->setReflexion(r);
    case03_node->setMaterial(case_material);
    case03_node->setScale(Vec3(2.0));
    case03_node->setRotation(Vec3(0.0,1.0,0.0));
    case03_node->setPosition(Vec3(0.0, -50.0, 0.0));
    // =====  classic geometry ========
    
    
    
    
    
    #ifdef USE_INSTANCING
    
    imp::ReflexionModel::Ptr instance_shader = imp::ReflexionModel::create(
            imp::ReflexionModel::Lighting_Phong,
            imp::ReflexionModel::Texturing_Samplers_CNE,
            imp::ReflexionModel::MRT_2_Col_Emi, "glsl instancing");
    instance_shader->_vertCode = glsl_instancing_vert;
    // instance_shader->_fragCode_lighting = glsl_instancing_frag;
    
    
    
    
    
    InstancedGeometry::Ptr instanced = InstancedGeometry::create();
    instanced->clone(case01_node->_geo);
    case01_node->computeMatrices();
    case02_node->computeMatrices();
    case03_node->computeMatrices();
    
    instanced->setTransformAt(0, case01_node->getModelMatrix());
    instanced->setTransformAt(1, case02_node->getModelMatrix());
    instanced->setTransformAt(2, case03_node->getModelMatrix());
    
    GeoNode::Ptr instance_node = GeoNode::create(instanced, false);
    instance_node->setMaterial(case_material);
    instance_node->setReflexion(instance_shader);
    root->addNode(instance_node);
    
    #else
    
    root->addNode(case01_node);
    root->addNode(case02_node);
    root->addNode(case03_node);
    
    #endif
    
    
    
    
    
    
    
    
    
    

    root->addNode(camera);
    root->addNode(roomNode);
    root->addNode(coordsNode);
    root->addNode(light);
    root->addNode(pointNode);
    graph->setRoot(root);
    
    RenderPass::Ptr env_sha = RenderPass::create();
    RenderPass::Ptr env = RenderPass::create();
    RenderPass::Ptr sha = RenderPass::create();
    
    env->enablePass(RenderPass::Pass_EnvironmentMapping);
    env_sha->enablePass(RenderPass::Pass_EnvironmentMapping);
    env_sha->enablePass(RenderPass::Pass_ShadowMapping);
    sha->enablePass(RenderPass::Pass_ShadowMapping);

    roomNode->getState()->setRenderPass(env);
    coordsNode->getState()->setRenderPass(sha);
    case01_node->getState()->setRenderPass(env_sha);
    case02_node->getState()->setRenderPass(env_sha);
    case03_node->getState()->setRenderPass(env_sha);
    
    #ifdef USE_INSTANCING
    instance_node->getState()->setRenderPass(env_sha);
    #endif

    SceneRenderer::Ptr renderer = renderModeMngr.loadRenderer();
    renderer->enableFeature(SceneRenderer::Feature_Shadow, true);
    renderer->enableFeature(SceneRenderer::Feature_Environment, true);
    renderer->enableFeature(SceneRenderer::Feature_Bloom, true);
    renderer->enableFeature(SceneRenderer::Feature_SSAO, true);
    renderer->enableFeature(SceneRenderer::Feature_Phong, true);
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
