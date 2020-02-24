#ifndef IMP_LIGHTING_MODEL_H
#define IMP_LIGHTING_MODEL_H

#include <OGLPlugin/Pipeline.h>

IMPGEARS_BEGIN

struct GlRenderer;

class IMP_API LightingModel : public FrameOperation
{
public:
    Meta_Class(LightingModel)

    LightingModel();
    virtual ~LightingModel();

    virtual void setup(const Vec4& vp);
    virtual void apply(GlRenderer* renderer, bool skip = false);

protected:
    Graph::Ptr _graph;
    Graph::Ptr _fillingGraph;
    RenderTarget::Ptr _frame;
    
    RenderQueue::Ptr _queue;
    RenderQueue::Ptr _fillingQueue;
};

IMPGEARS_END

#endif // IMP_LIGHTING_MODEL_H
