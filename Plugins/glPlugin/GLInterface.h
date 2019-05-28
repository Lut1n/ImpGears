#ifndef GLINTERFACE_H_INCLUDED
#define GLINTERFACE_H_INCLUDED

#include <Renderer/RenderPlugin.h>

IMPGEARS_BEGIN

class IMP_API GlPlugin : public RenderPlugin
{
public:
	
	Meta_Class(GlPlugin);
	
	virtual void init();
	
	virtual void apply(const ClearNode* clear);
	
	virtual void setCulling(int mode);
	virtual void setBlend(int mode);
	virtual void setLineW(float lw);
	virtual void setViewport(Vec4 vp);
	virtual void setDepthTest(int mode);
	
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
	
	struct Priv;
	
	static Priv* s_internalState;
};

IMPGEARS_END

#endif // GLINTERFACE_H_INCLUDED
