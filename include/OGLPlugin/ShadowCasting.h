#ifndef IMP_SHADOW_CASTING_FX_H
#define IMP_SHADOW_CASTING_FX_H

#include <OGLPlugin/Pipeline.h>
#include <OGLPlugin/Export.h>

IMPGEARS_BEGIN

class GlRenderer;

class IG_BKND_GL_API ShadowCasting : public FrameOperation
{
public:
    Meta_Class(ShadowCasting)

    ShadowCasting();
    virtual ~ShadowCasting();

    virtual void setup(const Vec4& vp);
    virtual void apply(GlRenderer* renderer, bool skip = false);

    void setSampleCount(int samples){_samples=samples;}

protected:
    Graph::Ptr _graph;
    Graph::Ptr _fillingGraph;
    RenderTarget::Ptr _frame;
    
    RenderQueue::Ptr _queue;
    RenderQueue::Ptr _fillingQueue;

    int _samples;
};

IMPGEARS_END

#endif // IMP_SHADOW_CASTING_FX_H
