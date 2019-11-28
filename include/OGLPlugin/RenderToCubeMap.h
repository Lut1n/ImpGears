#ifndef IMP_RENDER_TO_CUBEMAP_H
#define IMP_RENDER_TO_CUBEMAP_H

#include <Core/Object.h>
#include <Graphics/Sampler.h>
#include <Renderer/RenderTarget.h>
#include <SceneGraph/Graph.h>
#include <SceneGraph/Camera.h>


IMPGEARS_BEGIN

struct GlRenderer;

class IMP_API RenderToCubeMap : public Object
{
public:

    Meta_Class(RenderToCubeMap)

    RenderToCubeMap(GlRenderer* renderer, CubeMapSampler::Ptr cubemap = nullptr);

    virtual ~RenderToCubeMap();

    void setCubeMap(CubeMapSampler::Ptr cubemap);
    CubeMapSampler::Ptr getCubeMap();

    void render(const Graph::Ptr& scene, const Vec3& center);

protected:
private:

    GlRenderer* _renderer;
    Graph::Ptr _clone;
    CubeMapSampler::Ptr _cubemap;
    Matrix4 _proj;
    Camera::Ptr _camera;

    Vec3 _directions[6];
    Vec3 _upDirections[6];
    RenderTarget::Ptr _targets[6];

};


IMPGEARS_END

#endif // IMP_RENDER_TO_CUBEMAP_H
