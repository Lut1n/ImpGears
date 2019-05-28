#ifndef IMP_CPURENDER_PLUGIN_H
#define IMP_CPURENDER_PLUGIN_H

#include <Core/Object.h>
#include <Renderer/RenderPlugin.h>

IMPGEARS_BEGIN

class IMP_API CpuRenderPlugin : public RenderPlugin
{
public:
	
	Meta_Class(CpuRenderPlugin);
	
	virtual void init();
	
	virtual void apply(ClearNode::Ptr& clear);
	
	virtual int load(Geometry::Ptr& geo);
	
	virtual int load(ImageSampler::Ptr& sampler);
	
	virtual int load(ReflexionModel::Ptr& program);
	
	virtual void update(ImageSampler::Ptr& sampler);
	
	virtual void bind(RenderTarget::Ptr& target);
	
	virtual void bind(ReflexionModel::Ptr& reflexion);
	
	virtual void bind(Geometry::Ptr& geo);
	
	virtual void bind(ImageSampler::Ptr& geo);
	
	virtual void init(RenderTarget::Ptr& target);
	
	virtual void unbind();
	
	virtual void bringBack(ImageSampler::Ptr& sampler);
	
	virtual void draw(Geometry::Ptr& geo);
	
	virtual void update(ReflexionModel::Ptr& reflexion, Uniform::Ptr& uniform);
	
	virtual void setCulling(int mode);
	
	virtual void setBlend(int mode);
	
	virtual void setLineW(float lw);
	
	virtual void setViewport(Vec4 vp);
	
	virtual void setDepthTest(int mode);
	
	struct Priv;
	
	static Priv* s_internalState;
};

IMPGEARS_END

#endif // IMP_CPURENDER_PLUGIN_H
