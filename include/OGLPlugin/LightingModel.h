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

    virtual void setup();
    virtual void apply(GlRenderer* renderer);

protected:
    Graph::Ptr _graph;
    RenderTarget::Ptr _frame;
};

IMPGEARS_END

#endif // IMP_LIGHTING_MODEL_H
