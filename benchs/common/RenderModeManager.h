#ifndef RENDER_MODE_MANAGER_H
#define RENDER_MODE_MANAGER_H

#include <ImpGears/Renderer/SceneRenderer.h>
#include <ImpGears/Renderer/CpuRenderer.h>
#include <ImpGears/Plugins/RenderPlugin.h>

struct RenderModeManager
{
    bool offscreen;
    RenderPlugin::Ptr plugin;
    SceneRenderer::Ptr renderer;
    Vec4 viewport;
    Image::Ptr target;
    sf::Texture texture;
    sf::Sprite sprite;

    RenderModeManager();

    void setArgs(int argc, const char* argv[]);
    void setArgs(int argc, char* argv[]);

    SceneRenderer::Ptr loadRenderer();

    void draw(sf::RenderTarget& renderTarget);
};

#endif


#if defined IMPLEMENT_RENDER_MODE_MANAGER

RenderModeManager::RenderModeManager()
{
    offscreen = false;
}

void RenderModeManager::setArgs(int argc, const char* argv[])
{
    std::string arg = "";
    if(argc>1) arg = argv[1];

    offscreen = arg != "-gpu";
}

void RenderModeManager::setArgs(int argc, char* argv[])
{
    std::string arg = "";
    if(argc>1) arg = argv[1];

    offscreen = arg != "-gpu";
}

SceneRenderer::Ptr RenderModeManager::loadRenderer()
{
    if(!offscreen)
    {
        std::string pluginName = "OGLPlugin";
        pluginName = "./lib" + pluginName + "." + LIB_EXT;
        plugin = PluginManager::open( pluginName );
        if(plugin)
        {
            renderer = plugin->getRenderer();
            renderer->enableFeature(SceneRenderer::Feature_Bloom, true);
            viewport.set(0.0,0.0,1024,1024);
            target = Image::create(1024,1024,4);

            ImageSampler::Ptr sampler = ImageSampler::create(target);
            std::vector<ImageSampler::Ptr> samplers = {sampler};
            RenderTarget::Ptr rt = RenderTarget::create();
            rt->build(samplers, true);
            renderer->setTargets(rt);
        }
        else
        {
            offscreen = true;
        }
    }

    if(offscreen)
    {
        renderer = CpuRenderer::create();

        int RES = 128;
        target = Image::create(RES,RES,4);

        ImageSampler::Ptr sampler = ImageSampler::create(target);
        std::vector<ImageSampler::Ptr> samplers = {sampler};
        RenderTarget::Ptr rt = RenderTarget::create();
        rt->build(samplers, true);
        renderer->setTargets(rt);

        viewport.set(0.0,0.0,RES,RES);
        renderer->setDirect(false);
    }

    texture.create(target->width(), target->height());
    // texture.setSmooth(true);
    sprite.setTexture(texture);

    return renderer;
}

void RenderModeManager::draw(sf::RenderTarget& renderTarget)
{
    if(!renderer->isDirect()) // offscreen
    {
        if(plugin) target = renderer->getTarget(true);

        texture.update(target->data(),target->width(),target->height(),0,0);
        sprite.setScale( float(renderTarget.getSize().x)/target->width(), -float(renderTarget.getSize().y)/target->height() );
        sprite.setPosition( 0, renderTarget.getSize().y );
        renderTarget.draw(sprite);
    }
}

#endif

