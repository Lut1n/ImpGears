#ifndef GLINTERFACE_H_INCLUDED
#define GLINTERFACE_H_INCLUDED

#include <Plugins/RenderPlugin.h>

IMPGEARS_BEGIN

class IMP_API GlPlugin : public RenderPlugin
{
public:

    Meta_Class(GlPlugin);

    virtual void init();

    virtual void apply(ClearNode::Ptr& clear);

    virtual void setCulling(int mode);
    virtual void setBlend(int mode);
    virtual void setLineW(float lw);
    virtual void setViewport(Vec4 vp);
    virtual void setDepthTest(int mode);

    virtual int load(Geometry::Ptr& geo);
    virtual int load(ImageSampler::Ptr& sampler);
    virtual int load(CubeMapSampler::Ptr& sampler);
    virtual int load(ReflexionModel::Ptr& program);

    virtual void update(ImageSampler::Ptr& sampler);

    virtual void bind(RenderTarget::Ptr& target);
    virtual void bind(ReflexionModel::Ptr& reflexion);
    virtual void bind(Geometry::Ptr& geo);
    virtual void bind(ImageSampler::Ptr& geo);
    virtual void bind(CubeMapSampler::Ptr& geo);

    virtual void init(RenderTarget::Ptr& target);
    virtual void unbind();

    virtual void bringBack(ImageSampler::Ptr& sampler);

    virtual void draw(Geometry::Ptr& geo);

    virtual void update(ReflexionModel::Ptr& reflexion, Uniform::Ptr& uniform);

    virtual SceneRenderer::Ptr getRenderer();

    struct Priv;

    static Priv* s_internalState;
};

IMPGEARS_END

#endif // GLINTERFACE_H_INCLUDED
