#ifndef IMP_AMBIENT_OCCLUSION_H
#define IMP_AMBIENT_OCCLUSION_H

#include <OGLPlugin/Pipeline.h>

IMPGEARS_BEGIN

struct GlRenderer;

class IMP_API AmbientOcclusion : public FrameOperation
{
public:
    Meta_Class(AmbientOcclusion)

    AmbientOcclusion();
    virtual ~AmbientOcclusion();

    virtual void setup(const Vec4& vp);
    virtual void apply(GlRenderer* renderer, bool skip = false);

protected:

    Matrix4 _proj;
    Graph::Ptr _graph;
    Graph::Ptr _fillingGraph;
    RenderTarget::Ptr _frame;
    
    RenderQueue::Ptr _queue;
    RenderQueue::Ptr _fillingQueue;
};

IMPGEARS_END

#endif // IMP_AMBIENT_OCCLUSION_H
