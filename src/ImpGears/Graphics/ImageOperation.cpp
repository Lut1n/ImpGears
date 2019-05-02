#include <Graphics/ImageOperation.h>

#include <Core/Math.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
void ImageOperation::execute(Image::Ptr& src)
{
	if(src == nullptr) return;
	
	Vec2 dim((float)src->width(),(float)src->height());
	ImageSampler sampler(src);
	
	for(int i=0;i<dim[0];++i) for(int j=0;j<dim[1];++j)
	{
		Vec2 ij((float)i,(float)j);
		Vec2 uv = ij / dim;
		
		Vec4 color;
		apply(uv, color);
		sampler.set(uv[0],uv[1],color);
	}
}

//--------------------------------------------------------------
void HashOperation::apply(const Vec2& uv, Vec4& outColor)
{
	Vec3 v(12.19823371,10.27653,35.746541);
	float n = 45758.2653872 + _seed;
	float c = Vec3(uv[0],uv[1],0.0).dot( v );
	float val = std::abs( frac( std::sin( c )* n ) );
	outColor.set(val,val,val,1.0);
}

//--------------------------------------------------------------
void NoiseOperation::apply(const Vec2& uv, Vec4& outColor)
{
	ImageSampler sampler(_hash, ImageSampler::Mode_Repeat);
	Vec3 iv( floor(uv[0]*_freq), floor(uv[1]*_freq),0.0);
	Vec3 fv( frac(uv[0]*_freq), frac(uv[1]*_freq),0.0);

	fv = fv*fv*3.0 - fv*fv*fv*2.0; // smooth mix

	Vec3 invfrq(1.0/_freq);
	float v[4];
	v[0] = sampler( (iv+Vec3(0.0,0.0,0.0))*invfrq )[0];
	v[1] = sampler( (iv+Vec3(0.0,1.0,0.0))*invfrq )[0];
	v[2] = sampler( (iv+Vec3(1.0,0.0,0.0))*invfrq )[0];
	v[3] = sampler( (iv+Vec3(1.0,1.0,0.0))*invfrq )[0];
	
	float v01 = mix(v[0],v[1],fv[1]);
	float v23 = mix(v[2],v[3],fv[1]);

	float val = mix(v01,v23,fv[0]);
	outColor.set(val,val,val,1.0);
}

//--------------------------------------------------------------
// thanks to Inigo Quilez	
void VoronoiOperation::apply(const Vec2& uv, Vec4& outColor)
{
	ImageSampler sampler(_hash, ImageSampler::Mode_Repeat);
	Vec3 iv( floor(uv[0]*_freq), floor(uv[1]*_freq),0.0);
	Vec3 fv( frac(uv[0]*_freq), frac(uv[1]*_freq),0.0);
	
	Vec3 mb, mr;

	float res=8.0;
	for(int a=-1;a<=1;++a) for(int b=-1;b<=1;++b)
	{
		Vec3 bb( a,b,0.0 );
		Vec3 r = bb + Vec3( sampler((iv+bb)/_freq)[0] ) - fv;
		r[2] = 0.0;
		float d = r.length2();
		
		if( d < res)
		{
			res = d;
			mr = r;
			mb = bb;
		}
	}
	
	res = 8.0;
	for(int a=-2;a<=2;++a) for(int b=-2;b<=2;++b)
	{
		Vec3 bb = mb + Vec3(a,b,0.0);
		Vec3 r = bb + Vec3( sampler((iv+bb)/_freq)[0] ) - fv;
		r[2] = 0.0;
		Vec3 nor = r - mr; nor.normalize();
		Vec3 mid = (mr + r) * 0.5;
		float d = mid.dot(nor);
		
		res = std::min( res, d );
	}
	
	// opt
	// res = 1.0 - smoothstep(0.0, 0.05, res);
	outColor.set(res,res,res,1.0);
}

//--------------------------------------------------------------
// Fractional Brownian Motion
void FbmOperation::apply(const Vec2& uv, Vec4& outColor)
{
	_perlinOp.setHashmap(_hash);
	_voronoiOp.setHashmap(_hash);
	Vec3 uvw( uv[0],uv[1],0.0 );
	float persist = _persist;
	float freq = _freq;
	float res = 0.0;
	Vec4 subcolor;
	
	for(int o=0;o<_octaves;++o)
	{
		_perlinOp.setFreq(freq);
		_voronoiOp.setFreq(freq);
		if(_type == 0) _perlinOp.apply(uvw,subcolor);
		if(_type == 1) _voronoiOp.apply(uvw,subcolor);
		res += subcolor[0] * persist;
		persist *= 0.5;
		freq *= 2.0;
	};
	outColor.set(res,res,res,1.0);
}

//--------------------------------------------------------------
void SignalOperation::apply(const Vec2& uv, Vec4& outColor)
{
	Vec3 uvw(uv[0],uv[1],0.0);
	float ad = 0.0;
	
	if(_noise != nullptr)
	{
		ImageSampler sampl(_noise);
		Vec4 offset = sampl(uvw);
		// uvw += (Vec3( offset[0],offset[1],0.0 ) * 2.0 - 1.0) * 0.1;
		ad = ((offset[0]) * 2.0 - 1.0) * _noiseIntensity;
	}
	
	float d = 0.0;
	if(_type == 0) d = uvw.dot(_vec);
	if(_type == 1) d = (uvw - _vec).length();
	d += ad;
	float t = d * _freq + _phase;
	float v = _base + std::sin(t * 3.141592) * _ampl;
	
	
	outColor.set(v,v,v,1.0);
}

//--------------------------------------------------------------
void BumpToNormalOperation::apply(const Vec2& uv, Vec4& outColor)
{
	ImageSampler sampler(_target, ImageSampler::Mode_Repeat);
	
	float xoff = 1.0/_target->width();
	float yoff = 1.0/_target->height();
	
	float dxa = sampler.get( uv[0]-xoff, uv[1] )[0];
	float dxb = sampler.get( uv[0]+xoff, uv[1] )[0];
	float dya = sampler.get( uv[0], uv[1]-yoff )[0];
	float dyb = sampler.get( uv[0], uv[1]+yoff )[0];
	
	Vec3 normal(dxa-dxb,dya-dyb,1.0); normal.normalize();
	normal *= 0.5; normal += 0.5;
	
	outColor.set(normal[0],normal[1],normal[2],1.0);
}

//--------------------------------------------------------------
void PerturbateOperation::apply(const Vec2& uv, Vec4& outColor)
{
	ImageSampler sampler(_target, ImageSampler::Mode_Mirror);
	ImageSampler uvSampler(_uvmap);
	
	Vec4 p = uvSampler.get(uv[0],uv[1]); p *= 2.0; p -= 1.0;
	Vec2 coords = uv + Vec2(p[0],p[1]) * _intensity;
	
	outColor = sampler.get( coords[0], coords[1] );
}

//--------------------------------------------------------------
void ColorMixOperation::apply(const Vec2& uv, Vec4& outColor)
{
	ImageSampler sampler(_target);
	sampler.setInterpo(ImageSampler::Interpo_Linear);
	float f = sampler(uv)[0];
	outColor = mix(_color1,_color2,f);
}

//--------------------------------------------------------------
void MorphoOperation::apply(const Vec2& uv, Vec4& outColor)
{
	int C=3, R=3;
	int xCenter = C / 2;
	int yCenter = R / 2;
	ImageSampler sampler(_target, ImageSampler::Mode_Mirror);
	
	Vec4 e = _type<0 ? Vec4(1.0) : Vec4(0.0);
	for(int c=0;c<C;++c) for(int r=0;r<R;++r)
	{
		float xoff = (float)(c-xCenter)/_target->width();
		float yoff = (float)(r-yCenter)/_target->height();
		Vec4 v = sampler.get(uv[0]+xoff,uv[1]+yoff) * _mat(c,r);
	
		for(int i=0;i<3;++i) e[i] = _type<0 ? std::min(v[i],e[i]) : std::max(v[i],e[i]);
	}
	
	e[3] = 1.0;
	outColor = e;
}

//--------------------------------------------------------------
void Conv2dOperation::apply(const Vec2& uv, Vec4& outColor)
{
	int C=3, R=3;
	int xCenter = C / 2;
	int yCenter = R / 2;
	ImageSampler sampler(_target, ImageSampler::Mode_Mirror);
	
	Vec4 accu=0.0;
	for(int c=0;c<C;++c) for(int r=0;r<R;++r)
	{
		float xoff = (float)(c-xCenter)/_target->width();
		float yoff = (float)(r-yCenter)/_target->height();
		Vec4 spl = sampler.get(uv[0]+xoff,uv[1]+yoff);
		float coef = _mat(c,r);
		accu += (spl * coef);
	}
	
	for(int i=0;i<4;++i)accu[i] = std::abs(accu[i]);
	outColor = accu;
}

IMPGEARS_END
