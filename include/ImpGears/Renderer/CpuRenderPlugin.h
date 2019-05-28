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
	
	virtual void apply(const ClearNode* clear);
	
	virtual int load(const Geometry* geo);
	
	virtual int load(const ImageSampler* sampler);
	
	virtual int load(const ReflexionModel* program);
	
	virtual void update(const ImageSampler* sampler);
	
	virtual void bind(RenderTarget* target);
	
	virtual void bind(ReflexionModel* reflexion);
	
	virtual void bind(Geometry* geo);
	
	virtual void bind(ImageSampler* geo);
	
	virtual void init(RenderTarget* target);
	
	virtual void unbind(RenderTarget* target);
	
	virtual void bringBack(ImageSampler* sampler);
	
	virtual void draw(Geometry* geo);
	
	virtual void update(ReflexionModel* reflexion, const Uniform::Ptr& uniform);
	
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
