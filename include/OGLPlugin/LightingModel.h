#ifndef IMP_LIGHTING_MODEL_H
#define IMP_LIGHTING_MODEL_H

#include <Core/Object.h>
#include <Graphics/Sampler.h>

#include <OGLPlugin/FxRenderPass.h>


IMPGEARS_BEGIN

struct GlRenderer;

class IMP_API LightingModel : public FxRenderPass
{
public:

    Meta_Class(LightingModel)

    LightingModel();
    virtual ~LightingModel();

    virtual void setup(std::vector<ImageSampler::Ptr>& input, std::vector<ImageSampler::Ptr>& output);

    void setCameraPos(const Vec3& cameraPos);
    void setLightPos(const Vec3& lightPos);

    virtual void apply(GlRenderer* renderer);

protected:

    Graph::Ptr _graph;
    RenderTarget::Ptr _frame;
    Vec3 _camera;
    Vec3 _lightPos;
};

IMPGEARS_END

#endif // IMP_LIGHTING_MODEL_H
