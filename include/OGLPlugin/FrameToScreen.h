#ifndef IMP_FRAME_TO_SCREEN_H
#define IMP_FRAME_TO_SCREEN_H

#include <Core/Object.h>

#include <OGLPlugin/FxRenderPass.h>

IMPGEARS_BEGIN

struct GlRenderer;

class IMP_API FrameToScreen : public FxRenderPass
{
public:

    Meta_Class(FrameToScreen)

    FrameToScreen();
    virtual ~FrameToScreen();

    virtual void setup(std::vector<ImageSampler::Ptr>& input, std::vector<ImageSampler::Ptr>& output);

    virtual void apply(GlRenderer* renderer);

protected:

    int _subpassCount;
    Graph::Ptr _graph;
    RenderTarget::Ptr _frame;
};

IMPGEARS_END

#endif // IMP_FRAME_TO_SCREEN_H
