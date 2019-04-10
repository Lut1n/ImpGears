#ifndef GLINTERFACE_H_INCLUDED
#define GLINTERFACE_H_INCLUDED

#include <SceneGraph/RefactoInterface.h>

IMPGEARS_BEGIN

struct GLInterface : public RefactoInterface
{
	virtual void init();
	
	virtual void apply(const ClearNode* clear);
	
	virtual void setCulling(int mode);
	virtual void setBlend(int mode);
	virtual void setLineW(float lw);
	virtual void setViewport(Vec4 vp);
	virtual void setDepthTest(int mode);
	
	virtual Data* load(const Geometry* geo);
	
	virtual Data* load(const Sampler* samper);
	
	virtual Data* load(const std::string& vert, const std::string& frag);
	
	virtual void update(Data* data, const Sampler* sampler);
	
	virtual void bind(Data* data);
	
	virtual void init(Target* target);
	virtual void bringBack(Image::Ptr img, Data* data, int n = 0);
	
	virtual void draw(Data* data);
	
	virtual void update(Data* d, const Uniform* uniform);
	
};

IMPGEARS_END

#endif // GLINTERFACE_H_INCLUDED
