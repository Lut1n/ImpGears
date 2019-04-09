#include <SceneGraph/Sampler.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
Sampler::Sampler()
	: _smooth(false)
	, _repeated(false)
	, _hasMipmap(false)
	, _maxMipmapLvl(1000)
{
	_d = NULL;
}

//--------------------------------------------------------------
Sampler::~Sampler()
{
}

//--------------------------------------------------------------
void Sampler::setSource(const Image::Ptr& src)
{
	_srcImage = src;
}

//--------------------------------------------------------------
Image::Ptr Sampler::getSource() const
{
	return _srcImage;
}

//--------------------------------------------------------------
void Sampler::enableSmooth(bool enable)
{
	_smooth = enable;
}

//--------------------------------------------------------------
void Sampler::enableRepeated(bool enable)
{
	_repeated = enable;
}

//--------------------------------------------------------------
void Sampler::enableMipmap(bool enable)
{
	_hasMipmap = enable;
}

//--------------------------------------------------------------
bool Sampler::hasSmoothEnable() const
{
	return _smooth;
}

//--------------------------------------------------------------
bool Sampler::hasRepeatedEnable() const
{
	return _repeated;
}

//--------------------------------------------------------------
bool Sampler::hasMipmapEnable() const
{
	return _hasMipmap;
}

//--------------------------------------------------------------
void Sampler::setMaxMipmapLvl(int maxLvl)
{
	_maxMipmapLvl = maxLvl;
}

//--------------------------------------------------------------
int Sampler::getMaxMipmapLvl() const
{
	return _maxMipmapLvl;
}

IMPGEARS_END
