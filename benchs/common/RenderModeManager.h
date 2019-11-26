#ifndef RENDER_MODE_MANAGER_H
#define RENDER_MODE_MANAGER_H

#include <Renderer/SceneRenderer.h>
#include <Renderer/CpuRenderer.h>
#include <Plugins/RenderPlugin.h>

struct RenderModeManager
{
    bool offscreen;
    RenderPlugin::Ptr plugin;
    SceneRenderer::Ptr renderer;
    Vec4 viewport;
    Image::Ptr target, target2;
    sf::Texture texture;
    sf::Sprite sprite;

    RenderModeManager();

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
        std::string pluginName = "glPlugin";
        pluginName = "lib" + pluginName + "." + LIB_EXT;
        plugin = PluginManager::open( pluginName );
        if(plugin)
        {
            renderer = plugin->getRenderer();
            renderer->enableFeature(SceneRenderer::Feature_Bloom, true);
            viewport.set(0.0,0.0,512,512);
            target = Image::create(512,512,4);
            renderer->setTarget(target, 0);
            target2 = Image::create(512,512,4);
            renderer->setTarget(target2, 1);
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
        renderer->setTarget(target, 0);
        target2 = Image::create(RES,RES,4);
        renderer->setTarget(target2, 1);

        viewport.set(0.0,0.0,RES,RES);
        renderer->setDirect(false);

        texture.create(512, 512);
        // texture.setSmooth(true);
        sprite.setTexture(texture);
    }

    return renderer;
}

void RenderModeManager::draw(sf::RenderTarget& renderTarget)
{
    if(offscreen)
    {
        texture.update(target->data(),target->width(),target->height(),0,0);
        sprite.setScale( 512.0 / target->width(), -512.0 / target->height() );
        sprite.setPosition( 0, 512 );
        renderTarget.draw(sprite);
    }
}

#endif

