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
};

IMPGEARS_END

#endif // IMP_LIGHTING_MODEL_H
