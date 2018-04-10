#include <Texture/ImageUtils.h>
#include <Core/Perlin.h>

#include <iostream>

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
	float ln = v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
	ln = sqrtf(ln);

	v[0] /= ln;
	v[1] /= ln;
	v[2] /= ln;
}

void cross(Vec3 v1, Vec3 v2, Vec3& result)
{
	result[0] = v1[1]*v2[2] - v1[2]*v2[1];
	result[1] = v1[2]*v2[0] - v1[0]*v2[2];
	result[2] = v1[0]*v2[1] - v1[1]*v2[0];
}

float smoothstep(char edge0, char edge1, float delta)
{
    float x = clamp(delta, 0.0, 1.0); 
    return x*x*(3 - 2*x);
}

Vec4 lerp(Vec4 a, Vec4 b, float delta)
{
	delta = smoothstep(0.0, 1.0, delta);
	Vec4 result;
	result[0] = delta*b[0] + (1.0-delta)*a[0];
	result[1] = delta*b[1] + (1.0-delta)*a[1];
	result[2] = delta*b[2] + (1.0-delta)*a[2];
	result[3] = delta*b[3] + (1.0-delta)*a[3];

	return result;
}

Vec4 bilerp(Vec4 oo, Vec4 xo, Vec4 oy, Vec4 xy, float deltaX, float deltaY)
{
	Vec4 lx1 = lerp(oo, xo, deltaX);
	Vec4 lx2 = lerp(oy, xy, deltaX);
	return lerp(lx1, lx2, deltaY);
}

void applyBilinearInterpo(ImageData& bitmap, float frqX, float frqY)
{
	ImageData temp;
	temp.clone(bitmap);

	for(std::uint32_t i=0; i<bitmap.getWidth(); ++i)
	{
		for(std::uint32_t j=0; j<bitmap.getHeight(); ++j)
		{
			float periodX = bitmap.getWidth() / frqX;
			float periodY = bitmap.getHeight() / frqY;

			int x0 = floor(i/periodX) * periodX;
			int y0 = floor(j/periodY) * periodY;
			int x1 = x0 + periodX;
			int y1 = y0 + periodY;

			float dx = frac(i/periodX);
			float dy = frac(j/periodY);

			Vec4 oo = temp.getPixel(x0, y0);
			Vec4 xo = temp.getPixel(x1, y0);
			Vec4 oy = temp.getPixel(x0, y1);
			Vec4 xy = temp.getPixel(x1, y1);

			Vec4 color = bilerp(oo, xo, oy, xy, dx, dy);
			//Vec4 color = bismooth(oo, xo, oy, xy, dx, dy);

			bitmap.setPixel(i,j, color);
		}
	}
}

void blend(ImageData& dst, ImageData& src, float alpha)
{
	for(std::uint32_t i=0; i<dst.getWidth(); ++i)
	{
		for(std::uint32_t j=0; j<dst.getHeight(); ++j)
		{
			Vec4 srcColor, dstColor;
			srcColor = src.getPixel(i,j);
			dstColor = dst.getPixel(i,j);

			dstColor = lerp(dstColor, srcColor, alpha);
			dst.setPixel(i,j,dstColor);
		}
	}
}

void blend(ImageData& dst, ImageData& src, ImageData& alphaMap, double threshold)
{
   	for(std::uint32_t i=0; i<dst.getWidth(); ++i)
	{
		for(std::uint32_t j=0; j<dst.getHeight(); ++j)
		{
			Vec4 srcColor, dstColor;
			srcColor = src.getPixel(i,j);
			dstColor = dst.getPixel(i,j);
            
            double alpha = (double)(alphaMap.getPixel(i,j)[0]) / 255.0;
            /*if(alpha > threshold)
                dstColor = srcColor;*/
            
			dstColor = lerp(dstColor, srcColor, alpha);
			dst.setPixel(i,j,dstColor);
		}
	}
}

void heightToNormal(ImageData& in, ImageData& out, float force, float prec)
{
	int dist = 1.0;// prec * 1.0;
    force = 1.0;

	out.clone(in); 

	for(std::uint32_t i=0; i<in.getWidth(); ++i)
	{
		for(std::uint32_t j=0; j<in.getHeight(); ++j)
		{

			float h1 = (float)(in.getPixel(i-dist,j)[0])/255.0;
			float h2 = (float)(in.getPixel(i+dist,j)[0])/255.0;
			float h3 = (float)(in.getPixel(i,j-dist)[0])/255.0;
			float h4 = (float)(in.getPixel(i,j+dist)[0])/255.0;

			Vec3 nx, ny, normal;

			nx[0] = 2.0;
			nx[1] = 0.0;
			nx[2] = (h2-h1) * force;

			ny[0] = 0.0;
			ny[1] = 2.0;
			ny[2] = (h4-h3) * force;

			cross(nx, ny, normal);
			normalize(normal);

			normal[0] += 1.0; normal[0] /= 2.0;
			normal[1] += 1.0; normal[1] /= 2.0;
			normal[2] += 1.0; normal[2] /= 2.0;

			Vec4 pixel;
			pixel[2] = (unsigned char)(normal[0] * 255); // red
			pixel[1] = (unsigned char)(normal[1] * 255); //green
			pixel[0] = (unsigned char)(normal[2] * 255); // blue
            pixel[3] = 255;
			out.setPixel(i,j,pixel);
		}
	}
}

inline Vec4 mirGet(imp::ImageData& img, unsigned int x, unsigned int y)
{
	if(x < 0) x = -x;
	else if(x>=img.getWidth()) x = img.getWidth() - (x-img.getWidth());
	if(y < 0) y= -y;
	else if(y>=img.getHeight()) y = img.getHeight() - (y-img.getHeight());

	return img.getPixel(x,y);
}

void drawDirectionnalSinus(imp::ImageData& img, double dirX, double dirY, float freq, float ampl, float maxPeriodRatio,
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
			t += perturbIntensity * perturbation->getPixel(i,j)[0];//imp::perlinMain(x/64,y/64,perturb/64);

		float v = imp::Sin(t/diag * 3.141592 * freq, maxPeriodRatio);
		unsigned int comp = (v+1.0)/2.0 * ampl;
		Vec4 px(comp,comp,comp,255);
		img.setPixel(i,j,px);
	}}
}

void drawDirectionnalSignal(imp::ImageData& img, SignalType signalType, double dirX, double dirY, float freq, float ampl, float maxPeriodRatio,
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
			t += perturbIntensity * perturbation->getPixel(i,j)[0];//imp::perlinMain(x/64,y/64,perturb/64);

		float v = 0;
		if(signalType == SignalType_Sinus)
			v = imp::Sin(t/diag * 3.141592 * freq, maxPeriodRatio);
		else if(signalType == SignalType_Square)
			v = imp::SquareSignal(t/diag * 3.141592 * freq, maxPeriodRatio);
		else if(signalType == SignalType_Triangle)
			v = imp::TriangleSignal(t/diag * 3.141592 * freq, maxPeriodRatio);

		unsigned int comp = (v+1.0)/2.0 * ampl;
		Vec4 px(comp,comp,comp,255);
		img.setPixel(i,j,px);
	}}
}

void drawRadialSinus(imp::ImageData& img, double posX, double posY, float freq, float ampl, float maxPeriodRatio,
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
			t += perturbIntensity * perturbation->getPixel(i,j)[0];//imp::perlinMain(x/64,y/64,perturb/64);

		float v = imp::Sin(t/diag * 3.141592 * freq, maxPeriodRatio);
		unsigned int comp = (v+1.0)/2.0 * ampl;
		Vec4 px(comp,comp,comp,255);
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
		float depX = (normals.getPixel(i,j)[0])/255.0;
		float depY = (normals.getPixel(i,j)[1])/255.0;

		depX = depX*2.0 - 1.0;
		depY = depY*2.0 - 1.0;


		Vec4 px = mirGet(cloned, i+depX*intensity, j+depY*intensity);

		img.setPixel(i,j,px);
	}}
}

void applyColorization(imp::ImageData& img, const imp::Vec4& color1, const imp::Vec4& color2)
{
	for(unsigned int i=0; i<img.getWidth(); ++i)
	{for(unsigned int j=0; j<img.getHeight(); ++j)
	{
		imp::Vec4 px = img.getPixel(i,j);

		px[0] = (unsigned char)((px[0]/255.0) * ((double)color2[0]-(double)color1[0]) + (double)color1[0]);
		px[1] = (unsigned char)((px[1]/255.0) * ((double)color2[1]-(double)color1[1]) + (double)color1[1]);
		px[2] = (unsigned char)((px[2]/255.0) * ((double)color2[2]-(double)color1[2]) + (double)color1[2]);

		img.setPixel(i,j,px);
	}}
}

void applyColorization(imp::ImageData& img, const imp::Vec4& color1, const imp::Vec4& color2, Distribution& distrib)
{
	for(unsigned int i=0; i<img.getWidth(); ++i)
	{for(unsigned int j=0; j<img.getHeight(); ++j)
	{
		imp::Vec4 px = img.getPixel(i,j);

		double t1 = (px[0]/255.0);
		double t2 = (px[1]/255.0);
		double t3 = (px[2]/255.0);

		t1 = distrib(t1);
		t2 = distrib(t2);
		t3 = distrib(t3);

		px[0] = (unsigned char)(t1 * ((double)color2[0]-(double)color1[0]) + (double)color1[0]);
		px[1] = (unsigned char)(t2 * ((double)color2[1]-(double)color1[1]) + (double)color1[1]);
		px[2] = (unsigned char)(t3 * ((double)color2[2]-(double)color1[2]) + (double)color1[2]);

		img.setPixel(i,j,px);
	}}
}

void applyMaximization(imp::ImageData& img)
{
	std::uint8_t maxR = 0;
	std::uint8_t minR = 255;

	// scan
	for(unsigned int i=0; i<img.getWidth(); ++i)
	{for(unsigned int j=0; j<img.getHeight(); ++j)
	{
		std::uint8_t r = img.getPixel(i,j)[0];
		if(r < minR)
			minR = r;
		else if(r > maxR)
			maxR = r;
	}}

	// maximize
	for(unsigned int i=0; i<img.getWidth(); ++i)
	{for(unsigned int j=0; j<img.getHeight(); ++j)
	{
		double t = double(img.getPixel(i,j)[0]);
		std::uint8_t newt = (std::uint8_t)Lerp( 0.0, 255.0, (t-minR)/(maxR-minR) );
		Vec4 px(newt,newt,newt, 255);
		img.setPixel(i,j,px);
	}}
}

void drawCellularNoise(imp::ImageData& img, unsigned int cellcount, const imp::ImageData* noisemap)
{
    double xcell[cellcount*cellcount];
    double ycell[cellcount*cellcount];

    double fieldW = (noisemap->getWidth()/cellcount);
    double fieldH = (noisemap->getHeight()/cellcount);
    
    // initialize cell centers
    for(unsigned int i=0; i<cellcount; ++i)
    for(unsigned int j=0; j<cellcount; ++j)
    {
        
        double x = (double)i * fieldW;
        double y = (double)j * fieldH;
        
        xcell[j*cellcount+i] = x + double(noisemap->getPixel(x,y)[0])/255.0 * fieldW;
        ycell[j*cellcount+i] = y + double(noisemap->getPixel(x,y)[0])/255.0 * fieldH;
        
        std::cout << "x=" << xcell[j*cellcount+i] << "; y=" << ycell[j*cellcount+i] << std::endl;
    }
    
    // draw
    for(unsigned int i=0; i<img.getWidth(); ++i)
    {
        for(unsigned int j=0; j<img.getHeight(); ++j)
        {
            // find closest cell center
            double d = -1.0;
            for(unsigned int cell=0; cell<cellcount*cellcount; ++cell)
            {
                
                double dx = xcell[cell] - (double)i;
                double dy = ycell[cell] - (double)j;
                double d2 = dx*dx + dy*dy;
                if(d<0.0 || d2 < d)
                {
                    d = d2;
                }
                
                // ---- tiling effect -----
                dx = (xcell[cell]-img.getWidth()) - (double)i;
                dy = (ycell[cell]-img.getHeight()) - (double)j; 
                d2 = dx*dx + dy*dy;
                if(d<0.0 || d2 < d)
                {
                    d = d2;
                }
                dx = (xcell[cell]+img.getWidth()) - (double)i;
                dy = (ycell[cell]-img.getHeight()) - (double)j; 
                d2 = dx*dx + dy*dy;
                if(d<0.0 || d2 < d)
                {
                    d = d2;
                }
                dx = (xcell[cell]-img.getWidth()) - (double)i;
                dy = (ycell[cell]+img.getHeight()) - (double)j; 
                d2 = dx*dx + dy*dy;
                if(d<0.0 || d2 < d)
                {
                    d = d2;
                }
                dx = (xcell[cell]+img.getWidth()) - (double)i;
                dy = (ycell[cell]+img.getHeight()) - (double)j; 
                d2 = dx*dx + dy*dy;
                if(d<0.0 || d2 < d)
                {
                    d = d2;
                }
                // -------------------
            }
            
            // apply new value
            const double maxR = (fieldW)*(fieldW);
            std::uint8_t newt = (std::uint8_t)Lerp( 0.0, 255.0, d/maxR);
            Vec4 px(newt,newt,newt, 255);
            img.setPixel(i,j,px);
        }
    }
    
}

IMPGEARS_END
