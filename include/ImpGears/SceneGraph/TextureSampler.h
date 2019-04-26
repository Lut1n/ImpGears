#ifndef IMP_TEXTURE_SAMPLER_H
#define IMP_TEXTURE_SAMPLER_H

#include <Core/Object.h>
#include <Graphics/Sampler.h>

#include <SceneGraph/RenderPlugin.h>

IMPGEARS_BEGIN

class IMP_API TextureSampler : public ImageSampler
{
public:
	
	Meta_Class(TextureSampler);
	
	TextureSampler();
	virtual ~TextureSampler();
	
	void setInternalSrc(int w, int h, int chnl);
	
	void enableMipmap(bool enable);
	bool hasMipmapEnable() const;
	
	void setMaxMipmapLvl(int maxLvl);
	int getMaxMipmapLvl() const;
	
	RenderPlugin::Data::Ptr _d;
	
protected:
	
	bool _hasMipmap;
	int _maxMipmapLvl;
};

IMPGEARS_END

#endif // IMP_TEXTURE_SAMPLER_H
