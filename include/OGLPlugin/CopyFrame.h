#ifndef IMP_COPY_FRAME_H
#define IMP_COPY_FRAME_H

#include <Core/Object.h>

#include <OGLPlugin/FxRenderPass.h>

IMPGEARS_BEGIN

struct GlRenderer;

class IMP_API CopyFrame : public FxRenderPass
{
public:

    Meta_Class(CopyFrame)

    CopyFrame();
    virtual ~CopyFrame();

    virtual void setup(std::vector<ImageSampler::Ptr>& input, std::vector<ImageSampler::Ptr>& output);

    virtual void apply(GlRenderer* renderer);

protected:

    int _subpassCount;
    Graph::Ptr _graph;
    RenderTarget::Ptr _frame;
};

IMPGEARS_END

#endif // IMP_COPY_FRAME_H
