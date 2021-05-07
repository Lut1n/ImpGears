#ifndef IMP_BLEND_ALL_H
#define IMP_BLEND_ALL_H

#include <RendererGL/Pipeline.h>
#include <RendererGL/Export.h>

IMPGEARS_BEGIN

struct GlRenderer;

class IG_BKND_GL_API BlendAll : public FrameOperation
{
public:
    enum Type { Max = 0, Mix, Min, Mult };

    Meta_Class(BlendAll)

    BlendAll(Type t = Max);
    virtual ~BlendAll();

    virtual void setup(const Vec4& vp);
    virtual void apply(GlRenderer* renderer, bool skip = false);

protected:
    Type _type;
    Graph::Ptr _graph;
    RenderTarget::Ptr _frame;
    
    RenderQueue::Ptr _queue;
};

IMPGEARS_END

#endif // IMP_BLEND_ALL_H
