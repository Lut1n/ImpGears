#include <Data/ImageUtils.h>
#include <Math/Perlin.h>

IMPGEARS_BEGIN

float min(float a, float b)
{
	return a<b?a:b;
}

float max(float a, float b)
{
	return a>b?a:b;
}

float clamp(float x, float edge0, float edge1)
{
	return min( max(edge0,x), edge1 );
}

float floor(float v)
{
	int rv = (int)v;
	return (float)rv;
}

float frac(float v)
{
	return v - floor(v);
}

void normalize(Vec3& v)
{
	float ln = v.x*v.x + v.y*v.y + v.z*v.z;
	ln = sqrt(ln);

	v.x /= ln;
	v.y /= ln;
	v.z /= ln;
}

void cross(Vec3 v1, Vec3 v2, Vec3& result)
{
	result.x = v1.y*v2.z - v1.z*v2.y;
	result.y = v1.z*v2.x - v1.x*v2.z;
	result.z = v1.x*v2.y - v1.y*v2.x;
}

float smoothstep(char edge0, char edge1, float delta)
{
    float x = clamp(delta, 0.0, 1.0); 
    return x*x*(3 - 2*x);
}

Pixel lerp(Pixel a, Pixel b, float delta)
{
	delta = smoothstep(0.0, 1.0, delta);
	Pixel result;
	result.red = delta*b.red + (1.0-delta)*a.red;
	result.green = delta*b.green + (1.0-delta)*a.green;
	result.blue = delta*b.blue + (1.0-delta)*a.blue;
	result.alpha = delta*b.alpha + (1.0-delta)*a.alpha;

	return result;
}

Pixel bilerp(Pixel oo, Pixel xo, Pixel oy, Pixel xy, float deltaX, float deltaY)
{
	Pixel lx1 = lerp(oo, xo, deltaX);
	Pixel lx2 = lerp(oy, xy, deltaX);
	return lerp(lx1, lx2, deltaY);
}

void applyBilinearInterpo(ImageData& bitmap, float frqX, float frqY)
{
	ImageData temp;
	temp.clone(bitmap);

	for(Uint32 i=0; i<bitmap.getWidth(); ++i)
	{
		for(Uint32 j=0; j<bitmap.getHeight(); ++j)
		{
			float periodX = bitmap.getWidth() / frqX;
			float periodY = bitmap.getHeight() / frqY;

			int x0 = floor(i/periodX) * periodX;
			int y0 = floor(j/periodY) * periodY;
			int x1 = x0 + periodX;
			int y1 = y0 + periodY;

			float dx = frac(i/periodX);
			float dy = frac(j/periodY);

			Pixel oo = temp.getPixel(x0, y0);
			Pixel xo = temp.getPixel(x1, y0);
			Pixel oy = temp.getPixel(x0, y1);
			Pixel xy = temp.getPixel(x1, y1);

			Pixel color = bilerp(oo, xo, oy, xy, dx, dy);
			//Pixel color = bismooth(oo, xo, oy, xy, dx, dy);

			bitmap.setPixel(i,j, color);
		}
	}
}

void blend(ImageData& dst, ImageData& src, float alpha)
{
	for(Uint32 i=0; i<dst.getWidth(); ++i)
	{
		for(Uint32 j=0; j<dst.getHeight(); ++j)
		{
			Pixel srcColor, dstColor;
			srcColor = src.getPixel(i,j);
			dstColor = dst.getPixel(i,j);

			dstColor = lerp(dstColor, srcColor, alpha);
			dst.setPixel(i,j,dstColor);
		}
	}
}

void heightToNormal(ImageData& in, ImageData& out, float force, float prec)
{
	int dist = prec * 1.0;

	out.clone(in); 

	for(Uint32 i=0; i<in.getWidth(); ++i)
	{
		for(Uint32 j=0; j<in.getHeight(); ++j)
		{
			Pixel x0, x1, y0, y1;
			x0 = in.getPixel(i-dist,j);
			x1 = in.getPixel(i+dist,j);
			y0 = in.getPixel(i,j-dist);
			y1 = in.getPixel(i,j+dist);

			float vx0 = (float)(x0.red)/255.0;
			float vx1 = (float)(x1.red)/255.0;
			float vy0 = (float)(y0.red)/255.0;
			float vy1 = (float)(y1.red)/255.0;

			Vec3 nx, ny, normal;

			nx.x = 2.0 * dist;
			nx.y = 0.0 * dist;
			nx.z = (vx1-vx0) * force;
			normalize(nx);

			ny.x = 0.0 * dist;
			ny.y = 2.0 * dist;
			ny.z = (vy1-vy0) * force;
			normalize(ny);

			cross(nx, ny, normal);
			normalize(normal);

			normal.x += 1.0; normal.x /= 2.0;
			normal.y += 1.0; normal.y /= 2.0;
			normal.z += 1.0; normal.z /= 2.0;

			Pixel pixel;
			pixel.red = (unsigned char)(normal.x * 255);
			pixel.green = (unsigned char)(normal.y * 255);
			pixel.blue = (unsigned char)(normal.z * 255);
			out.setPixel(i,j,pixel);
		}
	}
}

inline imp::Pixel mirGet(imp::ImageData& img, unsigned int x, unsigned int y)
{
	if(x < 0) x = -x;
	else if(x>=img.getWidth()) x = img.getWidth() - (x-img.getWidth());
	if(y < 0) y= -y;
	else if(y>=img.getHeight()) y = img.getHeight() - (y-img.getHeight());

	return img.getPixel(x,y);
}

void drawDirectionnalSinus(imp::ImageData& img, double dirX, double dirY, float freq, float ampl,
	const imp::ImageData* perturbation, float perturbIntensity)
{
	float diag = sqrtf(img.getHeight()/2.0*img.getHeight()/2.0 + img.getWidth()/2.0*img.getWidth()/2.0);

	float dirLength = sqrtf(dirX*dirX + dirY*dirY);
	dirX /= dirLength;
	dirY /= dirLength;

	for(unsigned int i=0; i<img.getWidth(); ++i)
	{for(unsigned int j=0; j<img.getHeight(); ++j)
	{
		float dirDot = (i*dirX + j*dirY);
		float t = dirDot;

		if(perturbation != NULL)
			t += perturbIntensity * perturbation->getPixel(i,j).r;//imp::perlinMain(x/64,y/64,perturb/64);

		float v = sin(t/diag * 3.141592 * freq);
		unsigned int comp = (v+1.0)/2.0 * ampl;
		imp::Pixel px = {comp,comp,comp,255};
		img.setPixel(i,j,px);
	}}
}

void drawRadialSinus(imp::ImageData& img, double posX, double posY, float freq, float ampl,
	const imp::ImageData* perturbation, float perturbIntensity)
{
	float diag = sqrtf(img.getHeight()/2.0*img.getHeight()/2.0 + img.getWidth()/2.0*img.getWidth()/2.0);

	for(unsigned int i=0; i<img.getWidth(); ++i)
	{for(unsigned int j=0; j<img.getHeight(); ++j)
	{
		float distX = abs(i - posX);
		float distY = abs(j - posY);
		float t = sqrtf(distX*distX + distY*distY);

		if(perturbation != NULL)
			t += perturbIntensity * perturbation->getPixel(i,j).r;//imp::perlinMain(x/64,y/64,perturb/64);

		float v = sin(t/diag * 3.141592 * freq);
		unsigned int comp = (v+1.0)/2.0 * ampl;
		imp::Pixel px = {comp,comp,comp,255};
		img.setPixel(i,j,px);
	}}
}

void applyPerturbation(imp::ImageData& img, const imp::ImageData& normals, float intensity)
{
	imp::ImageData cloned;
	cloned.clone(img);

	for(unsigned int i=0; i<img.getWidth(); ++i)
	{for(unsigned int j=0; j<img.getHeight(); ++j)
	{
		float depX = (normals.getPixel(i,j).r)/255.0;
		float depY = (normals.getPixel(i,j).g)/255.0;

		depX = depX*2.0 - 1.0;
		depY = depY*2.0 - 1.0;


		imp::Pixel px = mirGet(cloned, i+depX*intensity, j+depY*intensity);

		img.setPixel(i,j,px);
	}}
}

void applyColorization(imp::ImageData& img, const imp::Pixel& color1, const imp::Pixel& color2)
{
	for(unsigned int i=0; i<img.getWidth(); ++i)
	{for(unsigned int j=0; j<img.getHeight(); ++j)
	{
		imp::Pixel px = img.getPixel(i,j);

		px.r = (unsigned char)((px.r/255.0) * (color2.r-color1.r) + color1.r);
		px.g = (unsigned char)((px.g/255.0) * (color2.g-color1.g) + color1.g);
		px.b = (unsigned char)((px.b/255.0) * (color2.b-color1.b) + color1.b);

		img.setPixel(i,j,px);
	}}
}

IMPGEARS_END
