#ifndef IMP_RENDERPLUGIN_H
#define IMP_RENDERPLUGIN_H

#include <ImpGears/Core/Object.h>
#include <ImpGears/Core/Vec4.h>
#include <ImpGears/Graphics/Image.h>
#include <ImpGears/Graphics/Uniform.h>
#include <ImpGears/Renderer/RenderTarget.h>
#include <ImpGears/Renderer/SceneRenderer.h>

#include <ImpGears/SceneGraph/ClearNode.h>
#include <ImpGears/SceneGraph/State.h>
#include <ImpGears/Geometry/Geometry.h>
#include <ImpGears/Graphics/Sampler.h>
#include <ImpGears/SceneGraph/ReflexionModel.h>

#include <map>

IMPGEARS_BEGIN 

class IMP_API RenderPlugin : public Object
{
public:

    Meta_Class(RenderPlugin);

    enum Type { Ty_Vbo, Ty_Tex, Ty_Shader, Ty_Tgt };

    struct Data : public Object { Meta_Class(Data) Type ty; };

    virtual void init() = 0;

    virtual void apply(ClearNode::Ptr& clear, int bufferIndex = -1) = 0;

    virtual int load(Geometry::Ptr& geo) = 0;

    virtual int load(ImageSampler::Ptr& sampler) = 0;
    virtual int load(CubeMapSampler::Ptr& sampler) = 0;

    virtual int load(ReflexionModel::Ptr& program) = 0;

    virtual void update(ImageSampler::Ptr& sampler) = 0;

    virtual void bind(RenderTarget::Ptr& target) = 0;

    virtual void bind(ReflexionModel::Ptr& reflexion) = 0;

    virtual void bind(Geometry::Ptr& geo) = 0;

    virtual void bind(ImageSampler::Ptr& sampler) = 0;
    virtual void bind(CubeMapSampler::Ptr& sampler) = 0;

    virtual void init(RenderTarget::Ptr& target) = 0;

    virtual void unbind() = 0;

    virtual void bringBack(ImageSampler::Ptr& sampler) = 0;

    virtual void draw(Geometry::Ptr& geo) = 0;

    virtual void update(ReflexionModel::Ptr& reflexion, Uniform::Ptr& uniform) = 0;

    virtual void setCulling(int mode) = 0;

    virtual void setBlend(int mode) = 0;

    virtual void setLineW(float lw) = 0;

    virtual void setViewport(Vec4 vp) = 0;

    virtual void setDepthTest(int mode) = 0;

    virtual SceneRenderer::Ptr getRenderer() = 0;

    virtual void unload(Geometry::Ptr& geo) = 0;
    virtual void unload(ImageSampler::Ptr& sampler) = 0;
    virtual void unload(CubeMapSampler::Ptr& sampler) = 0;
    virtual void unload(RenderTarget::Ptr& rt) = 0;
    virtual void unload(ReflexionModel::Ptr& shader) = 0;

    virtual void unloadUnused() = 0;
};


class IMP_API PluginManager : public Object
{
public:

    Meta_Class(PluginManager);

    typedef RenderPlugin::Ptr (*LoadFunc)();

    static RenderPlugin::Ptr open(const std::string& name);

    static void close(RenderPlugin::Ptr& plugin);

protected:

    static std::map<RenderPlugin::Ptr,void*> _handlers;
};

IMPGEARS_END

#endif // IMP_RENDERPLUGIN_H
