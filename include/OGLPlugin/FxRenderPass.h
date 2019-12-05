#ifndef IMP_FX_RENDERPASS_H
#define IMP_FX_RENDERPASS_H

#include <Core/Object.h>

#include <SceneGraph/Graph.h>
#include <SceneGraph/ReflexionModel.h>
#include <SceneGraph/QuadNode.h>
#include <Renderer/RenderTarget.h>


IMPGEARS_BEGIN

struct GlRenderer;

class IMP_API FxRenderPass : public Object
{
public:

    Meta_Class(FxRenderPass)

    FxRenderPass();
    virtual ~FxRenderPass();

    virtual void setup(std::vector<ImageSampler::Ptr>& input, std::vector<ImageSampler::Ptr>& output);

    Graph::Ptr buildQuadGraph(const std::string& debug_name,
                              const std::string& glsl_lighting,
                              Vec4 viewport = Vec4(0.0,0.0,1024.0,1024.0)) const;

    virtual void apply(GlRenderer* renderer) = 0;

    virtual void setInput(ImageSampler::Ptr sampler, int id = 0);
    virtual void setOutput(ImageSampler::Ptr sampler, int id = 0);

protected:

    std::vector<ImageSampler::Ptr> _input;
    std::vector<ImageSampler::Ptr> _output;
};

IMPGEARS_END

#endif // IMP_FX_RENDERPASS_H
