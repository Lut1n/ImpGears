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

#include <ImpGears/Geometry/InstancedGeometry.h>

using namespace imp;

// common stuff
#include "../common/inc_experimental.h"
#define IMPLEMENT_RENDER_MODE_MANAGER
#include "../common/RenderModeManager.h"


#define N_PARTICULES 10000


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

uniform float u_iTime;
uniform float u_iCount;

// fragment shader stage input
out vec2 v_texCoord;
out vec3 v_m;
out vec3 v_mv;
out vec3 v_n;
out vec3 v_vertex;
out vec3 v_color;
out mat4 v_model;


vec3 animate()
{
    float particuleCount = u_iCount;
    float i = float(gl_InstanceID);
    
    float rp = float(i)/particuleCount;
    float MIN_D = 5.0;
    float MAX_D = 50.0;
    float startd = MIN_D + rp * (MAX_D-MIN_D);
    float starta = rp * 3.141592*2.0 * 471.23714;
    
    float duration = 50.0;
    float starttime = i*7134.923145*duration/particuleCount;
    float elapsed = u_iTime+starttime;
    while(elapsed  > duration ) elapsed -= duration;
    float animation_t = elapsed/duration;
    
    float d = mix(startd, 0.0,animation_t);
    float a = mix(starta, starta+3.14*2.0,animation_t);
    
    return vec3(cos(a),0.0,sin(a))*d+ vec3(0.0,mix(-50.0,50.0,animation_t*animation_t*animation_t*animation_t),0.0);
}

void main()
{
    vec3 offsetInst = animate();
    
    mat4 _model = u_model;
    mat4 billboard_view = u_view;
    billboard_view[0].xyz = vec3(1.0,0.0,0.0);
    billboard_view[1].xyz = vec3(0.0,1.0,0.0);
    billboard_view[2].xyz = vec3(0.0,0.0,1.0);
    vec4 mv_pos = billboard_view * ((_model * vec4(a_vertex,1.0))+vec4(offsetInst,0.0));
    gl_Position = u_proj * mv_pos;
    v_n = normalize(a_normal);
    v_color = a_color;

    v_texCoord = a_texcoord;
    v_m = (_model * vec4(a_vertex,1.0)).xyz + offsetInst;
    v_mv = mv_pos.xyz;
    v_vertex = a_vertex;
    v_model = _model;
}

);

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

int main(int argc, char* argv[])
{
    RenderModeManager renderModeMngr;
    
    std::vector<const char*> argarr = {argv[0],"-gpu"};
    renderModeMngr.setArgs(argarr.size(), argarr.data());


    ImageSampler::Ptr colorMap = ImageSampler::create(16,16,4,Vec4(0.0));
    ImageSampler::Ptr emissiveMap = ImageSampler::create(16,16,4,Vec4(0.0));
    for(float u=0.0;u<1.0;u+=0.0625) for(float v=0.0;v<1.0;v+=0.0625)
    {
        Vec2 c(0.5,0.5);
        Vec2 d = Vec2(u,v)-c;
        float dist = d.length();
        float w = clamp(dist/0.5); w = clamp(std::cos(w * 3.141592)*0.5+0.5, 0.0, 1.0);
        float alpha = w>0.02?w:0.0;
        Vec4 color(0.0,0.0,0.0,alpha);
        Vec4 emi(w,w,0.0,1.0);
        emissiveMap->set(u,v,emi);
        colorMap->set(u,v,color);
    }

    sf::Clock clock;

    sf::RenderWindow window(sf::VideoMode(512, 512), "Particules system", sf::Style::Default, sf::ContextSettings(24));
    window.setFramerateLimit(60);

    Graph::Ptr graph = Graph::create();
    Node::Ptr root = Node::create();

    Camera::Ptr camera = Camera::create(Vec3(0.0,1.0,0.0));
    camera->setPosition(Vec3(27.0,-25.0,27.0));
    camera->setTarget(Vec3(0.0f, -40.0, 0.0f));

    imp::ReflexionModel::Ptr refl = imp::ReflexionModel::create(
            imp::ReflexionModel::Lighting_Phong,
            imp::ReflexionModel::Texturing_Samplers_CNE,
            imp::ReflexionModel::MRT_1_Col,
            "glsl for scene object");

    Material::Ptr case_material = Material::create(Vec4(1.0), 32.0);
    case_material->_baseColor = colorMap;
    case_material->_emissive = emissiveMap;

    GeoNode::Ptr roomNode = GeoNode::create(generateRoom(), false);
    roomNode->setReflexion(refl);
    roomNode->setPosition(Vec3(0.0,-50.0,0.0));

    LightNode::Ptr light = LightNode::create(Vec3(1.0),100.f);
    light->setPosition(Vec3(0.0,-30.0,0.0));
    
    imp::ReflexionModel::Ptr instance_shader = imp::ReflexionModel::create(
            imp::ReflexionModel::Lighting_None,
            imp::ReflexionModel::Texturing_Samplers_CNE,
            imp::ReflexionModel::MRT_2_Col_Emi, "glsl instancing");
    instance_shader->_vertCode = glsl_instancing_vert;
    
    QuadNode quadnodetmp;
    InstancedGeometry::Ptr instanced = InstancedGeometry::create();
    instanced->clone(quadnodetmp._geo);
    instanced->setCount(N_PARTICULES);
    
    GeoNode::Ptr instance_node = GeoNode::create(instanced, false);
    instance_node->setMaterial(case_material);
    instance_node->setReflexion(instance_shader);
    instance_node->setScale(Vec3(0.2));
    root->addNode(instance_node);
    root->addNode(camera);
    root->addNode(roomNode);
    root->addNode(light);
    graph->setRoot(root);
    
    
    Uniform::Ptr u_time = Uniform::create("u_iTime", Uniform::Type_1f);
    Uniform::Ptr u_count = Uniform::create("u_iCount", Uniform::Type_1f);
    instance_node->getState()->setUniform(u_time);
    instance_node->getState()->setUniform(u_count);
    
    u_count->set(float(N_PARTICULES));

    SceneRenderer::Ptr renderer = renderModeMngr.loadRenderer();
    renderer->enableFeature(SceneRenderer::Feature_Shadow, false);
    renderer->enableFeature(SceneRenderer::Feature_Environment, false);
    renderer->enableFeature(SceneRenderer::Feature_Bloom, true);
    renderer->enableFeature(SceneRenderer::Feature_SSAO, false);
    renderer->enableFeature(SceneRenderer::Feature_Phong, true);
    graph->getInitState()->setViewport( renderModeMngr.viewport );

    renderer->setOutputFrame(SceneRenderer::RenderFrame_Default);


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
        u_time->set(float(t));

        window.clear();
        renderer->render( graph );

        renderModeMngr.draw(window);
        window.display();
    }


    exit(EXIT_SUCCESS);
}
