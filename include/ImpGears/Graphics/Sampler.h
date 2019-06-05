#ifndef IMP_SAMPLER_H
#define IMP_SAMPLER_H

#include <Core/Object.h>
#include <Core/Vec2.h>
#include <Core/Vec3.h>
#include <Graphics/Image.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
template<typename TyIn, typename TyOut>
class IMP_API Sampler
{
public:
	
	Meta_Class(Sampler)
	
	enum Interpo
	{
		Interpo_Nearest,
		Interpo_Linear,
		Interpo_Smooth
	};
	
	Sampler(Interpo interpo = Interpo_Nearest) : _interpo(interpo) {}
	
	virtual TyOut operator()(const TyIn& t) = 0;
	void setInterpo(Interpo interpo) {_interpo = interpo;}
	Interpo getInterpo() const {return _interpo;}
	
	Interpo _interpo;
};

//--------------------------------------------------------------
class IMP_API ImageSampler : public Sampler< Vec2, Vec4 >
{
public:
	
	enum Mode
	{
		Mode_Clamp,
		Mode_Mirror,
		Mode_Repeat
	};
	
	Meta_Class(ImageSampler)
	
	ImageSampler(Image::Ptr src = nullptr, Mode mode = Mode_Clamp);
	ImageSampler(int w, int h, int chnl, const Vec4& color);
	void setSource(Image::Ptr src);
	Image::Ptr getSource() const;
	
	virtual Vec4 operator()(const Vec2& uv);
	
	void set(const Vec2& uv, const Vec4& color);
	void set(float u, float v, const Vec4& color);
	
	Vec4 get(const Vec2& uv);
	Vec4 get(float u, float v);
	
	void setMode(Mode mode);
	Mode getMode() const;
	
	void setInternalSrc(int w, int h, int chnl);
	
protected:
	
	Image::Ptr _src;
	Mode _mode;
	Vec2 _dims;
};

IMPGEARS_END

#endif // IMP_SAMPLER_H
