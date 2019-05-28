#ifndef IMP_RENDERPLUGIN_H
#define IMP_RENDERPLUGIN_H

#include <Core/Object.h>
#include <Core/Vec4.h>
#include <Graphics/Image.h>
#include <Renderer/Uniform.h>

#include <map>

IMPGEARS_BEGIN 

class ClearNode;
class State;
class Geometry;
class TextureSampler;
class RenderTarget;
class ReflexionModel;

class IMP_API RenderPlugin : public Object
{
public:
	
	Meta_Class(RenderPlugin);
	
	enum Type { Ty_Vbo, Ty_Tex, Ty_Shader, Ty_Tgt };
	
	struct Data : public Object { Meta_Class(Data) Type ty; };
	
	virtual void init() = 0;
	
	virtual void apply(const ClearNode* clear) = 0;
	
	virtual int load(const Geometry* geo) = 0;
	
	virtual int load(const ImageSampler* sampler) = 0;
	
	virtual int load(const ReflexionModel* program) = 0;
	
	virtual void update(const ImageSampler* sampler) = 0;
	
	virtual void bind(RenderTarget* target) = 0;
	
	virtual void bind(ReflexionModel* reflexion) = 0;
	
	virtual void bind(Geometry* geo) = 0;
	
	virtual void bind(ImageSampler* geo) = 0;
	
	virtual void init(RenderTarget* target) = 0;
	
	virtual void unbind(RenderTarget* target) = 0;
	
	virtual void bringBack(ImageSampler* sampler) = 0;
	
	virtual void draw(Geometry* geo) = 0;
	
	virtual void update(ReflexionModel* reflexion, const Uniform::Ptr& uniform) = 0;
	
	virtual void setCulling(int mode) = 0;
	
	virtual void setBlend(int mode) = 0;
	
	virtual void setLineW(float lw) = 0;
	
	virtual void setViewport(Vec4 vp) = 0;
	
	virtual void setDepthTest(int mode) = 0;
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
