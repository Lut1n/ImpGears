#ifndef IMP_RENDERPLUGIN_H
#define IMP_RENDERPLUGIN_H

#include <Core/Object.h>
#include <Core/Vec4.h>
#include <Graphics/Image.h>

#include <map>

IMPGEARS_BEGIN 

class ClearNode;
class State;
class Geometry;
class Sampler;
class Uniform;
class Target;

class IMP_API RenderPlugin : public Object
{
public:
	
	Meta_Class(RenderPlugin);
	
	enum Type { Ty_Vbo, Ty_Tex, Ty_Shader, Ty_Tgt };
	
	struct Data { Type ty; };
	
	virtual void init() = 0;
	
	virtual void apply(const ClearNode* clear) = 0;
	
	virtual Data* load(const Geometry* geo) = 0;
	
	virtual Data* load(const Sampler* sampler) = 0;
	
	virtual Data* load(const std::string& vert, const std::string& frag) = 0;
	
	virtual void update(Data* data, const Sampler* sampler) = 0;
	
	virtual void bind(Data* data) = 0;
	
	virtual void init(Target* target) = 0;
	
	virtual void bringBack(Image::Ptr img, Data* data, int n = 0) = 0;
	
	virtual void draw(Data* data) = 0;
	
	virtual void update(Data* d, const Uniform* uniform) = 0;
	
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
