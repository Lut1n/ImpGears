#ifndef IMP_ENVIRONMENT_FX_H
#define IMP_ENVIRONMENT_FX_H

#include <Core/Object.h>
#include <Graphics/Sampler.h>

#include <OGLPlugin/FxRenderPass.h>


IMPGEARS_BEGIN

struct GlRenderer;

class IMP_API EnvironmentFX : public FxRenderPass
{
public:

    Meta_Class(EnvironmentFX)

    EnvironmentFX();
    virtual ~EnvironmentFX();

    virtual void setup(std::vector<ImageSampler::Ptr>& input, std::vector<ImageSampler::Ptr>& output);

    void setCubeMap(CubeMapSampler::Ptr cubemap);
    void setCameraPos(const Vec3& cameraPos);

    virtual void apply(GlRenderer* renderer);

protected:

    Graph::Ptr _graph;
    RenderTarget::Ptr _frame;
    CubeMapSampler::Ptr _cubemap;
    Vec3 _camera;
};

IMPGEARS_END

#endif // IMP_ENVIRONMENT_FX_H
