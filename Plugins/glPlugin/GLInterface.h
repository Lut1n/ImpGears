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
	
	virtual Data::Ptr load(const Geometry* geo);
	
	virtual Data::Ptr load(const TextureSampler* samper);
	
	virtual Data::Ptr load(const ReflexionModel* program);
	
	virtual void update(Data::Ptr data, const TextureSampler* sampler);
	
	virtual void bind(Data::Ptr data);
	
	virtual void init(Target* target);
	
	virtual void unbind(Target* target);
	
	virtual void bringBack(Image::Ptr img, Data::Ptr data, int n = 0);
	
	virtual void draw(Data::Ptr data);
	
	virtual void update(Data::Ptr d, const Uniform::Ptr& uniform);
	
};

IMPGEARS_END

#endif // GLINTERFACE_H_INCLUDED
