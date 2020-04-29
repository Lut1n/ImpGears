#include <ImpGears/SceneGraph/Graph.h>
#include <ImpGears/SceneGraph/Camera.h>
#include <ImpGears/SceneGraph/GeoNode.h>
#include <ImpGears/SceneGraph/QuadNode.h>
#include <ImpGears/SceneGraph/RenderPass.h>
#include <ImpGears/SceneGraph/RenderToSamplerNode.h>
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
#define IMPLEMENT_RENDER_MODE_MANAGER
#include "../common/RenderModeManager.h"



#define FOV 90.0
#define NEAR 5.0





#define GLSL_CODE( code ) #code

static std::string glsl_billboard_vert = GLSL_CODE(

// vertex attributes ( @see glBindAttribLocation )
in vec3 a_vertex;   // location 0
in vec3 a_color;    // location 1
in vec3 a_normal;   // location 2
in vec2 a_texcoord; // location 3

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
    mat4 _model = u_model;
    mat4 billboard_view = u_view;
    billboard_view[0].xyz = vec3(1.0,0.0,0.0);
    billboard_view[1].xyz = vec3(0.0,1.0,0.0);
    billboard_view[2].xyz = vec3(0.0,0.0,1.0);
    vec4 mv_pos = billboard_view * _model * vec4(a_vertex,1.0);
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

static std::string glsl_line2d_vert = GLSL_CODE(

// vertex attributes ( @see glBindAttribLocation )
in vec3 a_vertex;   // location 0
in vec3 a_color;    // location 1
in vec3 a_normal;   // location 2
in vec2 a_texcoord; // location 3

// uniforms
uniform mat4 u_proj;
uniform mat4 u_view;
uniform mat4 u_model;
uniform mat3 u_normal;

uniform vec3 u_p1;
uniform vec3 u_p2;

// fragment shader stage input
out vec2 v_texCoord;
out vec3 v_m;
out vec3 v_mv;
out vec3 v_n;
out vec3 v_vertex;
out vec3 v_color;
out mat4 v_model;

vec3 rot(vec3 v, float a)
{
    return vec3(v.x*cos(a)-v.z*sin(a),v.y,v.x*sin(a)+v.z*cos(a));
}

vec3 line2d(vec3 v)
{
    vec3 c = (u_p1+u_p2) * 0.5;
    vec3 dif = u_p2 - u_p1;
    vec3 dif_n = normalize(dif);
    
    vec3 m = vec3(v.x,v.y,v.z);

    // scale
    float linewidth = 0.5;
    float l = length(dif);
    m= m*vec3(linewidth, 1.0, l*0.5);
    
    // rotation
    float a = atan(dif_n.x,dif_n.z);
    m = rot(m,-a);

    // translation
    m += c;
    
    return m;
}

void main()
{
    vec3 _vertex = line2d(a_vertex);
    mat4 _model = u_model;
    mat4 billboard_view = u_view;
    // billboard_view[0].xyz = vec3(1.0,0.0,0.0);
    // billboard_view[1].xyz = vec3(0.0,1.0,0.0);
    // billboard_view[2].xyz = vec3(0.0,0.0,1.0);
    vec4 mv_pos = billboard_view * _model * vec4(_vertex,1.0);
    gl_Position = u_proj * mv_pos;
    v_n = normalize(a_normal);
    v_color = a_color;

    v_texCoord = a_texcoord;
    v_m = (_model * vec4(_vertex,1.0)).xyz;
    v_mv = mv_pos.xyz;
    v_vertex = _vertex;
    v_model = _model;
}

);


Geometry::Ptr generateBox()
{
    Geometry::Ptr geometry = Geometry::create();
    *geometry = imp::Geometry::cube();
    Geometry::intoCCW(*geometry,true);
    geometry->generateColors(Vec3(1.0));
    geometry->generateTexCoords(Geometry::TexGenMode_Cubic,1.0);
    geometry->generateNormals(Geometry::NormalGenMode_PerFace);
    *geometry += Vec3(0.0, 1.0, 0.0);

    return geometry;
}

struct MiniMap : public RenderToSamplerNode
{
public:

    Meta_Class(MiniMap)
    
    Uniform::Ptr u_p1, u_p2, u_p3, u_p4;
    
    GeoNode::Ptr fakecamera;

    MiniMap()
    {
    }
    
    void init(LightNode::Ptr light, GeoNode::Ptr box)
    {
        ImageSampler::Ptr red = ImageSampler::create(8,8,3,Vec4(1.0,0.0,0.0,1.0));
        ImageSampler::Ptr green = ImageSampler::create(8,8,3,Vec4(0.0,1.0,0.0,1.0));
        
        ImageSampler::Ptr tex = ImageSampler::create(512,512,4,Vec4(1.0,0.0,0.0,1.0));
        setTexture(tex);
        
        Camera::Ptr camera2 = Camera::create(Vec3(0.0,0.0,0-1.0));
        camera2->setPosition(Vec3(0.0f, 1.0f, 10.0f));
        camera2->setTarget(Vec3(0.0f, 0.0f, 10.0f));
        
        Graph::Ptr graph2d = Graph::create();
        graph2d->getInitState()->setViewport( Vec4(0.0,0.0,512.0,512.0) );
        graph2d->getInitState()->setOrthographicProjection(-30.f, 30.f, -30.f, 30.f, -30.f, 30.f);
        graph2d->setClearColor(Vec4(1.0));
        Node::Ptr root2 = Node::create();
        root2->addNode(box);
        root2->addNode(camera2);
        // root2->addNode(light);
        graph2d->setRoot(root2);
        setScene(graph2d);
        
        imp::ReflexionModel::Ptr shader = imp::ReflexionModel::create(
            imp::ReflexionModel::Lighting_None,
            imp::ReflexionModel::Texturing_Samplers_CNE,
            imp::ReflexionModel::MRT_1_Col,
            "glsl for scene object");

        Material::Ptr fakecamera_material = Material::create(Vec3(1.0), 32.0);
        fakecamera_material->_baseColor = red;
        fakecamera_material->_emissive = red;
        
        Material::Ptr bg_material = Material::create(Vec3(1.0), 32.0);
        bg_material->_baseColor = green;
        bg_material->_emissive = green;
        
        fakecamera = GeoNode::create(generateBox(), false);
        fakecamera->setScale(Vec3(1.0));
        fakecamera->setPosition(Vec3(0.0, 0.0, 0.0));
        fakecamera->setMaterial(fakecamera_material);
        fakecamera->setReflexion(shader);
        root2->addNode(fakecamera);
        
        GeoNode::Ptr backg = GeoNode::create(generateBox(), false);
        backg->setPosition(Vec3(0.0, -10.0, 0.0));
        backg->setScale(Vec3(100.0,0.01,100.0));
        backg->setMaterial(bg_material);
        backg->setReflexion(shader);
        root2->addNode(backg);
        
        Material::Ptr line2d_material = Material::create(Vec3(0.0), 32.0);
        
        ReflexionModel::Ptr line2dshader = ReflexionModel::create(
                ReflexionModel::Lighting_None,
                ReflexionModel::Texturing_Samplers_CNE,
                ReflexionModel::MRT_1_Col, "glsl line2d");
        line2dshader->_vertCode = glsl_line2d_vert;
        
        u_p1 = Uniform::create("u_p1", Uniform::Type_3f);
        u_p2 = Uniform::create("u_p2", Uniform::Type_3f);
        
        u_p3 = Uniform::create("u_p1", Uniform::Type_3f);
        u_p4 = Uniform::create("u_p2", Uniform::Type_3f);
        
        float near = NEAR;
        
        Vec3 p2 = Vec3(0.0,0.0,-1.0)*50.0 * Matrix3::rotationY(FOV*0.5 * 3.141592 / 180.0);
        Vec3 p4 = Vec3(0.0,0.0,-1.0)*50.0 * Matrix3::rotationY(-FOV*0.5 * 3.141592 / 180.0);
        
        Vec3 p1 = p2; p1.normalize(); p1 *= near;
        Vec3 p3 = p4; p3.normalize(); p3 *= near;
        
        u_p1->set(p1);
        u_p2->set(p2);
        u_p3->set(p3);
        u_p4->set(p4);
        
        GeoNode::Ptr line2d = GeoNode::create(generateBox(),false);
        line2d->setReflexion(line2dshader);
        line2d->getState()->setUniform(u_p1);
        line2d->getState()->setUniform(u_p2);
        line2d->setMaterial(line2d_material);
        
        root2->addNode(line2d);
        
        GeoNode::Ptr line2d_2 = GeoNode::create(generateBox(),false);
        line2d_2->setReflexion(line2dshader);
        line2d_2->getState()->setUniform(u_p3);
        line2d_2->getState()->setUniform(u_p4);
        line2d_2->setMaterial(line2d_material);
        
        root2->addNode(line2d_2);
        
        
        Material::Ptr material = Material::create(Vec3(1.0), 32.0);
        material->_baseColor = texture();
        material->_emissive = texture();
        
        QuadNode::Ptr displayquad = QuadNode::create();
        displayquad->setMaterial(material);
        displayquad->setPosition(Vec3(-0.75,-0.75,0.0));
        displayquad->setScale(Vec3(0.2));
        
        ReflexionModel::Ptr displayshader = ReflexionModel::create(
                ReflexionModel::Lighting_None,
                ReflexionModel::Texturing_Samplers_CNE,
                ReflexionModel::MRT_1_Col, "glsl texture display");
                
        displayquad->setReflexion(displayshader);
        addNode(displayquad);
        
        makeDirty();
    }
    
    void updateFov(Vec3 cam, float fov, float near = NEAR)
    {
        fakecamera->setPosition(cam);
        
        Vec3 p2 = Vec3(0.0,0.0,-1.0)*50.0 * Matrix3::rotationY(fov*0.5 * 3.141592 / 180.0);
        Vec3 p4 = Vec3(0.0,0.0,-1.0)*50.0 * Matrix3::rotationY(-fov*0.5 * 3.141592 / 180.0);
        
        Vec3 p1 = p2; p1.normalize(); p1 *= near;
        Vec3 p3 = p4; p3.normalize(); p3 *= near;
        
        u_p1->set(Vec3(cam+p1));
        u_p2->set(Vec3(cam+p2));
        u_p3->set(Vec3(cam+p3));
        u_p4->set(Vec3(cam+p4));
        
        makeDirty();
    }
};


struct MainScene : public RenderToSamplerNode
{
public:

    Meta_Class(MainScene)
    
    Camera::Ptr camera;
    
    MainScene()
    {
    }
    
    void init(LightNode::Ptr light, GeoNode::Ptr box)
    {
        ImageSampler::Ptr tex = ImageSampler::create(512,512,4,Vec4(0.0,0.0,0.0,1.0));
        setTexture(tex);
        
        
        Graph::Ptr graph3d = Graph::create();
        Node::Ptr root = Node::create();

        camera = Camera::create(Vec3(0.0,1.0,0.0));
        camera->setPosition(Vec3(0.0f, 0.0f, 0.0f));
        camera->setTarget(Vec3(0.0f, -5.0f, -10.0f));
        
        root->addNode(box);
        root->addNode(camera);
        root->addNode(light);
        graph3d->setRoot(root);
        
        graph3d->getInitState()->setViewport( Vec4(0.0,0.0,512.0,512.0) );
        graph3d->getInitState()->setPerspectiveProjection(FOV, 1.0,NEAR, 1024.0);
        graph3d->setClearColor(Vec4(1.0,0.0,0.0,1.0));
        setScene(graph3d);
        
        
        
        
        Material::Ptr material = Material::create(Vec3(0.0), 32.0);
        material->_baseColor = texture();
        material->_emissive = texture();
        
        QuadNode::Ptr displayquad = QuadNode::create();
        displayquad->setMaterial(material);
        
        ReflexionModel::Ptr displayshader = ReflexionModel::create(
                ReflexionModel::Lighting_None,
                ReflexionModel::Texturing_Samplers_CNE,
                ReflexionModel::MRT_1_Col, "glsl texture display");
                
        // displayshader->_vertCode = glsl_billboard_vert;
        displayquad->setReflexion(displayshader);
        addNode(displayquad);
        
        
        
        
        
        
        makeDirty();
    }
    
    void updateFov(Vec3 cam, float fov, float near = NEAR)
    {
        camera->setPosition(cam);
        scene()->getInitState()->setPerspectiveProjection(fov, 1.0,near, 1024.0);
        
        makeDirty();
    }
};

int main(int argc, char* argv[])
{
    RenderModeManager renderModeMngr;
    renderModeMngr.setArgs(argc, argv);

    sf::Clock clock;

    sf::RenderWindow window(sf::VideoMode(512, 512), "FOV test", sf::Style::Default, sf::ContextSettings(24));
    window.setFramerateLimit(60);

    Graph::Ptr graph3d = Graph::create();
    Node::Ptr root = Node::create();

    imp::ReflexionModel::Ptr shader = imp::ReflexionModel::create(
            imp::ReflexionModel::Lighting_Phong,
            imp::ReflexionModel::Texturing_Samplers_CNE,
            imp::ReflexionModel::MRT_1_Col,
            "glsl for scene object");

    ImageSampler::Ptr blue = ImageSampler::create(8,8,3,Vec4(0.0,0.0,1.0,1.0));

    Material::Ptr case_material = Material::create(Vec3(1.0), 32.0);
    case_material->_baseColor = blue;
    // case_material->_emissive = blue;
    
    GeoNode::Ptr box = GeoNode::create(generateBox(), false);
    box->setScale(Vec3(4.0,1.0,1.0));
    box->setPosition(Vec3(0.0, -5.0, -10.0));
    box->setRotation(Vec3(0.0, 3.14*0.25, 0.0));
    box->setMaterial(case_material);
    box->setReflexion(shader);
    
    LightNode::Ptr light = LightNode::create();
    light->setPosition( Vec3(10.0,10.0,10.0) );
    graph3d->setRoot(root);

    SceneRenderer::Ptr renderer = renderModeMngr.loadRenderer();
    renderer->enableFeature(SceneRenderer::Feature_Shadow, false);
    renderer->enableFeature(SceneRenderer::Feature_Environment, false);
    renderer->enableFeature(SceneRenderer::Feature_Bloom, false);
    renderer->enableFeature(SceneRenderer::Feature_SSAO, false);
    renderer->enableFeature(SceneRenderer::Feature_Phong, true);
    graph3d->getInitState()->setViewport( renderModeMngr.viewport );
    graph3d->getInitState()->setOrthographicProjection(-1.f, 1.f, -1.f, 1.f, 0.f, 1.f);
    graph3d->setClearColor(Vec4(1.0,0.0,0.0,1.0));

    renderer->setOutputFrame(SceneRenderer::RenderFrame_Default);
    // renderer->setOutputFrame(SceneRenderer::RenderFrame_Color);
    
    MiniMap::Ptr minimap = MiniMap::create();
    minimap->init(light,box);
    
    MainScene::Ptr mainscene = MainScene::create();
    mainscene->init(light,box);
    
    root->addNode(mainscene);
    root->addNode(minimap);
    
    bool fuse = true;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed || event.type == sf::Event::KeyPressed)
                window.close();
        }
        if(!window.isOpen()) break;

        float elapsed = clock.getElapsedTime().asSeconds();
        
        float fct = std::sin(elapsed)*0.5+0.5;
       float fov = fct * (90.0 - 30.0) + 30.0;
       float near = 0.1;
       Vec3 p0(0.0f, 0.0f, 0.0f);
       Vec3 p1(0.0f, 10.0f, 20.0f);
       Vec3 cam = (p1-p0)*(1.0-fct) + p0;
       // float fov = 90.0;
       // float near = fct * (12.0 - 0.1) + 0.1;
        
        // if(fuse)
        {            
            minimap->updateFov(cam,fov,near);
            mainscene->updateFov(cam,fov,near);
        }
        fuse = !fuse;

        // window.clear();
        renderer->render( graph3d );

        window.display();
        
    }


    exit(EXIT_SUCCESS);
}
