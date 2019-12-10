#ifndef IMP_BLEND_ALL_H
#define IMP_BLEND_ALL_H

#include <OGLPlugin/Pipeline.h>

IMPGEARS_BEGIN

struct GlRenderer;

class IMP_API BlendAll : public FrameOperation
{
public:
    enum Type { Max = 0, Mix, Min, Mult };

    Meta_Class(BlendAll)

    BlendAll(Type t = Max);
    virtual ~BlendAll();

    virtual void setup();
    virtual void apply(GlRenderer* renderer);

protected:
    Type _type;
    Graph::Ptr _graph;
    RenderTarget::Ptr _frame;
};

IMPGEARS_END

#endif // IMP_BLEND_ALL_H
