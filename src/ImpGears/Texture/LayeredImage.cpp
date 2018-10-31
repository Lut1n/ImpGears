#include <Texture/LayeredImage.h>

#include <cstdlib>
#include <cstring>
#include <iostream>

IMPGEARS_BEGIN


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
		if(chnlSize == sizeof(b8))
			_layers[i] = ChannelLayer<b8>::create(Dimension<2>(w,h));
		if(chnlSize == sizeof(b16))
			_layers[i] = ChannelLayer<b16>::create(Dimension<2>(w,h));
		if(chnlSize == sizeof(b32))
			_layers[i] = ChannelLayer<b32>::create(Dimension<2>(w,h));
		if(chnlSize == sizeof(b64))
			_layers[i] = ChannelLayer<b64>::create(Dimension<2>(w,h));
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
		if(chnls[i] == sizeof(b8))
			_layers[i] = ChannelLayer<b8>::create(Dimension<2>(w,h));
		if(chnls[i] == sizeof(b16))
			_layers[i] = ChannelLayer<b16>::create(Dimension<2>(w,h));
		if(chnls[i] == sizeof(b32))
			_layers[i] = ChannelLayer<b32>::create(Dimension<2>(w,h));
		if(chnls[i] == sizeof(b64))
			_layers[i] = ChannelLayer<b64>::create(Dimension<2>(w,h));
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
	if(ls == sizeof(b8)) return getLayer<b8>(ch)->get(Dimension<2>(x,y));
	if(ls == sizeof(b16)) return getLayer<b16>(ch)->get(Dimension<2>(x,y));
	if(ls == sizeof(b32)) return getLayer<b32>(ch)->get(Dimension<2>(x,y));
	if(ls == sizeof(b64)) return getLayer<b64>(ch)->get(Dimension<2>(x,y));
	
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
	if(ls == sizeof(b8)) getLayer<b8>(ch)->set(Dimension<2>(x,y),val);
	if(ls == sizeof(b16)) getLayer<b16>(ch)->set(Dimension<2>(x,y),val);
	if(ls == sizeof(b32)) getLayer<b32>(ch)->set(Dimension<2>(x,y),val);
	if(ls == sizeof(b64)) getLayer<b64>(ch)->set(Dimension<2>(x,y),val);
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
		if(ls == sizeof(b8))
			getLayer<b8>(i)->set(*src.getLayer<b8>(i),dstRect);
		if(ls == sizeof(b16))
			getLayer<b16>(i)->set(*src.getLayer<b16>(i),dstRect);
		if(ls == sizeof(b32))
			getLayer<b32>(i)->set(*src.getLayer<b32>(i),dstRect);
		if(ls == sizeof(b64))
			getLayer<b64>(i)->set(*src.getLayer<b64>(i),dstRect);
    }
}

//--------------------------------------------------------------
void LayeredImage::fill(const Vec4& px)
{
	for(unsigned int i=0; i<_layers.size(); ++i)
	{
		int ls = _layers[i]->size();
		if(ls == sizeof(b8)) getLayer<b8>(i)->fill(px[i]);
		if(ls == sizeof(b16)) getLayer<b16>(i)->fill(px[i]);
		if(ls == sizeof(b32)) getLayer<b32>(i)->fill(px[i]);
		if(ls == sizeof(b64)) getLayer<b64>(i)->fill(px[i]);
	}
}

IMPGEARS_END
