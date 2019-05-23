#include <Graphics/Sampler.h>

#include <Core/Math.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
inline float applyMode(float& c, ImageSampler::Mode mode)
{
	if(mode == ImageSampler::Mode_Mirror)
	{
		if(c < 0.0) c = -c;
		else if(c >= 1.0)  c = 2.0-c;
	}
	else if(mode == ImageSampler::Mode_Repeat)
	{
		while(c < 0.0) c += 1.0;
		while(c >= 1.0) c -= 1.0;
	}
	
	return clamp(c,0.f,1.f);
};

//--------------------------------------------------------------
ImageSampler::ImageSampler(Image::Ptr src, Mode mode)
	: _mode(mode)
{
	setSource(src);
}

//--------------------------------------------------------------
void ImageSampler::setSource(Image::Ptr src)
{
	_src = src;
	
	if(_src != nullptr)
		_dims=Vec2((float)_src->width(),(float)_src->height());
	else
		_dims=Vec2(0.0);
}

//--------------------------------------------------------------
Image::Ptr ImageSampler::getSource() const
{
	return _src;
}

//--------------------------------------------------------------
Vec4 ImageSampler::operator()(const Vec2& uv)
{
	return get(uv);
}

//--------------------------------------------------------------
Vec4 ImageSampler::get(const Vec2& uv_orig)
{
	if(_src == nullptr) return Vec4(1.0);
	
	Vec2 uv = uv_orig;
	for(int i=0;i<2;++i)uv[i]=applyMode(uv[i],_mode);
	Vec2 coords = uv * _dims;
	
	Vec4 px;
	if( getInterpo() == Interpo_Nearest )
	{
		px = _src->getPixel( coords[0],coords[1] );
	}
	else
	{
		Vec2 floorUV, fracUV;
		for(int i=0;i<2;++i) { floorUV[i]=std::floor(coords[i]); fracUV[i]=coords[i]-floorUV[i]; }
		
		Vec4 px00 = _src->getPixel( floorUV[0]+0.0,floorUV[1]+0.0 );
		Vec4 px01 = _src->getPixel( floorUV[0]+0.0,floorUV[1]+1.0 );
		Vec4 px10 = _src->getPixel( floorUV[0]+1.0,floorUV[1]+0.0 );
		Vec4 px11 = _src->getPixel( floorUV[0]+1.0,floorUV[1]+1.0 );
		
		if( getInterpo() == Interpo_Smooth )
			for(int i=0;i<2;++i) fracUV[i]=smoothstep(0.0,1.0,fracUV[i]);
		
		px = mix2d(px00, px10, px01, px11, fracUV[0], fracUV[1]);
	}
	return px / 255.0;
}

//--------------------------------------------------------------
void ImageSampler::set(const Vec2& uv_orig, const Vec4& color)
{
	if(_src == nullptr) return;
	
	Vec2 uv = uv_orig;
	for(int i=0;i<2;++i)uv[i]=applyMode(uv[i],_mode);
	Vec2 coords = uv * _dims;
	Vec4 px = dotClamp(color, 0.f, 1.f) * 255.0;
	_src->setPixel( coords[0],coords[1], px );
}

//--------------------------------------------------------------
void ImageSampler::set(float u, float v, const Vec4& color)
{
	set(Vec2(u,v),color);
}

//--------------------------------------------------------------
Vec4 ImageSampler::get(float u, float v)
{
	return get(Vec2(u,v));
}

//--------------------------------------------------------------
void ImageSampler::setMode(Mode mode)
{
	_mode = mode;
}

//--------------------------------------------------------------
ImageSampler::Mode ImageSampler::getMode() const
{
	return _mode;
}

//--------------------------------------------------------------
void ImageSampler::setInternalSrc(int w, int h, int chnl)
{
	Image::Ptr src = Image::create(w,h,chnl);
	setSource(src);
}

IMPGEARS_END
