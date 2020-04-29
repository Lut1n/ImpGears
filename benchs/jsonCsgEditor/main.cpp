#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GL/glew.h>
// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

#include <ImpGears/Descriptors.h>
#include <ImpGears/Plugins.h>
#include <ImpGears/Graphics.h>
#include <ImpGears/SceneGraph.h>

using namespace imp;

#define IMPLEMENT_BASIC_GEOMETRIES
#include "../common/basic_geometries.h"

#include "render_skybox.h"

#include <fstream>


static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

#include "geometry_json.h"
#include "geometry_build.h"

struct MouseButtonState
{
   Vec2 start_pos;
   Vec2 last_pos;
   int last_state;
   
   MouseButtonState() : last_state(GLFW_RELEASE) {}
   
   Vec2 getStartPos() {return start_pos;}
   
   virtual void onPressed(const Vec2& curr_pos) {}
   virtual void onReleased(const Vec2& curr_pos) {}
   virtual void onDragged(const Vec2& curr_pos) {}
   
   void pressed(const Vec2& curr_pos)
   {
        start_pos = curr_pos;
        onPressed(curr_pos);
   }
   
   void released(const Vec2& curr_pos)
   {
        onReleased(curr_pos);
   }
   
   void dragged(const Vec2& curr_pos)
   {  
        onDragged(curr_pos);
   }
   
   void update(const Vec2& curr_pos, int button_state)
   {
        if (button_state == GLFW_PRESS)
        {
            if(last_state == GLFW_RELEASE) pressed(curr_pos);
            else if(last_pos != curr_pos) dragged(curr_pos);
            
        }
        else if(button_state == GLFW_RELEASE)
        {
            if(last_state == GLFW_PRESS) released(curr_pos);
        }

        last_state = button_state;
        last_pos = curr_pos;
   }
};

struct LeftButton : MouseButtonState
{
    ViewState* editor_view;
    Vec2 camera_rad_start;
    
    LeftButton(ViewState* view) : editor_view(view) {}
    
    void onPressed(const Vec2& curr_pos) override
    {
        camera_rad_start = editor_view->camera_rad;
        std::cout << "pressed " << camera_rad_start[0] << "; " << camera_rad_start[1] << std::endl;
    }
    
    void onReleased(const Vec2& curr_pos) override
    {
        Vec2 move = curr_pos - getStartPos();
        std::cout << "released " << move[0] << "; " << move[1] << std::endl;
    }
    
    void onDragged(const Vec2& curr_pos) override
    {
        // HOLD PRESSING AND MOVE
        Vec2 move = curr_pos - getStartPos();

        editor_view->camera_rad[0] = camera_rad_start[0] + move[0] * 10.0;
        editor_view->camera_rad[1] = clamp(camera_rad_start[1] + move[1] * 10.0,-1.0,1.0);
        
    }
};

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
    
    // request MSAA
    int samples = 4;
    // glfwWindowHint(GLFW_SAMPLES, samples);


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

    // glGetIntegerv(GL_SAMPLES, &samples);
    // std::cout << "context reports MSAA available with " << samples << " samples" << std::endl;
    // glEnable(GL_MULTISAMPLE);
    // glDisable(GL_MULTISAMPLE);

    
    std::string pluginName = "OGLPlugin";
    pluginName = "lib" + pluginName + "." + LIB_EXT;
    SceneRenderer::Ptr renderer;
    RenderPlugin::Ptr plugin = PluginManager::open( pluginName );
    renderer = plugin->getRenderer();
    
    
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
    material_light->_emissive = ImageSampler::create(8,8,3,Vec3(1.0));
    
    
    Geometry::Ptr coords = buildCoord();
    coords->scale(Vec3(10.0));
    GeoNode::Ptr coordsNode = GeoNode::create(coords, false);
    coordsNode->setReflexion(reflection_scene);
    coordsNode->setPosition(Vec3(0.0,0.0,0.0));

    LightNode::Ptr light = LightNode::create(Vec3(1.0),10.f);
    SkyBox::Ptr sky = SkyBox::create();

    root->addNode(light);
    root->addNode(camera);
    root->addNode(coordsNode);
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
    
    sky->getState()->setRenderPass(env);


    // SceneRenderer::Ptr renderer = renderModeMngr.loadRenderer();
    renderer->enableFeature(SceneRenderer::Feature_Shadow, true);
    renderer->enableFeature(SceneRenderer::Feature_Environment, true);
    renderer->enableFeature(SceneRenderer::Feature_Bloom, true);
    renderer->enableFeature(SceneRenderer::Feature_SSAO, true);
    renderer->enableMsaa(true);
    renderer->setAmbient(0.5);
    renderer->setLightPower(1000.0);
    renderer->setDirect(true);
    renderer->setShadowResolution(512.0);
    renderer->setOutputFrame(SceneRenderer::RenderFrame_Default);
    renderer->initialize();
    
    
    graph->getInitState()->setViewport( Vec4(0.0,0.0,1024.0,1024.0) );
    graph->setClearColor(Vec4(0.0,0.0,1.0,1.0));


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
    
    // Our state
    Vec3 obj_position = Vec3(0.0);
    Vec3 obj_rotation = Vec3(0.0);
    Vec3 obj_scale = Vec3(1.0);
    Vec4 material_color = Vec4(1.0);
    float shininess = 32.0;
    ViewState editor_view;

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
    
    std::vector<GeoNode::Ptr> models;
    std::vector<ModelState> states;
    
    bool requestSave = false;
    bool requestLoad = false;
    bool requestReset = false;
    bool requestDuplicate = false;
    bool requestAdd = false;
    int selected = 0;
    GeoPrimitive selectedPrimitive = Cube;
    
    std::string filename; filename.reserve(1024);
    char buff[1024]; buff[0] = '\0';
   
    Vec3 camera_rad_last;
    LeftButton left(&editor_view);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();
        
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        Vec2 curr_pos(xpos / 1024.0, ypos / 720.0);
        int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        left.update(curr_pos,state);
        
        
        

        // common part, do this only once
        float now = glfwGetTime();

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

        if(item_current == 0) renderer->setOutputFrame(SceneRenderer::RenderFrame_Default);
        else if(item_current == 1) renderer->setOutputFrame(SceneRenderer::RenderFrame_Lighting);
        else if(item_current == 2) renderer->setOutputFrame(SceneRenderer::RenderFrame_Emissive);
        else if(item_current == 3) renderer->setOutputFrame(SceneRenderer::RenderFrame_Normals);
        else if(item_current == 4) renderer->setOutputFrame(SceneRenderer::RenderFrame_ShadowMap);
        else if(item_current == 5) renderer->setOutputFrame(SceneRenderer::RenderFrame_Environment);
        else if(item_current == 6) renderer->setOutputFrame(SceneRenderer::RenderFrame_Bloom);
        else if(item_current == 7) renderer->setOutputFrame(SceneRenderer::RenderFrame_Color);
        else if(item_current == 8) renderer->setOutputFrame(SceneRenderer::RenderFrame_Reflectivity);
        else if(item_current == 9) renderer->setOutputFrame(SceneRenderer::RenderFrame_Depth);
        else if(item_current == 10) renderer->setOutputFrame(SceneRenderer::RenderFrame_SSAO);

        ImGui::End();

        ImGui::Begin("Scene");
        ImGui::SliderFloat("Camera long", &editor_view.camera_rad[0], -1.0f, 1.0f);
        ImGui::SliderFloat("Camera lat", &editor_view.camera_rad[1], -1.0f, 1.0f);
        ImGui::SliderFloat("Camera dist", &editor_view.camera_rad[2], 1.0f, 100.0f);
        

        ImGui::InputTextWithHint("filename", "enter filename for saving or loading", buff, filename.capacity());
        if(ImGui::Button("Save"))
        {
            filename = buff;
            requestSave = true;
        }
        if(ImGui::Button("Load"))
        {
            filename = buff;
            requestLoad = true;
        }
        if(ImGui::Button("Reset"))
        {
            requestReset = true;
        }
        
        ImGui::End();


        ImGui::Begin("Model");
        if(ImGui::Button("Add Cube"))
        {
            selectedPrimitive = Cube;
            requestAdd = true;
        }
        
        if(ImGui::Button("Add Sphere"))
        {
            selectedPrimitive = Sphere;
            requestAdd = true;
        }
        ImGui::SameLine();
        
        if(ImGui::Button("Add Torus"))
        {
            selectedPrimitive = Torus;
            requestAdd = true;
        }
        
        if(ImGui::Button("Add Cylinder"))
        {
            selectedPrimitive = Cylinder;
            requestAdd = true;
        }
        ImGui::SameLine();
        if(ImGui::Button("Add Capsule"))
        {
            selectedPrimitive = Capsule;
            requestAdd = true;
        }
        ImGui::SameLine();
        if(ImGui::Button("Add Crystal"))
        {
            selectedPrimitive = Crystal;
            requestAdd = true;
        }
        
        
        
        int n=0;
        for(int i=0;i<states.size();++i)
        {
            std::stringstream ss;
            ss << "primitive #" << i;
            std::string str = ss.str();
            if (ImGui::TreeNode(str.c_str()))
            {
                ImGui::SliderFloat3("position", states[i].position.data(),-10.0,10.0);
                ImGui::SliderFloat3("rotation", states[i].rotation.data(),-3.14,3.14);
                ImGui::SliderFloat3("scale", states[i].scale.data(),0.01,30.0);
                ImGui::ColorEdit3("color", states[i].color.data());
            
                models[i]->setPosition(states[i].position);
                models[i]->setRotation(states[i].rotation);
                models[i]->setScale(states[i].scale);
                models[i]->_material->_color = states[i].color;
                
                if(ImGui::Button("Erase"))
                {
                    root->remNode( models[i] );
                    models[i] = nullptr;
                }
                
                if(ImGui::Button("Duplicate"))
                {
                    selected = i;
                    requestDuplicate = true;
                }
                
                ImGui::TreePop();
            }
        }
        
        for(int i=models.size()-1;i>=0;--i)
        {
            if(models[i] == nullptr)
            {
                states.erase(states.begin()+i);
                models.erase(models.begin()+i);
            }
        }
        ImGui::End();

        // impgears stuff
        const float PI = 3.141592;
        Vec3 lp2(cos(editor_view.camera_rad[0]*PI ),sin(editor_view.camera_rad[1]*PI*0.5),sin(editor_view.camera_rad[0]*PI));
        camera->setPosition(Vec3(0.0,0.0,0.0) + lp2*Vec3(editor_view.camera_rad[2]));
        
        light->setPosition(Vec3(20.0,20.0,20.0) );
        camera->setTarget(Vec3(0.0f, 0.0, 0.0f));

        renderer->enableFeature(SceneRenderer::Feature_Phong, phongEnable);
        renderer->enableFeature(SceneRenderer::Feature_Shadow, shadowsEnable);
        renderer->enableFeature(SceneRenderer::Feature_Environment, environmentEnable);
        renderer->enableFeature(SceneRenderer::Feature_Bloom, bloomEnable);
        renderer->enableFeature(SceneRenderer::Feature_SSAO, ssaoEnable);


        // Rendering
        ImGui::Render();
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
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        
        if(requestReset)
        {
            for(int i=0;i<models.size();++i) root->remNode( models[i] );            
            states.clear();
            models.clear();
            editor_view = ViewState(); // default view
        }
        if(requestDuplicate)
        {
            createMesh( states, models, root, states[selected], reflection_scene, env_sha );
        }
        if(requestSave && !requestLoad && filename.size() > 0)
        {
            save(filename, states, editor_view);
        }
        if(requestLoad && !requestSave && filename.size() > 0)
        {
            std::vector<ModelState> toadd;
            load(filename, toadd, editor_view);
            for(auto a : toadd) createMesh( states, models, root, a, reflection_scene, env_sha );
        }
        if(requestAdd)
        {
            ModelState st; st.primitive = selectedPrimitive;
            createMesh( states, models, root, st, reflection_scene, env_sha );
        }
        
        requestReset = false;
        requestSave = false;
        requestLoad = false;
        requestDuplicate = false;
        requestAdd = false;
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;

}
