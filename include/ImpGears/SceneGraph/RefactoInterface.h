#ifndef IMP_RI_H
#define IMP_RI_H

#include <Core/Object.h>
#include <Core/Vec4.h>

IMPGEARS_BEGIN

class ClearNode;
class State;
class Geometry;
class Image;
class Uniform;

struct RefactoInterface
{
	enum Type { Ty_Vbo, Ty_Tex, Ty_Shader, Ty_Tgt };
	struct Data { Type ty; };
	
	virtual void init() = 0;
	
	virtual void apply(const ClearNode* clear) = 0;
	
	virtual Data* load(const Geometry* geo) = 0;
	
	virtual Data* load(const Image* img) = 0;
	
	virtual Data* load(const std::string& vert, const std::string& frag) = 0;
	
	virtual void update(Data* data, const Image* img) = 0;
	
	virtual void bind(Data* data) = 0;
	
	virtual void draw(Data* data) = 0;
	
	virtual void update(Data* d, const Uniform* uniform) = 0;
	
	virtual void setCulling(int mode) = 0;
	virtual void setBlend(int mode) = 0;
	virtual void setLineW(float lw) = 0;
	virtual void setViewport(Vec4 vp) = 0;
	virtual void setDepthTest(int mode) = 0;
};

IMPGEARS_END

#endif // IMP_RI_H
