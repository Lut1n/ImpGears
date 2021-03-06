// imgui
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

//glfw
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// ImpGears
#include <ImpGears/Descriptors.h>
#include <ImpGears/Plugins.h>
#include <ImpGears/Graphics.h>
#include <ImpGears/SceneGraph.h>
using namespace imp;

// help stuff
#define IMPLEMENT_BASIC_GEOMETRIES
#include "../common/basic_geometries.h"
#include "render_skybox.h"

// std
#include <fstream>

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void save(const std::string& filename, JsonSceneNode::Ptr models)
{
    std::string data = models->writeIntoJson();
    std::ofstream of(filename);
    of.write(data.c_str(), data.size());
}

void load(const std::string& filename, JsonSceneNode::Ptr models)
{
    std::string json = loadText(filename);
    models->parseJsonData(json);
}


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
    JsonSceneNode::Ptr editor_view;
    Vec3 camera_rad_start;
    
    LeftButton(JsonSceneNode::Ptr view) : editor_view(view) {}
    
    void onPressed(const Vec2& curr_pos) override
    {
        camera_rad_start = editor_view->favoriteViewOri();
        std::cout << "pressed " << camera_rad_start[0] << "; " << camera_rad_start[1] << std::endl;
    }
    
    void onReleased(const Vec2& curr_pos) override
    {
        Vec3 move = curr_pos - getStartPos();
        std::cout << "released " << move[0] << "; " << move[1] << std::endl;
    }
    
    void onDragged(const Vec2& curr_pos) override
    {
        // HOLD PRESSING AND MOVE
        Vec3 move = curr_pos - getStartPos();

        Vec3& camera_rad = editor_view->favoriteViewOri();
        camera_rad[0] = camera_rad_start[0] + move[0] * 10.0;
        camera_rad[1] = clamp(camera_rad_start[1] + move[1] * 10.0,-1.0,1.0);
        
    }
};

struct RightButton : MouseButtonState
{
    JsonSceneNode::Ptr editor_view;
    Vec3 camera_rad_start;
    
    RightButton(JsonSceneNode::Ptr view) : editor_view(view) {}
    
    void onPressed(const Vec2& curr_pos) override
    {
        camera_rad_start = editor_view->favoriteViewOri();
        std::cout << "pressed " << camera_rad_start[0] << "; " << camera_rad_start[1] << std::endl;
    }
    
    void onReleased(const Vec2& curr_pos) override
    {
        Vec3 move = curr_pos - getStartPos();
        std::cout << "released " << move[0] << "; " << move[1] << std::endl;
    }
    
    void onDragged(const Vec2& curr_pos) override
    {
        // HOLD PRESSING AND MOVE
        Vec3 move = curr_pos - getStartPos();

        Vec3& camera_rad = editor_view->favoriteViewOri();
        camera_rad[2] = clamp(camera_rad_start[2] + move[1] * 10.0,0.0,100.0);
        
    }
};

#include <ImpGears/Geometry/Polyhedron.h>
void getLines(Polyhedron poly, std::vector<Vec3>& vertices, std::vector<Vec3>& colors)
{
    vertices.clear();
    for(int f=0;f<poly.faceCount();++f)
    {
        std::vector<Vec3> verts = poly[f].vertices;
        
        Vec3 last = verts.back();
        for(auto p : verts)
        {
            vertices.push_back(last);
            vertices.push_back(p);
            colors.push_back(Vec3(0.0,0.0,1.0));
            colors.push_back(Vec3(0.0,0.0,1.0));
            last = p;
        }
    }
}

Geometry transformGeo(const Geometry& geo, const Vec3& p, const Vec3& r, const Vec3& s)
{
    Geometry copy = geo;
    copy *= s;
    copy.rotX(r[0]); copy.rotY(r[1]); copy.rotZ(r[2]);
    copy += p;
    return copy;
}

GeoNode::Ptr merge2first(const std::vector<GeoNode::Ptr>& nodes)
{   
    Geometry::Ptr geomerged = Geometry::create();
    Polyhedron merged;
    {
        // get geometry copies
        Geometry geo1 = transformGeo(*(nodes[0]->_geo), nodes[0]->getPosition(), nodes[0]->getRotation(), nodes[0]->getScale());
        Geometry geo2 = transformGeo(*(nodes[1]->_geo), nodes[1]->getPosition(), nodes[1]->getRotation(), nodes[1]->getScale());
        
        Polyhedron poly1, poly2;
        poly1.set(geo1._vertices);
        poly2.set(geo2._vertices);
        merged = poly1 + poly2;
        // merged = override_plus(poly1,poly2);
    }
    
    const bool __DEBUG_MERGE = true;
    
    geomerged->setPrimitive(Geometry::Primitive_Triangles);
    merged.getTriangles(geomerged->_vertices);
    // getTriangles(merged, geomerged->_vertices);
    
    geomerged->generateColors( Vec3(1.0) );
    geomerged->generateNormals( Geometry::NormalGenMode_PerFace );
    geomerged->generateTexCoords( Geometry::TexGenMode_Cubic, 1.0);
    
    GeoNode::Ptr result = GeoNode::create(geomerged);
    result->setReflexion( nodes[0]->getState()->getReflexion() );
    result->setRenderPass( nodes[0]->getState()->getRenderPass() );
    // result->setMaterial( nodes[0]->_material );
    
    if(__DEBUG_MERGE)
    {
        Geometry::Ptr geoline = Geometry::create();
        geoline->setPrimitive(Geometry::Primitive_Lines);
        getLines(merged, geoline->_vertices, geoline->_colors);
        geoline->_hasColors = true;
        GeoNode::Ptr linenode = GeoNode::create(geoline);
        linenode->setReflexion( nodes[0]->getState()->getReflexion() );
        linenode->setRenderPass( nodes[0]->getState()->getRenderPass() );
        result->addNode(linenode);
    }
    
    
    return result;
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
    renderer->enableFeature(SceneRenderer::Feature_Shadow, false);
    renderer->enableFeature(SceneRenderer::Feature_Environment, true);
    renderer->enableFeature(SceneRenderer::Feature_Bloom, true);
    renderer->enableFeature(SceneRenderer::Feature_SSAO, false);
    renderer->enableMsaa(true);
    renderer->setAmbient(0.5);
    renderer->setLightPower(1000.0);
    renderer->setDirect(true);
    renderer->setShadowResolution(512.0);
    renderer->setOutputFrame(SceneRenderer::RenderFrame_Default);
    renderer->initialize();
    
    
    graph->getInitState()->setViewport( Vec4(0.0,0.0,1024.0,1024.0) );
    graph->setClearColor(Vec4(0.0,0.0,1.0,1.0));
    graph->getInitState()->setLineWidth( 2.0 );


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

    // for each timer keep variable after how many seconds to trigger it
    float time = 2.f; // two seconds
    float previous = glfwGetTime();

    bool shadowsEnable = false;
    bool environmentEnable = true;
    bool bloomEnable = true;
    bool ssaoEnable = false;
    bool phongEnable = true;
    bool wireframe = false;

    Vec4 viewport(0.0,0.0,512.0,512.0);
    
    JsonSceneNode::Ptr scene = JsonSceneNode::create();
    std::vector<GeoNode::Ptr> models;
    
    bool requestSave = false;
    bool requestLoad = false;
    bool requestReset = false;
    bool requestDuplicate = false;
    bool requestAdd = false;
    bool requestMerge = false;
    int selected = 0;
    JsonSceneNode::GeometricPrimitive selectedPrimitive = JsonSceneNode::Cube;
    
    std::string filename; filename.reserve(1024);
    char buff[1024]; buff[0] = '\0';
   
    Vec3 camera_rad_last;
    LeftButton left(scene);
    RightButton right(scene);

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        Vec2 curr_pos(xpos / 1024.0, ypos / 720.0);
        int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
        left.update(curr_pos,state);
        state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
        right.update(curr_pos,state);

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
        ImGui::SliderFloat("Camera long", &scene->favoriteViewOri()[0], -1.0f, 1.0f);
        ImGui::SliderFloat("Camera lat", &scene->favoriteViewOri()[1], -1.0f, 1.0f);
        ImGui::SliderFloat("Camera dist", &scene->favoriteViewOri()[2], 1.0f, 100.0f);
        

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
        if(ImGui::Button("[experimental] Merge"))
        {
            requestMerge = true;
        }
        
        ImGui::End();


        ImGui::Begin("Model");
        if(ImGui::Button("Add Cube"))
        {
            selectedPrimitive = JsonSceneNode::Cube;
            requestAdd = true;
        }
        
        if(ImGui::Button("Add Sphere"))
        {
            selectedPrimitive = JsonSceneNode::Sphere;
            requestAdd = true;
        }
        ImGui::SameLine();
        
        if(ImGui::Button("Add Torus"))
        {
            selectedPrimitive = JsonSceneNode::Torus;
            requestAdd = true;
        }
        
        if(ImGui::Button("Add Cylinder"))
        {
            selectedPrimitive = JsonSceneNode::Cylinder;
            requestAdd = true;
        }
        ImGui::SameLine();
        if(ImGui::Button("Add Capsule"))
        {
            selectedPrimitive = JsonSceneNode::Capsule;
            requestAdd = true;
        }
        ImGui::SameLine();
        if(ImGui::Button("Add Crystal"))
        {
            selectedPrimitive = JsonSceneNode::Crystal;
            requestAdd = true;
        }
        
        
        
        int n=0;
        for(int i=0;i<scene->itemCount();++i)
        {
            std::stringstream ss;
            ss << "primitive #" << i;
            std::string str = ss.str();
            if (ImGui::TreeNode(str.c_str()))
            {
                ImGui::SliderFloat3("position", scene->getItem(i).position.data(),-10.0,10.0);
                ImGui::SliderFloat3("rotation", scene->getItem(i).rotation.data(),-3.14,3.14);
                ImGui::SliderFloat3("scale", scene->getItem(i).scale.data(),0.01,30.0);
                ImGui::ColorEdit3("color", scene->getItem(i).color.data());
            
                models[i]->setPosition(scene->getItem(i).position);
                models[i]->setRotation(scene->getItem(i).rotation);
                models[i]->setScale(scene->getItem(i).scale);
                models[i]->_material->_color = scene->getItem(i).color;
                
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
                scene->remItem(i);
                models.erase(models.begin()+i);
            }
        }
        ImGui::End();

        // impgears stuff
        const float PI = 3.141592;
        Vec3 lp2(cos(scene->favoriteViewOri()[0]*PI ),sin(scene->favoriteViewOri()[1]*PI*0.5),sin(scene->favoriteViewOri()[0]*PI));
        camera->setPosition(Vec3(0.0,0.0,0.0) + lp2*Vec3(scene->favoriteViewOri()[2]));
        
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
            for(int i=0;i<(int)models.size();++i) root->remNode( models[i] );
            scene->clearItems();
            models.clear();
            scene->favoriteViewOri().set(0.25,0.25,10.0);
        }
        if(requestDuplicate)
        {
            JsonSceneNode::Item item = scene->getItem(selected);
            scene->addItem(item);
            GeoNode::Ptr node = JsonSceneNode::buildGeometryNode( item, reflection_scene, env_sha );
            models.push_back( node );
            root->addNode(node);
        }
        if(requestSave && !requestLoad && filename.size() > 0)
        {
            save(filename, scene);
        }
        if(requestLoad && !requestSave && filename.size() > 0)
        {
            int lastSize = scene->itemCount();
            load(filename, scene);
            for(int i=lastSize;i<scene->itemCount();++i)
            {
                GeoNode::Ptr geo = JsonSceneNode::buildGeometryNode(scene->getItem(i), reflection_scene, env_sha);
                models.push_back(geo);
                root->addNode(geo);
            }
        }
        if(requestAdd)
        {
            JsonSceneNode::Item item; item.primitive = selectedPrimitive;
            scene->addItem(item);
            GeoNode::Ptr node = JsonSceneNode::buildGeometryNode( item, reflection_scene, env_sha );
            models.push_back( node );
            root->addNode(node);
        }
        if(requestMerge)
        {
            // create node
            GeoNode::Ptr nn = merge2first(models);
            
            // reset all
            for(int i=0;i<(int)models.size();++i) root->remNode( models[i] );
            scene->clearItems();
            models.clear();
            // scene->favoriteViewOri().set(0.25,0.25,10.0);
            //
            
            // add node to scene
            models.push_back(nn);
            root->addNode(nn);
        }
        
        requestReset = false;
        requestSave = false;
        requestLoad = false;
        requestDuplicate = false;
        requestAdd = false;
        requestMerge = false;
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;

}
