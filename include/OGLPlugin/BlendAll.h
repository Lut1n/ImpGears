#ifndef IMP_BLEND_ALL_H
#define IMP_BLEND_ALL_H

#include <Core/Object.h>

#include <OGLPlugin/FxRenderPass.h>

IMPGEARS_BEGIN

struct GlRenderer;

class IMP_API BlendAll : public FxRenderPass
{
public:

    Meta_Class(BlendAll)

    BlendAll();
    virtual ~BlendAll();

    virtual void setup(std::vector<ImageSampler::Ptr>& input, std::vector<ImageSampler::Ptr>& output);

    virtual void apply(GlRenderer* renderer);

protected:

    int _subpassCount;
    Graph::Ptr _graph;
    RenderTarget::Ptr _frame;
};

IMPGEARS_END

#endif // IMP_BLEND_ALL_H
