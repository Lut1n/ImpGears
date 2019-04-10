#ifndef IMP_SAMPLER_H
#define IMP_SAMPLER_H

#include <Core/Object.h>
#include <Graphics/Image.h>

#include <SceneGraph/RefactoInterface.h>

IMPGEARS_BEGIN

class IMP_API Sampler : public Object
{
public:
	
	Meta_Class(Sampler);
	
	Sampler();
	virtual ~Sampler();
	
	void setInternalSrc(int w, int h, int chnl);
	void setSource(const Image::Ptr& src);
	Image::Ptr getSource() const;
	
	void enableSmooth(bool enable);
	void enableRepeated(bool enable);
	void enableMipmap(bool enable);
	
	bool hasSmoothEnable() const;
	bool hasRepeatedEnable() const;
	bool hasMipmapEnable() const;
	
	void setMaxMipmapLvl(int maxLvl);
	int getMaxMipmapLvl() const;
	
	RefactoInterface::Data* _d;
	
protected:
	Image::Ptr _srcImage;
	
	bool _smooth;
	bool _repeated;
	bool _hasMipmap;

	int _maxMipmapLvl;
};

IMPGEARS_END

#endif // IMP_SAMPLER_H
