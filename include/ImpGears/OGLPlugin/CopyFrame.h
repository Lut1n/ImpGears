#ifndef IMP_COPY_FRAME_H
#define IMP_COPY_FRAME_H

#include <OGLPlugin/Pipeline.h>
#include <OGLPlugin/Export.h>

IMPGEARS_BEGIN

struct GlRenderer;

class IG_BKND_GL_API CopyFrame : public FrameOperation
{
public:
    Meta_Class(CopyFrame)

    CopyFrame();
    virtual ~CopyFrame();

    virtual void setup(const Vec4& vp);
    virtual void apply(GlRenderer* renderer, bool skip = false);

protected:
    Graph::Ptr _graph;
    RenderTarget::Ptr _frame;
    
    RenderQueue::Ptr _queue;
};

IMPGEARS_END

#endif // IMP_COPY_FRAME_H
