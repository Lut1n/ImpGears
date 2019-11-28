#ifndef IMP_BLOOM_FX_H
#define IMP_BLOOM_FX_H

#include <Core/Object.h>

#include <SceneGraph/Graph.h>
#include <SceneGraph/ReflexionModel.h>
#include <Renderer/RenderTarget.h>


IMPGEARS_BEGIN

struct GlRenderer;

class IMP_API BloomFX : public Object
{
public:

    Meta_Class(BloomFX)

    BloomFX();
    virtual ~BloomFX();

    void setup(int subpassCount, Vec4 viewport);

    void bind(GlRenderer* renderer, int subpassID = 0);

    void process(GlRenderer* renderer, int subpassID);

    void apply(GlRenderer* renderer, const Graph::Ptr& g);

protected:

    int _subpassCount;
    Graph::Ptr _graph;
    RenderTarget::Ptr _frames[2];
    ReflexionModel::Ptr _shader;
};

IMPGEARS_END

#endif // IMP_BLOOM_FX_H
