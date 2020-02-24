#ifndef IMP_BLOOM_FX_H
#define IMP_BLOOM_FX_H

#include <OGLPlugin/Pipeline.h>

IMPGEARS_BEGIN

struct GlRenderer;

class IMP_API BloomFX : public FrameOperation
{
public:
    Meta_Class(BloomFX)

    BloomFX();
    virtual ~BloomFX();

    void bind(GlRenderer* renderer, int subpassID = 0);
    void process(GlRenderer* renderer, int subpassID);

    virtual void setup(const Vec4& vp);
    virtual void apply(GlRenderer* renderer, bool skip = false);

protected:
    int _subpassCount;
    Graph::Ptr _graph;
    Graph::Ptr _copyGraph;
    RenderTarget::Ptr _frames[2];
    
    RenderQueue::Ptr _queue;
    RenderQueue::Ptr _copyQueue;
};

IMPGEARS_END

#endif // IMP_BLOOM_FX_H
