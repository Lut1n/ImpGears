#include <Renderer/TextureSampler.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
TextureSampler::TextureSampler()
	: _hasMipmap(false)
	, _maxMipmapLvl(1000)
{
	setInternalSrc(128,128,4);
	_d = NULL;
}

//--------------------------------------------------------------
TextureSampler::~TextureSampler()
{
}

//--------------------------------------------------------------
void TextureSampler::setInternalSrc(int w, int h, int chnl)
{
	Image::Ptr src = Image::create(w,h,chnl);
	setSource(src);
}

//--------------------------------------------------------------
void TextureSampler::enableMipmap(bool enable)
{
	_hasMipmap = enable;
}

//--------------------------------------------------------------
bool TextureSampler::hasMipmapEnable() const
{
	return _hasMipmap;
}

//--------------------------------------------------------------
void TextureSampler::setMaxMipmapLvl(int maxLvl)
{
	_maxMipmapLvl = maxLvl;
}

//--------------------------------------------------------------
int TextureSampler::getMaxMipmapLvl() const
{
	return _maxMipmapLvl;
}

IMPGEARS_END
