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
#include <ImpGears/Plugins/RenderPlugin.h>
#include <ImpGears/Descriptors/ImageIO.h>
#include <ImpGears/Descriptors/FileInfo.h>
#include <ImpGears/Descriptors/JsonImageOp.h>

using namespace imp;

#define IMPLEMENT_BASIC_GEOMETRIES
#include "../common/basic_geometries.h"

#include "render_skybox.h"


static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}


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


    
    std::string pluginName = "OGLPlugin";
    pluginName = "lib" + pluginName + "." + LIB_EXT;
    SceneRenderer::Ptr renderer;
    RenderPlugin::Ptr plugin = PluginManager::open( pluginName );
    renderer = plugin->getRenderer();
    
    // ImageSampler::Ptr halfOpaque = ImageSampler::create(8,8,4,Vec4(1.0,1.0,1.0,0.5));
    ImageSampler::Ptr emi = ImageSampler::create(8,8,3,Vec3(1.0,1.0,1.0));
    emi->setWrapping(ImageSampler::Wrapping_Repeat);
    emi->setFiltering(ImageSampler::Filtering_Linear);
    emi->_mipmap = true;
    ImageSampler::Ptr refl = ImageSampler::create(8,8,3,Vec3(0.5));
    refl->setWrapping(ImageSampler::Wrapping_Repeat);
    refl->setFiltering(ImageSampler::Filtering_Linear);
    refl->_mipmap = true;

    // impgears stuff
    Graph::Ptr graph = Graph::create();
    Node::Ptr root = Node::create();

    Camera::Ptr camera = Camera::create(Vec3(0.0,1.0,0.0));
    camera->setPosition(Vec3(10.0f, 10.0f, 10.0f));
    camera->setTarget(Vec3(0.0f, 0.0f, 0.0f));
            

    imp::ReflexionModel::Ptr reflection_scene = imp::ReflexionModel::create(
            imp::ReflexionModel::Lighting_Phong,
            imp::ReflexionModel::Texturing_Samplers_CNE,
            imp::ReflexionModel::MRT_2_Col_Emi, "scene reflection");

    Material::Ptr material_light = Material::create(Vec3(1.0), 64.0);
    material_light->_emissive = emi;
    
    Material::Ptr material_cloud = Material::create(Vec4(1.0,1.0,1.0,0.3), 64.0);
    
    Material::Ptr material_water = Material::create(Vec4(0.3,0.5,1.0,0.3), 64.0);
    material_water->_reflectivity = refl;


    ImageSampler::Ptr hashmap = ImageSampler::create(128,128,3,Vec4(1.0));
    ImageSampler::Ptr noisemap = ImageSampler::create(128,128,3,Vec4(1.0));
    noisemap->setWrapping(ImageSampler::Wrapping_Repeat);
    noisemap->setFiltering(ImageSampler::Filtering_Linear);
    noisemap->_mipmap = true;
    
    HashOperation hashOp;
    hashOp.setSeed(14214.62354123);
    hashOp.execute(hashmap->getSource());
    
    FbmOperation fbmOp;
    
    fbmOp.setHashmap(hashmap->getSource());
    fbmOp.setFreq(4.0);
    fbmOp.setType(0);
    fbmOp.setOctaves(4);
    fbmOp.setPersist(0.7);
    
    fbmOp.execute(noisemap->getSource());
    
    material_cloud->_baseColor = noisemap;
    

    Geometry::Ptr geometry_sky = Geometry::create();
    *geometry_sky = Geometry::sphere(8,512.0);
    // *geometry_sky = geometry_water->subdivise(16);
    Geometry::intoCCW(*geometry_sky, false);
    geometry_sky->generateColors(Vec3(0.7,0.7,0.8));
    // geometry_sky->generateTexCoords( Geometry::TexGenMode_Cubic, 50.0);
    // geometry_sky->sphericalNormalization(1.0);
    // geometry_water->noiseBump(8, 0.7, 2.0, 0.3);
    geometry_sky->generateNormals( Geometry::NormalGenMode_PerFace );
    geometry_sky->interpolateNormals();
    GeoNode::Ptr skyNode = GeoNode::create(geometry_sky, false);
    skyNode->setReflexion(reflection_scene);
    // skyNode->setScale( Vec3(50.0,50.0,50.0) );
    skyNode->setPosition( Vec3(0.0,0.0,0.0) );


    Geometry::Ptr geometry_terrain = Geometry::create();
    *geometry_terrain = Geometry::cube();
    *geometry_terrain = geometry_terrain->subdivise(24);
    Geometry::intoCCW(*geometry_terrain);
    geometry_terrain->generateColors(Vec3(0.5,1.0,0.3));
    geometry_terrain->generateTexCoords( Geometry::TexGenMode_Cubic, 16.0);
    geometry_terrain->sphericalNormalization(1.0);
    geometry_terrain->noiseBump(8, 0.4, 4.0, 0.3);
    geometry_terrain->generateNormals( Geometry::NormalGenMode_PerFace );
    geometry_terrain->interpolateNormals();
    geometry_terrain->scale(10.0);
    GeoNode::Ptr terrainNode = GeoNode::create(geometry_terrain, false);
    terrainNode->setReflexion(reflection_scene);
    // terrainNode->setScale( Vec3(10.0,10.0,10.0) );                                           // /!\ scale breaks normals (need to be fixed)
    terrainNode->setPosition( Vec3(0.0,0.0,0.0) );


    Geometry::Ptr geometry_water = Geometry::create();
    *geometry_water = Geometry::sphere(8,10.0);
    // *geometry_water = geometry_water->subdivise(16);
    // geometry_water->sphericalNormalization(1.0);
    Geometry::intoCCW(*geometry_water);
    geometry_water->generateColors(Vec3(1.0));
    geometry_water->generateTexCoords( Geometry::TexGenMode_Spheric, 1.0);
    // geometry_water->noiseBump(8, 0.7, 2.0, 0.3);
    geometry_water->generateNormals( Geometry::NormalGenMode_PerFace );
    geometry_water->interpolateNormals();
    GeoNode::Ptr waterNode = GeoNode::create(geometry_water, false);
    waterNode->setReflexion(reflection_scene);
    waterNode->setMaterial(material_water);
    waterNode->getState()->setTransparent(true);
    // waterNode->setScale( Vec3(10.0,10.0,10.0) );
    // waterNode->setPosition( Vec3(0.0,0.0,0.0) );
    
    Geometry::Ptr geometry_cloud = Geometry::create();
    *geometry_cloud = Geometry::cube();
    *geometry_cloud = geometry_cloud->subdivise(16);
    Geometry::intoCCW(*geometry_cloud);
    geometry_cloud->generateColors(Vec3(1.0,1.0,1.0));
    geometry_cloud->generateTexCoords( Geometry::TexGenMode_Cubic, 1.0);
    geometry_cloud->sphericalNormalization(1.0);
    // geometry_cloud->noiseBump(3, 0.5, 4.0, 0.1);
    geometry_cloud->generateNormals( Geometry::NormalGenMode_PerFace );
    geometry_cloud->interpolateNormals();
    GeoNode::Ptr cloudNode = GeoNode::create(geometry_cloud, false);
    cloudNode->setReflexion(reflection_scene);
    cloudNode->setScale( Vec3(12.0,12.0,12.0) );
    cloudNode->setPosition( Vec3(0.0,0.0,0.0) );
    cloudNode->setMaterial(material_cloud);
    cloudNode->getState()->setTransparent(true);



    Geometry::Ptr coords = buildCoord();
    coords->scale(Vec3(30.0));
    GeoNode::Ptr coordsNode = GeoNode::create(coords, false);
    coordsNode->setReflexion(reflection_scene);
    coordsNode->setPosition(Vec3(0.0,15.0,0.0));

    LightNode::Ptr light = LightNode::create(Vec3(1.0),10.f);

    Geometry::Ptr pointGeo = Geometry::create();
    *pointGeo = Geometry::sphere(16, 1.0);
    pointGeo->setPrimitive(Geometry::Primitive_Triangles);
    Geometry::intoCCW(*pointGeo, true);
    pointGeo->generateColors(Vec3(1.0,1.0,1.0));
    pointGeo->generateNormals();
    pointGeo->generateTexCoords(Geometry::TexGenMode_Spheric);
    pointGeo->interpolateNormals();

    GeoNode::Ptr pointNode = GeoNode::create(pointGeo, false);
    pointNode->setPosition(Vec3(0.0,0,0.0));
    pointNode->setReflexion(reflection_scene);
    pointNode->setMaterial(material_light);
    pointNode->setScale(Vec3(5.0));
    
    SkyBox::Ptr sky = SkyBox::create();

    root->addNode(light);
    root->addNode(camera);

    root->addNode(terrainNode);
    root->addNode(coordsNode);
    root->addNode(pointNode);
    root->addNode(waterNode);
    // root->addNode(cloudNode);
    
    root->addNode(sky);

    graph->setRoot(root);

    RenderPass::Ptr env_sha = RenderPass::create();
    RenderPass::Ptr env = RenderPass::create();
    RenderPass::Ptr sha = RenderPass::create();
    
    env->enablePass(RenderPass::Pass_EnvironmentMapping);
    env_sha->enablePass(RenderPass::Pass_EnvironmentMapping);
    env_sha->enablePass(RenderPass::Pass_ShadowMapping);
    sha->enablePass(RenderPass::Pass_ShadowMapping);
    
    coordsNode->getState()->setRenderPass(env_sha);
    terrainNode->getState()->setRenderPass(env_sha);
    waterNode->getState()->setRenderPass(env_sha);
    skyNode->getState()->setRenderPass(env_sha);
    
    pointNode->getState()->setRenderPass(env);
    sky->getState()->setRenderPass(env);


    // SceneRenderer::Ptr renderer = renderModeMngr.loadRenderer();
    renderer->enableFeature(SceneRenderer::Feature_Shadow, true);
    renderer->enableFeature(SceneRenderer::Feature_Environment, true);
    renderer->enableFeature(SceneRenderer::Feature_Bloom, true);
    renderer->enableFeature(SceneRenderer::Feature_SSAO, true);
    renderer->setDirect(true);
    graph->getInitState()->setViewport( Vec4(0.0,0.0,1024.0,1024.0) );
    graph->setClearColor(Vec4(0.0,0.0,1.0,1.0));

    renderer->setOutputFrame(SceneRenderer::RenderFrame_Default);

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


            // ImGui::Begin("Model");
            //ImGui::Checkbox("Wireframe", &wireframe);
            //ImGui::SliderFloat("base radius", &dscEdited._baseRadius, 0.1, 10.0);
            //ImGui::SliderFloat("base height", &dscEdited._baseHeight, 0.1, 10.0);
            //ImGui::SliderInt("node count", &dscEdited._nodeCount, 1, 7);
            //ImGui::SliderInt("division count", &dscEdited._divisionCount, 1, 7);
            //ImGui::SliderFloat("tilt", &dscEdited._theta, 0.0, 3.0);
            //ImGui::SliderFloat("radius factor", &dscEdited._radiusFactor, 0.1, 1.0);
            //ImGui::SliderFloat("height factor", &dscEdited._heightFactor, 0.1, 1.0);
            //ImGui::SliderFloat("leaf size", &dscEdited._leafSize, 0.1, 10.0);
            //ImGui::SliderInt("leaf count", &dscEdited._leafCount, 0, 7);
            //ImGui::ColorEdit3("leaf color", dscEdited._leafColor.data());
            //if(ImGui::Button("Apply"))
            //{
            //    dsc = dscEdited;
            //    root->remNode(treeNode);
            //    root->remNode(leafNode);
            //    treeNode = gen.buildInstancedTree(dsc, material_tree, instancedShader);
            //    leafNode = gen.buildInstancedLeaf(dsc, material_leaf, instancedShader);
            //    treeNode->getState()->setRenderPass(env_sha);
            //    leafNode->getState()->setRenderPass(env_sha);
            //    root->addNode(treeNode);
            //    root->addNode(leafNode);
            //}
            //if(ImGui::Button("Revert"))
            //{
            //    dscEdited = dsc;
            //}
            // ImGui::End();
        }
        #endif

        // impgears stuff
        const float PI = 3.141592;
        double t = now;
        Vec3 lp(cos(t)*50.0,14.0,sin(t)*50.0);
        // Vec3 lp(cos(t)*10.0,0.0,sin(t)*10.0);
        light->setPosition(lp);
        pointNode->setPosition(lp);
        Vec3 lp2(cos(camera_rad*PI ),sin(camera_rad_y*PI*0.5),sin(camera_rad*PI));
        camera->setPosition(Vec3(0.0,0.0,0.0) + lp2*Vec3(camera_dst));
        camera->setTarget(Vec3(0.0f, 0.0, 0.0f));
        // camera->setTarget(Vec3(0.0));

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
