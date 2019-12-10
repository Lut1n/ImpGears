#ifndef IMP_COPY_FRAME_H
#define IMP_COPY_FRAME_H

#include <OGLPlugin/Pipeline.h>

IMPGEARS_BEGIN

struct GlRenderer;

class IMP_API CopyFrame : public FrameOperation
{
public:
    Meta_Class(CopyFrame)

    CopyFrame();
    virtual ~CopyFrame();

    virtual void setup();
    virtual void apply(GlRenderer* renderer);

protected:
    Graph::Ptr _graph;
    RenderTarget::Ptr _frame;
};

IMPGEARS_END

#endif // IMP_COPY_FRAME_H
