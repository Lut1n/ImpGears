#ifndef IMP_RENDER_TO_CUBEMAP_H
#define IMP_RENDER_TO_CUBEMAP_H

#include <Core/Object.h>
#include <Graphics/Sampler.h>
#include <Renderer/RenderTarget.h>
#include <Renderer/SceneRenderer.h>
#include <SceneGraph/Graph.h>
#include <SceneGraph/Camera.h>


IMPGEARS_BEGIN

class GlRenderer;

class IMP_API RenderToCubeMap : public Object
{
public:
    Meta_Class(RenderToCubeMap)

    RenderToCubeMap(GlRenderer* renderer, CubeMapSampler::Ptr cubemap = nullptr);

    virtual ~RenderToCubeMap();

    void setCubeMap(CubeMapSampler::Ptr cubemap);
    CubeMapSampler::Ptr getCubeMap();

    void setRange(float near, float far);
    void setResolution(float resolution);

    void render(const Graph::Ptr& scene, const Vec3& center, SceneRenderer::RenderFrame frameType = SceneRenderer::RenderFrame_Default, ReflexionModel::Ptr overrideShader = nullptr);

    void render(RenderQueue::Ptr& queue, const Vec3& center, SceneRenderer::RenderFrame frameType = SceneRenderer::RenderFrame_Default, ReflexionModel::Ptr overrideShader = nullptr);

protected:
    GlRenderer* _renderer;
    Graph::Ptr _clone;
    RenderQueue::Ptr _queue;
    CubeMapSampler::Ptr _cubemap;
    Matrix4 _proj;
    Camera::Ptr _camera;
    State::Ptr _state;

    Vec3 _directions[6];
    Vec3 _upDirections[6];
    RenderTarget::Ptr _targets[6];
    Vec2 _range;
    float _resolution;
};


IMPGEARS_END

#endif // IMP_RENDER_TO_CUBEMAP_H
