#ifndef IMP_RENDERTARGET_H
#define IMP_RENDERTARGET_H

#include <Core/Object.h>
#include <Graphics/Sampler.h>

IMPGEARS_BEGIN

class IMP_API RenderTarget : public Object
{
public:

    Meta_Class(RenderTarget);

    using FaceSampler = std::pair<CubeMapSampler::Ptr, int>;

    RenderTarget();
    virtual ~RenderTarget();

    void build(int w, int h, int count = 1, bool hasDepth = false);
    void build(const std::vector<ImageSampler::Ptr>& textures, bool hasDepth = false);
    void build(const FaceSampler& face, bool hasDepth = false);

    void destroy();

    bool hasDepth() const;
    bool useFaceSampler() const;

    ImageSampler::Ptr get(int n);
    FaceSampler getFace();

    int width() const;
    int height() const;
    int count() const;

    void change();
    void update();

protected:

    std::vector<ImageSampler::Ptr> _targets;
    FaceSampler _faceTarget;
    bool _useFaceSampler;
    bool _hasDepthBuffer;
    bool _hasChanged;
};

IMPGEARS_END

#endif // IMP_RENDERTARGET_H
