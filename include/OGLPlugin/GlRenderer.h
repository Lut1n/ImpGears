#ifndef IMP_GL_RENDERER_H
#define IMP_GL_RENDERER_H

#include <Renderer/RenderTarget.h>

#include <Plugins/RenderPlugin.h>

#include <SceneGraph/ClearNode.h>
#include <SceneGraph/GeoNode.h>
#include <OGLPlugin/Pipeline.h>

IMPGEARS_BEGIN

struct BloomFX;
struct EnvironmentFX;

class IMP_API GlRenderer : public SceneRenderer
{
public:

    Meta_Class(GlRenderer)

    GlRenderer();
    virtual ~GlRenderer();

    RenderQueue::Ptr applyRenderVisitor(const Graph::Ptr& scene);

    void drawQueue( RenderQueue::Ptr& queue, State::Ptr overrideState = nullptr,
                    SceneRenderer::RenderFrame renderPass = SceneRenderer::RenderFrame_Default );

    virtual void render(const Graph::Ptr& scene);

    virtual Image::Ptr getTarget(bool dlFromGPU = false, int id = 0);

    void loadRenderPlugin(const std::string& renderPlugin);
    void setRenderPlugin(RenderPlugin* plugin);

    void applyState(const State::Ptr& state,
                    SceneRenderer::RenderFrame renderPass = SceneRenderer::RenderFrame_Default);
    void applyClear(ClearNode* clearNode, SceneRenderer::RenderFrame renderPass = SceneRenderer::RenderFrame_Default);
    void drawGeometry(GeoNode* geoNode);

    RenderPlugin* _renderPlugin;

protected:

    Pipeline::Ptr _pipeline;

    RenderTarget::Ptr _internalFrames;
    // RenderTarget::Ptr _renderTargets;
};

IMPGEARS_END

#endif // IMP_GL_RENDERER_H
