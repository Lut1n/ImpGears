#include <Texture/LayeredImage.h>

#include <cstdlib>
#include <cstring>
#include <iostream>

IMPGEARS_BEGIN

int LayeredImage::b8 = sizeof(LayeredImage::b8_t);
int LayeredImage::b16 = sizeof(LayeredImage::b16_t);
int LayeredImage::b32 = sizeof(LayeredImage::b32_t);
int LayeredImage::b64 = sizeof(LayeredImage::b64_t);

//--------------------------------------------------------------
LayeredImage::LayeredImage()
	: _width(0)
    , _height(0)
{
}

//--------------------------------------------------------------
LayeredImage::~LayeredImage()
{
    destroy();
}

//--------------------------------------------------------------
void LayeredImage::build(int w, int h, int chnlCount, int chnlSize)
{
    destroy();

	_width = w;
	_height = h;
	_layers.resize(chnlCount);
	
	for(unsigned int i=0; i<_layers.size();++i)
	{
		if(chnlSize == b8)
			_layers[i] = ChannelLayer<b8_t>::create(Dimension<2>(w,h));
		if(chnlSize == b16)
			_layers[i] = ChannelLayer<b16_t>::create(Dimension<2>(w,h));
		if(chnlSize == b32)
			_layers[i] = ChannelLayer<b32_t>::create(Dimension<2>(w,h));
		if(chnlSize == b64)
			_layers[i] = ChannelLayer<b64_t>::create(Dimension<2>(w,h));
	}
}

//--------------------------------------------------------------
void LayeredImage::build(int w, int h, const std::vector<int>& chnls)
{
    destroy();

	_width = w;
	_height = h;
	_layers.resize(chnls.size());
	
	for(unsigned int i=0; i<_layers.size();++i)
	{
		if(chnls[i] == b8)
			_layers[i] = ChannelLayer<b8_t>::create(Dimension<2>(w,h));
		if(chnls[i] == b16)
			_layers[i] = ChannelLayer<b16_t>::create(Dimension<2>(w,h));
		if(chnls[i] == b32)
			_layers[i] = ChannelLayer<b32_t>::create(Dimension<2>(w,h));
		if(chnls[i] == b64)
			_layers[i] = ChannelLayer<b64_t>::create(Dimension<2>(w,h));
	}
}

//--------------------------------------------------------------
void LayeredImage::destroy()
{
    _width = 0;
	_height = 0;
	_layers.clear();
}

//--------------------------------------------------------------
void LayeredImage::resize(int w, int h, int xrel, int yrel)
{ 
	// todo
}

//--------------------------------------------------------------
Vec4 LayeredImage::get(int x, int y) const
{
	Vec4 result;
	for(unsigned int i=0;i<_layers.size();++i) result[i] = get(x,y,i);
	return result;
}

//--------------------------------------------------------------
std::uint64_t LayeredImage::get(int x, int y, int ch) const
{
	int ls = _layers[ch]->size();
	if(ls == b8) return getLayer<b8_t>(ch)->get(Dimension<2>(x,y));
	if(ls == b16) return getLayer<b16_t>(ch)->get(Dimension<2>(x,y));
	if(ls == b32) return getLayer<b32_t>(ch)->get(Dimension<2>(x,y));
	if(ls == b64) return getLayer<b64_t>(ch)->get(Dimension<2>(x,y));
	
	return 0;
}

//--------------------------------------------------------------
void LayeredImage::set(int x, int y, const Vec4& px)
{
	for(unsigned int i=0;i<_layers.size();++i) set(x,y,i,px[i]);
}

//--------------------------------------------------------------
void LayeredImage::set(int x, int y, int ch, std::uint64_t val)
{
	int ls = _layers[ch]->size();
	if(ls == b8) getLayer<b8_t>(ch)->set(Dimension<2>(x,y),val);
	if(ls == b16) getLayer<b16_t>(ch)->set(Dimension<2>(x,y),val);
	if(ls == b32) getLayer<b32_t>(ch)->set(Dimension<2>(x,y),val);
	if(ls == b64) getLayer<b64_t>(ch)->set(Dimension<2>(x,y),val);
}

//--------------------------------------------------------------
void LayeredImage::clone(const LayeredImage& other)
{
	destroy();
	build(other._width, other._height, other.chnlSize());
	for(unsigned int i=0; i<_layers.size();++i)
	{
		*(_layers[i]) = *( other._layers[i] );
	}
}

//--------------------------------------------------------------
void LayeredImage::draw(const LayeredImage& src, const IntRect& dstRect)
{
    for(unsigned int i=0; i<_layers.size();++i)
    {
		int ls = _layers[i]->size();
		if(ls == b8)
			getLayer<b8_t>(i)->set(*src.getLayer<b8_t>(i),dstRect);
		if(ls == b16)
			getLayer<b16_t>(i)->set(*src.getLayer<b16_t>(i),dstRect);
		if(ls == b32)
			getLayer<b32_t>(i)->set(*src.getLayer<b32_t>(i),dstRect);
		if(ls == b64)
			getLayer<b64_t>(i)->set(*src.getLayer<b64_t>(i),dstRect);
    }
}

//--------------------------------------------------------------
void LayeredImage::fill(const Vec4& px)
{
	for(unsigned int i=0; i<_layers.size(); ++i)
	{
		int ls = _layers[i]->size();
		if(ls == b8) getLayer<b8_t>(i)->fill(px[i]);
		if(ls == b16) getLayer<b16_t>(i)->fill(px[i]);
		if(ls == b32) getLayer<b32_t>(i)->fill(px[i]);
		if(ls == b64) getLayer<b64_t>(i)->fill(px[i]);
	}
}

IMPGEARS_END
