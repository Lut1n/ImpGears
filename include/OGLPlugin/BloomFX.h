#ifndef IMP_BLOOM_FX_H
#define IMP_BLOOM_FX_H

#include <Core/Object.h>

#include <OGLPlugin/FxRenderPass.h>

IMPGEARS_BEGIN

struct GlRenderer;

class IMP_API BloomFX : public FxRenderPass
{
public:

    Meta_Class(BloomFX)

    BloomFX();
    virtual ~BloomFX();

    void setup(std::vector<ImageSampler::Ptr>& input, std::vector<ImageSampler::Ptr>& output);

    // void setup(int subpassCount, Vec4 viewport);

    void bind(GlRenderer* renderer, int subpassID = 0);

    void process(GlRenderer* renderer, int subpassID);

    void apply(GlRenderer* renderer);

protected:

    int _subpassCount;
    Graph::Ptr _graph;
    RenderTarget::Ptr _frames[2];
};

IMPGEARS_END

#endif // IMP_BLOOM_FX_H
