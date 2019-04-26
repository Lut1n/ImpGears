#ifndef IMP_CPURENDER_PLUGIN_H
#define IMP_CPURENDER_PLUGIN_H

#include <Core/Object.h>
#include <SceneGraph/RenderPlugin.h>

IMPGEARS_BEGIN

class IMP_API CpuRenderPlugin : public RenderPlugin
{
public:
	
	Meta_Class(CpuRenderPlugin);
	
	virtual void init();
	
	virtual void apply(const ClearNode* clear);
	
	virtual Data::Ptr load(const Geometry* geo);
	
	virtual Data::Ptr load(const TextureSampler* sampler);
	
	virtual Data::Ptr load(const ShaderDsc* program);
	
	virtual void update(Data::Ptr data, const TextureSampler* sampler);
	
	virtual void bind(Data::Ptr data);
	
	virtual void init(Target* target);
	
	virtual void unbind(Target* target);
	
	virtual void bringBack(Image::Ptr img, Data::Ptr data, int n = 0);
	
	virtual void draw(Data::Ptr data);
	
	virtual void update(Data::Ptr d, const Uniform* uniform);
	
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
