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

    virtual void setup();
    virtual void apply(GlRenderer* renderer);

protected:
    int _subpassCount;
    Graph::Ptr _graph;
    RenderTarget::Ptr _frames[2];
};

IMPGEARS_END

#endif // IMP_BLOOM_FX_H
