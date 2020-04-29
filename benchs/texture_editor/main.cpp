#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <GL/glew.h>
// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

#include <SceneGraph/Graph.h>
#include <SceneGraph/QuadNode.h>
#include <Plugins/RenderPlugin.h>
#include <Descriptors/ImageIO.h>
#include <Descriptors/FileInfo.h>
#include <Descriptors/JsonImageOp.h>

using namespace imp;

#include <SceneGraph/RenderToSamplerNode.h>
#include "leafrenderer.h"

#define IMPLEMENT_RENDER_MODE_MANAGER
#include "../common/RenderModeManager_glfw.h"


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

    RenderModeManager renderModeMngr;
    renderModeMngr.setArgs(argc, argv);

    // impgears stuff
    Graph::Ptr graph = Graph::create();
    LeafRenderer::Ptr leafR = LeafRenderer::create();
    graph->setRoot(leafR);


    SceneRenderer::Ptr renderer = renderModeMngr.loadRenderer();
    renderer->enableFeature(SceneRenderer::Feature_Shadow, false);
    renderer->enableFeature(SceneRenderer::Feature_Environment, false);
    renderer->enableFeature(SceneRenderer::Feature_Bloom, false);
    renderer->enableFeature(SceneRenderer::Feature_SSAO, false);
    renderer->setDirect(true);
    graph->getInitState()->setViewport( renderModeMngr.viewport );
    graph->getInitState()->setOrthographicProjection(-1.f, 1.f, -1.f, 1.f, 0.f, 1.f);
    graph->setClearColor(Vec4(0.0,0.0,1.0,1.0));

    // renderer->setOutputFrame(SceneRenderer::RenderFrame_Default);
    renderer->setOutputFrame(SceneRenderer::RenderFrame_Color);
    
    
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
    
    LeafDescription leafDescEdit;

    bool shadowsEnable = false;
    bool environmentEnable = false;
    bool bloomEnable = false;
    bool ssaoEnable = false;
    bool phongEnable = false;

    Vec4 viewport(0.0,0.0,512.0,512.0);
    

    // Main loop
    while(!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

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
        static int item_current = 7;
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
        ImGui::SliderFloat2("Model position", obj_position.data(),-1.0,1.0);
        ImGui::SliderFloat("Model rotation", &obj_rotation[2],-3.14,3.14);
        ImGui::SliderFloat2("Model scale", obj_scale.data(),0.1,2.0);
        ImGui::End();


        ImGui::Begin("Model");
        ImGui::SliderInt("branch depth", &leafDescEdit.branchDepth, 0.0, 10.0);
        ImGui::SliderInt("branch count", &leafDescEdit.branchCount, 0.0, 10.0);
        ImGui::SliderFloat("branch tilt", &leafDescEdit.tilt, 0.0, 3.0);
        ImGui::SliderFloat("branch length", &leafDescEdit.length, 0.0, 1.0);
        ImGui::SliderFloat("branch length fct", &leafDescEdit.lengthFct, 0.0, 2.0);
        ImGui::SliderFloat("branch thickness", &leafDescEdit.thickness, 0.0, 1.0);
        
        ImGui::SliderInt("texture octaves", &leafDescEdit.texOctave, 0.0, 4.0);
        ImGui::SliderFloat("texture frequency", &leafDescEdit.texFreq, 0.0, 32.0);
        ImGui::SliderFloat("texture persistence", &leafDescEdit.texPersist, 0.0, 1.0);
        
        ImGui::SliderFloat("shape freq start", &leafDescEdit.shapeFq1, 0.0, 3.0);
        ImGui::SliderFloat("shape freq end", &leafDescEdit.shapeFq2, 0.0, 3.0);
        ImGui::SliderFloat("shape size min", &leafDescEdit.shapeMin, 0.0, 1.0);
        ImGui::SliderFloat("shape size max", &leafDescEdit.shapeMax, 0.0, 1.0);
        // ImGui::SliderFloat("base radius", &dscEdited._baseRadius, 0.1, 10.0);
        // ImGui::SliderFloat("base height", &dscEdited._baseHeight, 0.1, 10.0);
        // ImGui::SliderInt("node count", &dscEdited._nodeCount, 1, 7);
        // ImGui::SliderInt("division count", &dscEdited._divisionCount, 1, 7);
        // ImGui::SliderFloat("tilt", &dscEdited._theta, 0.0, 3.0);
        // ImGui::SliderFloat("radius factor", &dscEdited._radiusFactor, 0.1, 1.0);
        // ImGui::SliderFloat("height factor", &dscEdited._heightFactor, 0.1, 1.0);
        // ImGui::SliderFloat("leaf size", &dscEdited._leafSize, 0.1, 10.0);
        // ImGui::SliderInt("leaf count", &dscEdited._leafCount, 0, 7);
        // ImGui::ColorEdit3("leaf color", dscEdited._leafColor.data());
        if(ImGui::Button("Apply"))
        {
            leafR->updateDesc(leafDescEdit);
        }
        if(ImGui::Button("Revert"))
        {
            leafDescEdit = leafR->desc;
        }
        ImGui::End();

        // impgears stuff
        leafR->setPosition(obj_position);
        leafR->setRotation(obj_rotation);
        leafR->setScale(obj_scale);

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
        // renderModeMngr.plugin->unloadUnused();
        // imgui
        glViewport(0, 0, display_w, display_h);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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
