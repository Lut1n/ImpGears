#ifndef IMP_RENDERTARGET_H
#define IMP_RENDERTARGET_H

#include <ImpGears/Core/Object.h>
#include <ImpGears/Graphics/Sampler.h>
#include <ImpGears/Renderer/Export.h>

IMPGEARS_BEGIN

class IG_RENDERER_API RenderTarget : public Object
{
public:

    Meta_Class(RenderTarget);

    using FaceSampler = std::pair<CubeMapSampler::Ptr, int>;

    RenderTarget();
    virtual ~RenderTarget();

    void build(int w, int h, int count = 1, bool hasDepth = false, bool msaa = false);
    void build(const std::vector<ImageSampler::Ptr>& textures, bool hasDepth = false);
    void build(const FaceSampler& face, bool hasDepth = false);

    void setClearColors(const std::vector<Vec4>& clearColors);
    void setClearColor(int index, const Vec4& clearColor);
    const std::vector<Vec4>& getClearColors() const;
    const Vec4& getClearColor(int index) const;

    void clearTargets();

    void destroy();

    bool hasDepth() const;
    bool useFaceSampler() const;

    ImageSampler::Ptr get(int index);
    FaceSampler getFace();

    int width() const;
    int height() const;
    int count() const;

    void change();
    void update();

protected:

    std::vector<ImageSampler::Ptr> _targets;
    std::vector<Vec4> _clearColors;
    FaceSampler _faceTarget;
    bool _useFaceSampler;
    bool _hasDepthBuffer;
    bool _hasChanged;
};

IMPGEARS_END

#endif // IMP_RENDERTARGET_H
