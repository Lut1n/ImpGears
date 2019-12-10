#ifndef IMP_SHADOW_CASTING_FX_H
#define IMP_SHADOW_CASTING_FX_H

#include <OGLPlugin/Pipeline.h>

IMPGEARS_BEGIN

struct GlRenderer;

class IMP_API ShadowCasting : public FrameOperation
{
public:
    Meta_Class(ShadowCasting)

    ShadowCasting();
    virtual ~ShadowCasting();

    virtual void setup();
    virtual void apply(GlRenderer* renderer);

protected:
    Graph::Ptr _graph;
    RenderTarget::Ptr _frame;
};

IMPGEARS_END

#endif // IMP_SHADOW_CASTING_FX_H
