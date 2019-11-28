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

    virtual void setup(std::vector<ImageSampler::Ptr>& input, std::vector<ImageSampler::Ptr>& output) = 0;

    Graph::Ptr buildQuadGraph(const std::string& glsl_lighting, Vec4 viewport = Vec4(0.0,0.0,512.0,512.0)) const;

    virtual void apply(GlRenderer* renderer) = 0;

protected:

    std::vector<ImageSampler::Ptr> _input;
    std::vector<ImageSampler::Ptr> _output;

    // Vec4 _viewport;
    // ReflexionModel::Ptr _shader;
    // std::vector<Uniform::Ptr> _uniforms;
    // RenderTarget::Ptr _frameBuffer;
    // Graph::Ptr _graph;
    // QuadNode::Ptr _quad;
};

IMPGEARS_END

#endif // IMP_FX_RENDERPASS_H
