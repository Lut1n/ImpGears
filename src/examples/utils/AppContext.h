#ifndef IMP_COMMON_APP_CONTEXT_H
#define IMP_COMMON_APP_CONTEXT_H


#include <ImpGears/Renderer/SceneRenderer.h>
#include <ImpGears/Renderer/CpuRenderer.h>
#include <ImpGears/Plugins/RenderPlugin.h>

//--------------------------------------------------------------
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

//--------------------------------------------------------------
#include "glad/glad.h"

namespace imp
{
    struct GLDisplayableTexture
    {
        GLuint tex = 0;
        GLuint readFboId = 0;
        GLsizei width = 0;
        GLsizei height = 0;

        void create(unsigned w = 512, unsigned h = 512);
        void destroy();
        void display(const Image::Ptr& target, int winWidth, int winHeight);
    };

    struct AppContext
    {
        bool offscreen = true;
        RenderPlugin::Ptr plugin;
        SceneRenderer::Ptr renderer;
        Vec4 viewport;
        Image::Ptr target;
        GLDisplayableTexture displayable;
        GLFWwindow* window = nullptr;
        int internalWidth = 128;
        int internalHeight = 128;
        int winWidth = 512;
        int winHeight = 512;

        AppContext();
        void setArgs(int argc, char* argv[]);
        double elapsedTime();

        bool begin();
        void end();

        SceneRenderer::Ptr loadRenderer(const std::string& title, int wWidth = 512, int wHeight = 512);
    };
}

#endif // IMP_COMMON_APP_CONTEXT_H


#if defined IMPLEMENT_APP_CONTEXT

// Append glad source code here
#include "../thirdparty/glad/src/glad.c"

namespace imp
{
    static void error_callback(int error, const char* description)
    {
        fprintf(stderr, "Error: %s\n", description);
    }

    static void window_size_callback(GLFWwindow* window, int width, int height)
    {}

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {}

    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    static void character_callback(GLFWwindow* window, unsigned int codepoint)
    {}

    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
    {}

    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
    {}

    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    {}
}


void imp::GLDisplayableTexture::create(unsigned w, unsigned h)
{
    width = w;
    height = h;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &readFboId);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, readFboId);
    glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D, tex, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}

void imp::GLDisplayableTexture::destroy()
{
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glDeleteTextures(1, &tex);
    glDeleteFramebuffers(1, &readFboId);
}

void imp::GLDisplayableTexture::display(const Image::Ptr& target, int winWidth, int winHeight)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, target->data());
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, readFboId);
    glBlitFramebuffer(0, 0, width, height,
        0, 0, winWidth, winHeight,
        GL_COLOR_BUFFER_BIT, GL_LINEAR);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}

imp::AppContext::AppContext()
{
}

double imp::AppContext::elapsedTime()
{
    return glfwGetTime();
}

void imp::AppContext::setArgs(int argc, char* argv[])
{
    std::string arg = "";
    if(argc>1) arg = argv[1];
    offscreen = arg != "-gpu";
}

imp::SceneRenderer::Ptr imp::AppContext::loadRenderer(const std::string& title, int wWidth, int wHeight)
{
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    winWidth = wWidth;
    winHeight = wHeight;
    window = glfwCreateWindow(wWidth, wHeight, title.c_str(), NULL, NULL);
    if (!window)
    {
        std::cout << "error: cannot create glfw window" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCharCallback(window, character_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwMakeContextCurrent(window);

    if(!offscreen)
    {
        std::string pluginName = std::string("./impgears-renderer-gl.") + LIB_EXT;
        plugin = PluginManager::open( pluginName );
        if(plugin)
        {
            plugin->init((void*) (glfwGetProcAddress));
            renderer = plugin->getRenderer();
            renderer->enableFeature(SceneRenderer::Feature_Bloom, true);
            internalWidth = 1024;
            internalHeight = 1024;
        }
        else
        {
            offscreen = true;
        }
    }

    if (offscreen)
    {
        renderer = CpuRenderer::create();
        gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
        displayable.create(internalWidth, internalHeight);
    }

    viewport.set(0.0, 0.0, internalWidth, internalHeight);
    target = Image::create(internalWidth, internalHeight, 4);
    ImageSampler::Ptr sampler = ImageSampler::create(target);
    std::vector<ImageSampler::Ptr> samplers = { sampler };
    RenderTarget::Ptr rt = RenderTarget::create();
    rt->build(samplers, true);
    renderer->setTargets(rt);

    renderer->setDirect(!offscreen);
    return renderer;
}

bool imp::AppContext::begin()
{
    return !glfwWindowShouldClose(window);
}

void imp::AppContext::end()
{
    if (plugin) plugin->unloadUnused();

    if (offscreen) displayable.display(target, winWidth, winHeight);

    glfwSwapBuffers(window);
    glfwPollEvents();
}

#endif // IMPLEMENT_APP_CONTEXT