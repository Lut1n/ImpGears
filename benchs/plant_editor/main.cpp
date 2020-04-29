#define USE_GUI
// #include "stb_sprintf.h"

#ifdef USE_GUI
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#endif

#include <GL/glew.h>
// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

#include <ImpGears/Graphics/ImageOperation.h>
#include <ImpGears/SceneGraph/Graph.h>
#include <ImpGears/SceneGraph/Camera.h>
#include <ImpGears/SceneGraph/GeoNode.h>
#include <ImpGears/SceneGraph/QuadNode.h>
#include <ImpGears/SceneGraph/RenderPass.h>
#include <ImpGears/Renderer/CpuRenderer.h>
#include <ImpGears/Plugins/RenderPlugin.h>
#include <ImpGears/Descriptors/ImageIO.h>
#include <ImpGears/Descriptors/FileInfo.h>
#include <ImpGears/Descriptors/JsonImageOp.h>
#include <ImpGears/Geometry/InstancedGeometry.h>

using namespace imp;

// common stuff
// #include "../common/inc_experimental.h"

// #include <filesystem>

#define IMPLEMENT_RENDER_MODE_MANAGER
#include "../common/RenderModeManager_glfw.h"
#define IMPLEMENT_BASIC_GEOMETRIES
#include "../common/basic_geometries.h"

// #include "../common/basic_skybox.h"
// #include "proc_skybox.h"
#include "render_skybox.h"


#include "vegetal.h"
#include "rock.h"



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
out mat4 v_view;

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
    v_view = u_view;
}

);

static std::string waterVert = GLSL_CODE(

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
uniform float u_time;

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
    vec4 vert = vec4(a_vertex,1.0);
    vec4 mvert = u_model * vert;
    float oft = mvert.x+mvert.z;
    vert += vec4(0.0,1.0,0.0,0.0) * (sin(oft*10.0+u_time*2.0)*0.5+0.5) * 0.5;
    vec4 mv_pos = u_view * u_model * vert;
    gl_Position = u_proj * mv_pos;
    v_n = normalize(u_normal * a_normal);
    v_color = a_color;

    v_texCoord = a_texcoord;
    v_m = (u_model * vert).xyz;
    v_mv = mv_pos.xyz;
    v_vertex = vert.xyz;
    v_model = u_model;
}

);




static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

// -----------------------------------------------------------------------------------------------------------------------
void loadSamplers(ImageSampler::Ptr& sampler, ImageSampler::Ptr& color, ImageSampler::Ptr& normal)
{
    if( !fileExists("./cache/scene_terrain.tga") || !fileExists("./cache/scene_color.tga") || !fileExists("./cache/scene_normal.tga"))
    {
        // std::filesystem::create_directories("cache");
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

    normal =ImageSampler::create();
    normal->setSource( ImageIO::load("./cache/scene_normal.tga") );
    normal->setWrapping(ImageSampler::Wrapping_Repeat);
    normal->setFiltering(ImageSampler::Filtering_Linear);
    normal->_mipmap = true;
}

GeoNode::Ptr buildGeoNode(Geometry::Ptr geo, Material::Ptr mat, ReflexionModel::Ptr refl, bool wireframe)
{
    GeoNode::Ptr node = GeoNode::create(geo, wireframe);
    node->setMaterial(mat);
    node->setReflexion(refl);
    node->getState()->getRenderPass()->enablePass(RenderPass::Pass_EnvironmentMapping);
    node->getState()->getRenderPass()->enablePass(RenderPass::Pass_ShadowMapping);

    return node;
}

IMPGEARS_BEGIN
class WaterNode : public GeoNode
{
    public:
    Meta_Class(WaterNode)

    WaterNode(const Polyhedron& buf, bool wireframe = false) : GeoNode(buf,wireframe) {}
    WaterNode(const Geometry::Ptr& geo, bool wireframe = false) : GeoNode(geo,wireframe) {}
};
class GroundNode : public GeoNode
{
    public:
    Meta_Class(GroundNode)

    GroundNode(const Polyhedron& buf, bool wireframe = false) : GeoNode(buf,wireframe) {}
    GroundNode(const Geometry::Ptr& geo, bool wireframe = false) : GeoNode(geo,wireframe) {}
};
class CoordNode : public GeoNode
{
    public:
    Meta_Class(CoordNode)

    CoordNode(const Polyhedron& buf, bool wireframe = false) : GeoNode(buf,wireframe) {}
    CoordNode(const Geometry::Ptr& geo, bool wireframe = false) : GeoNode(geo,wireframe) {}
};
class BallNode : public GeoNode
{
    public:
    Meta_Class(BallNode)

    BallNode(const Polyhedron& buf, bool wireframe = false) : GeoNode(buf,wireframe) {}
    BallNode(const Geometry::Ptr& geo, bool wireframe = false) : GeoNode(geo,wireframe) {}
};
IMPGEARS_END

int main(int argc, char* argv[])
{
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);


    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "ImpGears + DearImGui", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    bool err = glewInit() != GLEW_OK;
    if (err)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }


    RenderModeManager renderModeMngr;
    renderModeMngr.setArgs(argc, argv);

    ImageSampler::Ptr sampler, color, emi, normals;
    loadSamplers(sampler,color,normals);
    emi = ImageSampler::create(8, 8, 4, Vec4(1.0));

    ImageSampler::Ptr reflectivityMap = ImageSampler::create(8,8,3,Vec4(1.0));

    // normals = ImageSampler::create(128.0,128.0,4,Vec4(0.0,0.0,1.0,1.0));
    // normals->setWrapping(ImageSampler::Wrapping_Repeat);
    // normals->setFiltering(ImageSampler::Filtering_Linear);
    // BumpToNormalOperation op;
    // op.setTarget(sampler->getSource());
    // op.execute(normals->getSource());
    
    ImageSampler::Ptr halfOpaque = ImageSampler::create(8,8,4,Vec4(1.0,1.0,1.0,0.5));

    // impgears stuff
    Graph::Ptr graph = Graph::create();
    Node::Ptr root = Node::create();

    Camera::Ptr camera = Camera::create(Vec3(0.0,1.0,0.0));
    camera->setPosition(Vec3(10.0f, 10.0f, 10.0f));
    camera->setTarget(Vec3(0.0f, 0.0f, 0.0f));
    
    imp::ReflexionModel::Ptr instancedShader = imp::ReflexionModel::create(
            imp::ReflexionModel::Lighting_Phong,
            imp::ReflexionModel::Texturing_Samplers_CNE,
            imp::ReflexionModel::MRT_2_Col_Emi, "glsl instanced tree");
    instancedShader->_vertCode = glsl_instancing_vert;
            

    imp::ReflexionModel::Ptr reflection_scene = imp::ReflexionModel::create(
            imp::ReflexionModel::Lighting_Phong,
            imp::ReflexionModel::Texturing_Samplers_CNE,
            imp::ReflexionModel::MRT_2_Col_Emi, "scene reflection");

    imp::ReflexionModel::Ptr reflection_water = imp::ReflexionModel::create(
            imp::ReflexionModel::Lighting_Phong,
            imp::ReflexionModel::Texturing_Samplers_CNE,
            imp::ReflexionModel::MRT_2_Col_Emi, "water reflection");
    reflection_water->_vertCode = waterVert;


    Material::Ptr material_tree = Material::create(Vec3(0.5,0.4,0.1), 32.0);
    material_tree->_normalmap = normals;
    
    Material::Ptr material_leaf = Material::create(Vec3(0.3,1.0,0.4), 32.0);

    Material::Ptr material_rock = Material::create(Vec3(0.5,0.7,0.5), 16.0);
    material_rock->_reflectivity = color;

    Material::Ptr material_ground = Material::create(Vec3(0.3,1.0,0.4), 64.0);
    material_ground->_normalmap = normals;
    material_ground->_baseColor = color;

    Material::Ptr material_water = Material::create(Vec3(0.1,0.4,1.0), 16.0);
    material_water->_baseColor = halfOpaque;
    material_water->_normalmap = normals;
    material_water->_reflectivity = reflectivityMap;

    Material::Ptr light_material = Material::create(Vec3(1.0), 32.0);
    light_material->_emissive = emi;

    TreeDescriptor dsc, dscEdited, grassDesc;
    RockDescriptor rdsc;
    PlantGenerator gen;
    RockGenerator rgen;

    // Geometry::Ptr treeGeometry = gen.generateTreeGeo(dsc);
    // Node::Ptr treeNode = gen.buildTreeNode(dsc, material_tree, reflection_scene);
    Node::Ptr treeNode = gen.buildInstancedTree(dsc, material_tree, instancedShader);
    Node::Ptr leafNode = gen.buildInstancedLeaf(dsc, material_leaf, instancedShader);
    GeoNode::Ptr rockNode = rgen.generateRock(rdsc, material_rock, reflection_scene, false);


    Geometry::Ptr geometry_terrain = Geometry::create();
    *geometry_terrain = Geometry::cube();
    *geometry_terrain = geometry_terrain->subdivise(16);
    Geometry::intoCCW(*geometry_terrain);
    geometry_terrain->generateColors(Vec3(1.0));
    geometry_terrain->generateTexCoords( Geometry::TexGenMode_Cubic, 16.0);
    geometry_terrain->sphericalNormalization(0.8);
    geometry_terrain->noiseBump(8, 0.7, 2.0, 0.3);
    geometry_terrain->generateNormals( Geometry::NormalGenMode_PerFace );
    geometry_terrain->interpolateNormals();
    GeoNode::Ptr terrainNode = GroundNode::create(geometry_terrain, false);
    terrainNode->setReflexion(reflection_scene);
    terrainNode->setMaterial(material_ground);
    terrainNode->setScale( Vec3(20.0,10.0,20.0) );
    terrainNode->setPosition( Vec3(0.0,-10.0,0.0) );

    grassDesc._divisionCount = 1;
    grassDesc._leafCount = 0;
    grassDesc._nodeCount = 3;
    grassDesc._baseRadius = 0.1;
    grassDesc._baseHeight = 1.0;
    grassDesc._theta = 0.2;
    Node::Ptr grassNode = gen.generateGrass(grassDesc, material_tree, instancedShader, false, geometry_terrain, Vec3(20.0,10.0,20.0), Vec3(0.0,-10.0,0.0));


    Geometry::Ptr geometry_water = Geometry::create();
    *geometry_water = Geometry::cube();
    *geometry_water = geometry_water->subdivise(16);
    Geometry::intoCCW(*geometry_water);
    geometry_water->generateColors(Vec3(1.0));
    geometry_water->generateTexCoords( Geometry::TexGenMode_Cubic, 50.0);
    geometry_water->noiseBump(8, 0.7, 2.0, 0.3);
    geometry_water->generateNormals( Geometry::NormalGenMode_PerFace );
    geometry_water->interpolateNormals();
    GeoNode::Ptr waterNode = WaterNode::create(geometry_water, false);
    waterNode->setReflexion(reflection_water);
    waterNode->setMaterial(material_water);
    waterNode->setScale( Vec3(50.0,1.0,50.0) );
    waterNode->setPosition( Vec3(0.0,-4.0,0.0) );



    Geometry::Ptr coords = buildCoord();
    coords->scale(Vec3(30.0));
    GeoNode::Ptr coordsNode = CoordNode::create(coords, false);
    coordsNode->setReflexion(reflection_scene);
    coordsNode->setMaterial(light_material);
    coordsNode->setPosition(Vec3(0.0,15.0,0.0));

    LightNode::Ptr light = LightNode::create(Vec3(1.0),10.f);

    Geometry::Ptr pointGeo = Geometry::create();
    *pointGeo = Geometry::sphere(16, 0.4);
    pointGeo->setPrimitive(Geometry::Primitive_Triangles);
    Geometry::intoCCW(*pointGeo, true);
    pointGeo->generateColors(Vec3(1.0,1.0,1.0));
    pointGeo->generateNormals();
    pointGeo->generateTexCoords(Geometry::TexGenMode_Spheric);
    pointGeo->interpolateNormals();

    GeoNode::Ptr pointNode = BallNode::create(pointGeo, false);
    pointNode->setPosition(Vec3(0.0,2,0.0));
    pointNode->setReflexion(reflection_scene);
    pointNode->setMaterial(light_material);

    GeoNode::Ptr test = GeoNode::create(pointGeo, false);
    test->setPosition(Vec3(0.0,0.0,0.0));
    test->setScale(Vec3(10.0));
    test->setReflexion(reflection_scene);
    test->setMaterial(material_rock);

    SkyBox::Ptr sky = SkyBox::create();

    root->addNode(sky);
    root->addNode(light);
    root->addNode(camera);


    // GeoNode::Ptr tree_01 = buildGeoNode(treeGeometry, material_tree, reflection_scene, false);
    // GeoNode::Ptr tree_02 = buildGeoNode(treeGeometry, material_tree, reflection_scene, false);
    // GeoNode::Ptr tree_03 = buildGeoNode(treeGeometry, material_tree, reflection_scene, false);
    // tree_01->setPosition(Vec3(10, 0.0, 15.0));
    // tree_02->setPosition(Vec3(-15, 0.0, 5.0));
    // tree_03->setPosition(Vec3(7, 0.0, -10.0));
    // root->addNode(tree_01);
    // root->addNode(tree_02);
    // root->addNode(tree_03);

    root->addNode(treeNode);
    root->addNode(leafNode);
    root->addNode(rockNode);
    root->addNode(terrainNode);
    root->addNode(coordsNode);
    root->addNode(pointNode);
    root->addNode(grassNode);
    root->addNode(waterNode);

    // root->addNode(test);
    graph->setRoot(root);

    // RenderPass::Ptr rp_info = RenderPass::create();
    // rp_info->enablePass(RenderPass::Pass_EnvironmentMapping);
    // RenderPass::Ptr rp_info2 = RenderPass::create();
    // rp_info2->enablePass(RenderPass::Pass_ShadowMapping);
    // coordsNode->setRenderPass(rp_info);
    // sky->setRenderPass(rp_info);
    // pointNode->setRenderPass(rp_info);

    RenderPass::Ptr env_sha = RenderPass::create();
    RenderPass::Ptr env = RenderPass::create();
    RenderPass::Ptr sha = RenderPass::create();
    
    env->enablePass(RenderPass::Pass_EnvironmentMapping);
    env_sha->enablePass(RenderPass::Pass_EnvironmentMapping);
    env_sha->enablePass(RenderPass::Pass_ShadowMapping);
    sha->enablePass(RenderPass::Pass_ShadowMapping);
    
    coordsNode->getState()->setRenderPass(env_sha);
    terrainNode->getState()->setRenderPass(env_sha);
    rockNode->getState()->setRenderPass(env_sha);
    grassNode->getState()->setRenderPass(env_sha);
    treeNode->getState()->setRenderPass(env_sha);
    leafNode->getState()->setRenderPass(env_sha);
    
    pointNode->getState()->setRenderPass(env);
    sky->getState()->setRenderPass(env);


    SceneRenderer::Ptr renderer = renderModeMngr.loadRenderer();
    renderer->enableFeature(SceneRenderer::Feature_Shadow, true);
    renderer->enableFeature(SceneRenderer::Feature_Environment, true);
    renderer->enableFeature(SceneRenderer::Feature_Bloom, true);
    renderer->enableFeature(SceneRenderer::Feature_SSAO, true);
    renderer->setDirect(true);
    graph->getInitState()->setViewport( renderModeMngr.viewport );
    graph->setClearColor(Vec4(0.0,0.0,1.0,1.0));

    renderer->setOutputFrame(SceneRenderer::RenderFrame_Default);
    Image::Ptr target;

#ifdef USE_GUI
    bool use_imgui = true;
#else
    bool use_imgui = false;
#endif
    
    
    #ifdef USE_GUI
    if(use_imgui)
    {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();

        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);
    }
    #endif
    
    // Our state
    Vec3 obj_position = Vec3(0.0);
    Vec3 obj_rotation = Vec3(0.0);
    Vec3 obj_scale = Vec3(1.0);
    Vec4 material_color = Vec4(1.0);
    float shininess = 32.0;
    float camera_rad = 0.0;
    float camera_rad_y = -0.07;
    float camera_dst = 35.0;

    // for each timer keep variable after how many seconds to trigger it
    float time = 2.f; // two seconds
    float previous = glfwGetTime();

    bool shadowsEnable = true;
    bool environmentEnable = true;
    bool bloomEnable = true;
    bool ssaoEnable = true;
    bool phongEnable = true;
    bool wireframe = false;

    Vec4 viewport(0.0,0.0,512.0,512.0);
    

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        // common part, do this only once
        float now = glfwGetTime();

    #ifdef USE_GUI
        if(use_imgui)
        {
            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::Begin("Context");
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::Checkbox("Enable phong", &phongEnable);
            ImGui::Checkbox("Enable shadows", &shadowsEnable);
            ImGui::Checkbox("Enable environment", &environmentEnable);
            ImGui::Checkbox("Enable bloom", &bloomEnable);
            ImGui::Checkbox("Enable ambient occlusion", &ssaoEnable);

            const char* items[] = { "Default", "Lighting", "Emissive", "Normals", "Shadows", "Environment", "Bloom", "Color", "Reflectivity", "Depth", "SSAO"};
            static int item_current = 0;
            ImGui::Combo("Output frame", &item_current, items, IM_ARRAYSIZE(items));

            if(item_current == 0)
                renderer->setOutputFrame(SceneRenderer::RenderFrame_Default);
            else if(item_current == 1)
                renderer->setOutputFrame(SceneRenderer::RenderFrame_Lighting);
            else if(item_current == 2)
                renderer->setOutputFrame(SceneRenderer::RenderFrame_Emissive);
            else if(item_current == 3)
                renderer->setOutputFrame(SceneRenderer::RenderFrame_Normals);
            else if(item_current == 4)
                renderer->setOutputFrame(SceneRenderer::RenderFrame_ShadowMap);
            else if(item_current == 5)
                renderer->setOutputFrame(SceneRenderer::RenderFrame_Environment);
            else if(item_current == 6)
                renderer->setOutputFrame(SceneRenderer::RenderFrame_Bloom);
            else if(item_current == 7)
                renderer->setOutputFrame(SceneRenderer::RenderFrame_Color);
            else if(item_current == 8)
                renderer->setOutputFrame(SceneRenderer::RenderFrame_Reflectivity);
            else if(item_current == 9)
                renderer->setOutputFrame(SceneRenderer::RenderFrame_Depth);
            else if(item_current == 10)
                renderer->setOutputFrame(SceneRenderer::RenderFrame_SSAO);

            ImGui::End();

            ImGui::Begin("Scene");
            ImGui::SliderFloat("Camera long", &camera_rad, -1.0f, 1.0f);
            ImGui::SliderFloat("Camera lat", &camera_rad_y, -1.0f, 1.0f);
            ImGui::SliderFloat("Camera dist", &camera_dst, 1.0f, 100.0f);
            ImGui::SliderFloat3("Model position", obj_position.data(),-10.0,10.0);
            ImGui::SliderFloat3("Model rotation", obj_rotation.data(),-1.0,1.0);
            ImGui::SliderFloat3("Model scale", obj_scale.data(),0.1,10.0);
            ImGui::End();


            ImGui::Begin("Material");
            ImGui::ColorEdit3("Color", material_color.data());
            ImGui::SliderFloat("Shininess", &shininess, 0.0f, 100.0f);
            ImGui::End();


            ImGui::Begin("Model");
            ImGui::Checkbox("Wireframe", &wireframe);
            ImGui::SliderFloat("base radius", &dscEdited._baseRadius, 0.1, 10.0);
            ImGui::SliderFloat("base height", &dscEdited._baseHeight, 0.1, 10.0);
            ImGui::SliderInt("node count", &dscEdited._nodeCount, 1, 7);
            ImGui::SliderInt("division count", &dscEdited._divisionCount, 1, 7);
            ImGui::SliderFloat("tilt", &dscEdited._theta, 0.0, 3.0);
            ImGui::SliderFloat("radius factor", &dscEdited._radiusFactor, 0.1, 1.0);
            ImGui::SliderFloat("height factor", &dscEdited._heightFactor, 0.1, 1.0);
            ImGui::SliderFloat("leaf size", &dscEdited._leafSize, 0.1, 10.0);
            ImGui::SliderInt("leaf count", &dscEdited._leafCount, 0, 7);
            ImGui::ColorEdit3("leaf color", dscEdited._leafColor.data());
            if(ImGui::Button("Apply"))
            {
                dsc = dscEdited;
                root->remNode(treeNode);
                root->remNode(leafNode);
                treeNode = gen.buildInstancedTree(dsc, material_tree, instancedShader);
                leafNode = gen.buildInstancedLeaf(dsc, material_leaf, instancedShader);
                treeNode->getState()->setRenderPass(env_sha);
                leafNode->getState()->setRenderPass(env_sha);
                root->addNode(treeNode);
                root->addNode(leafNode);
            }
            if(ImGui::Button("Revert"))
            {
                dscEdited = dsc;
            }
            ImGui::End();
        }
        #endif

        // impgears stuff
        const float PI = 3.141592;
        double t = now;
        Vec3 lp(cos(t)*10.0,14.0,sin(t)*10.0);
        // Vec3 lp(cos(t)*10.0,0.0,sin(t)*10.0);
        light->setPosition(lp);
        pointNode->setPosition(lp);
        Vec3 lp2(cos(camera_rad*PI ),sin(camera_rad_y*PI*0.5),sin(camera_rad*PI));
        camera->setPosition(Vec3(0.0,dsc._baseHeight,0.0) + lp2*Vec3(camera_dst));
        camera->setTarget(Vec3(0.0f, dsc._baseHeight, 0.0f));
        // camera->setTarget(Vec3(0.0));
        treeNode->setPosition( obj_position );
        treeNode->setRotation( obj_rotation * PI );
        treeNode->setScale( obj_scale );
        material_tree->_color = material_color;
        material_tree->_shininess = shininess;

        waterNode->getState()->setUniform("u_time", (float)t);

        renderer->enableFeature(SceneRenderer::Feature_Phong, phongEnable);
        renderer->enableFeature(SceneRenderer::Feature_Shadow, shadowsEnable);
        renderer->enableFeature(SceneRenderer::Feature_Environment, environmentEnable);
        renderer->enableFeature(SceneRenderer::Feature_Bloom, bloomEnable);
        renderer->enableFeature(SceneRenderer::Feature_SSAO, ssaoEnable);


        // Rendering
        #ifdef USE_GUI
        if(use_imgui) ImGui::Render();
        #endif
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        if(display_w != (int)viewport[2] || display_h != viewport[3])
        {
            viewport = Vec4(0.0,0.0,display_w,display_h);
            float mn = std::min(display_w,display_h);
            float oft = display_w*0.5 - mn*0.5;
            Vec4 effective_vp(oft,0.0,mn,mn);
            renderer->setOuputViewport(effective_vp);
        }

        // impgears
        renderer->render( graph );
        // imgui
        glViewport(0, 0, display_w, display_h);
        #ifdef USE_GUI
        if(use_imgui) ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        #endif
        glfwSwapBuffers(window);
    }

    // Cleanup
    #ifdef USE_GUI
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    #endif

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;

}
