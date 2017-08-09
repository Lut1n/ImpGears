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
	float ln = v.x*v.x + v.y*v.y + v.z*v.z;
	ln = sqrtf(ln);

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

void blend(ImageData& dst, ImageData& src, ImageData& alphaMap, double threshold)
{
   	for(Uint32 i=0; i<dst.getWidth(); ++i)
	{
		for(Uint32 j=0; j<dst.getHeight(); ++j)
		{
			Pixel srcColor, dstColor;
			srcColor = src.getPixel(i,j);
			dstColor = dst.getPixel(i,j);
            
            double alpha = (double)(alphaMap.getPixel(i,j).r) / 255.0;
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

	for(Uint32 i=0; i<in.getWidth(); ++i)
	{
		for(Uint32 j=0; j<in.getHeight(); ++j)
		{

			float h1 = (float)(in.getPixel(i-dist,j).red)/255.0;
			float h2 = (float)(in.getPixel(i+dist,j).red)/255.0;
			float h3 = (float)(in.getPixel(i,j-dist).red)/255.0;
			float h4 = (float)(in.getPixel(i,j+dist).red)/255.0;

			Vec3 nx, ny, normal;

			nx.x = 2.0;
			nx.y = 0.0;
			nx.z = (h2-h1) * force;

			ny.x = 0.0;
			ny.y = 2.0;
			ny.z = (h4-h3) * force;

			cross(nx, ny, normal);
			normalize(normal);

			normal.x += 1.0; normal.x /= 2.0;
			normal.y += 1.0; normal.y /= 2.0;
			normal.z += 1.0; normal.z /= 2.0;

			Pixel pixel;
			pixel.blue = (unsigned char)(normal.x * 255); // red
			pixel.green = (unsigned char)(normal.y * 255); //green
			pixel.red = (unsigned char)(normal.z * 255); // blue
            pixel.alpha = 255;
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
			t += perturbIntensity * perturbation->getPixel(i,j).r;//imp::perlinMain(x/64,y/64,perturb/64);

		float v = imp::Sin(t/diag * 3.141592 * freq, maxPeriodRatio);
		unsigned int comp = (v+1.0)/2.0 * ampl;
		imp::Pixel px = {comp,comp,comp,255};
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
			t += perturbIntensity * perturbation->getPixel(i,j).r;//imp::perlinMain(x/64,y/64,perturb/64);

		float v = 0;
		if(signalType == SignalType_Sinus)
			v = imp::Sin(t/diag * 3.141592 * freq, maxPeriodRatio);
		else if(signalType == SignalType_Square)
			v = imp::SquareSignal(t/diag * 3.141592 * freq, maxPeriodRatio);
		else if(signalType == SignalType_Triangle)
			v = imp::TriangleSignal(t/diag * 3.141592 * freq, maxPeriodRatio);

		unsigned int comp = (v+1.0)/2.0 * ampl;
		imp::Pixel px = {comp,comp,comp,255};
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
			t += perturbIntensity * perturbation->getPixel(i,j).r;//imp::perlinMain(x/64,y/64,perturb/64);

		float v = imp::Sin(t/diag * 3.141592 * freq, maxPeriodRatio);
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

		px.r = (unsigned char)((px.r/255.0) * ((double)color2.r-(double)color1.r) + (double)color1.r);
		px.g = (unsigned char)((px.g/255.0) * ((double)color2.g-(double)color1.g) + (double)color1.g);
		px.b = (unsigned char)((px.b/255.0) * ((double)color2.b-(double)color1.b) + (double)color1.b);

		img.setPixel(i,j,px);
	}}
}

void applyColorization(imp::ImageData& img, const imp::Pixel& color1, const imp::Pixel& color2, Distribution& distrib)
{
	for(unsigned int i=0; i<img.getWidth(); ++i)
	{for(unsigned int j=0; j<img.getHeight(); ++j)
	{
		imp::Pixel px = img.getPixel(i,j);

		double t1 = (px.r/255.0);
		double t2 = (px.g/255.0);
		double t3 = (px.b/255.0);

		t1 = distrib(t1);
		t2 = distrib(t2);
		t3 = distrib(t3);

		px.r = (unsigned char)(t1 * ((double)color2.r-(double)color1.r) + (double)color1.r);
		px.g = (unsigned char)(t2 * ((double)color2.g-(double)color1.g) + (double)color1.g);
		px.b = (unsigned char)(t3 * ((double)color2.b-(double)color1.b) + (double)color1.b);

		img.setPixel(i,j,px);
	}}
}

void applyMaximization(imp::ImageData& img)
{
	Uint8 maxR = 0;
	Uint8 minR = 255;

	// scan
	for(unsigned int i=0; i<img.getWidth(); ++i)
	{for(unsigned int j=0; j<img.getHeight(); ++j)
	{
		Uint8 r = img.getPixel(i,j).r;
		if(r < minR)
			minR = r;
		else if(r > maxR)
			maxR = r;
	}}

	// maximize
	for(unsigned int i=0; i<img.getWidth(); ++i)
	{for(unsigned int j=0; j<img.getHeight(); ++j)
	{
		double t = double(img.getPixel(i,j).r);
		Uint8 newt = (Uint8)Lerp( 0.0, 255.0, (t-minR)/(maxR-minR) );
		Pixel px = {newt,newt,newt, 255};
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
        
        xcell[j*cellcount+i] = x + double(noisemap->getPixel(x,y).r)/255.0 * fieldW;
        ycell[j*cellcount+i] = y + double(noisemap->getPixel(x,y).r)/255.0 * fieldH;
        
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
            Uint8 newt = (Uint8)Lerp( 0.0, 255.0, d/maxR);
            Pixel px = {newt,newt,newt, 255};
            img.setPixel(i,j,px);
        }
    }
    
}

IMPGEARS_END
