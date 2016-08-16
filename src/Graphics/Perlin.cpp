#include "Graphics/Perlin.h"

using namespace imp;

Perlin::Perlin()
{

}

Perlin::Perlin(const Config& config)
{
	setConfig(config);
}

void Perlin::setConfig(const Config& config)
{
	_config = config;

	    //ctor
	seedmap.create(config.resolutionX, config.resolutionY, 24, PixelFormat_RGB8);

	octavemap.resize(config.octaveCount);

    for(unsigned int i=0; i<octavemap.size(); ++i)
    {
        octavemap[i].create(config.resolutionX, config.resolutionY, 24, PixelFormat_RGB8);
    }

    resultmap.create(config.resolutionX, config.resolutionY, 24, PixelFormat_RGB8);

    this->vmin = config.valueMin;
    this->vmax = config.valueMax;

    srand(time(NULL));
}

Perlin::~Perlin()
{
    //dtor
	seedmap.destroy();

    for(unsigned int i=0; i<octavemap.size(); ++i)
        octavemap[i].destroy();

    resultmap.destroy();
}

void Perlin::generateSeedMap()
{
    for(unsigned int x=0; x<seedmap.getWidth(); ++x)
    {
        for(unsigned int y=0; y<seedmap.getHeight(); ++y)
        {
			double val = rand()%(vmax - vmin) + vmin;
			Pixel pix;
			pix.red = val * 255;
			pix.green = val * 255;
			pix.blue = val * 255;
            seedmap.setPixel(x,y,pix);
        }
    }
}

void Perlin::smoothSeedMap()
{

    imp::ImageData smoothdata;
	smoothdata.create(seedmap.getWidth(), seedmap.getHeight(), 24, imp::PixelFormat_BGR8);

    for(unsigned int x=0;x<seedmap.getWidth(); ++x)
    {
        for(unsigned int y=0; y<seedmap.getHeight(); ++y)
        {
			Pixel px;
			px.red = smoothPoint(x,y) * 255;
			px.green = px.red;
			px.blue = px.red;
			smoothdata.setPixel(x,y,px);
        }
    }

    seedmap.destroy();
    seedmap.clone(smoothdata);
}

float Perlin::smoothPoint(unsigned int x, unsigned int y)
{
    float total = 0.f;

unsigned int i2,j2;

    for(unsigned int i=x; i<x+3; ++i)
    {
        i2 = i;
        if(i2 >= seedmap.getWidth())
            i2 -= seedmap.getWidth();

        for(unsigned int j=y; j<y+3; ++j)
        {
            j2 = j;
            if(j2 >= seedmap.getHeight())
                j2 -= seedmap.getHeight();

			Pixel px = seedmap.getPixel(i2,j2);
            total += (float)(px.red)/255.0;
        }
    }

    return total/9;
}

void Perlin::apply()
{
	generateSeedMap();
	smoothSeedMap();
	for(unsigned int octave=0; octave<octavemap.size(); ++octave)
		generateOctaveMap(octave);

	compileResult();
}

void Perlin::generateOctaveMap(int i)
{
    float freq = pow(2.f, (float)i+2);
    float persistence = 0.75f;
    float amplitude = pow(persistence, (float)i);

    for(unsigned int x=0; x<octavemap[i].getWidth(); ++x)
    {
        for(unsigned int y=0; y<octavemap[i].getHeight(); ++y)
        {
			Pixel pix;
			pix.red = interpolatedSeed(x,y,freq) * amplitude * 255;
			pix.green = pix.red;
			pix.blue = pix.red;
            octavemap[i].setPixel(x,y,pix);
        }
    }
}

void Perlin::compileResult()
{
    float maxvalue = 0;
    float minvalue = 255;

    for(unsigned int x=0; x<resultmap.getWidth(); ++x)
    {
        for(unsigned int y=0; y<resultmap.getHeight(); ++y)
        {
            float total = 0;
            for(unsigned int i=0; i<octavemap.size(); ++i)
            {
                total += octavemap[i].getPixel(x,y).red;
            }

            if(total > maxvalue)maxvalue = total;
            if(total < minvalue)minvalue = total;

			Pixel pixel;
			pixel.red = total;
			pixel.green = total;
			pixel.blue = total;
            resultmap.setPixel(x,y,pixel);
        }
    }

    for(unsigned int x=0; x<resultmap.getWidth(); ++x)
	for(unsigned int y=0; y<resultmap.getHeight(); ++y)
    {
        float currentvalue = resultmap.getPixel(x,y).red;

        currentvalue = (currentvalue-minvalue)/(maxvalue - minvalue);
        currentvalue = currentvalue * (vmax - vmin) + vmin;

		Pixel px;
		px.red = currentvalue;
		px.green = currentvalue;
		px.blue = currentvalue;
        resultmap.setPixel(x,y,px);
    }
}

ImageData& Perlin::getSeedMap()
{
    return seedmap;
}

ImageData& Perlin::getOctave(int i)
{
    return octavemap[i];
}

ImageData& Perlin::getResult()
{
    return resultmap;
}

float Perlin::interpolate(float a, float b, float x)
{
    //linear
    //return a+(b-a)*x;

    //cos
    float ft = x * 3.1415927f;
	float f = (1.f - cos(ft)) * .5f;
	return  a*(1.f-f) + b*f;

    //cubic

}

float Perlin::interpolatedSeed(float x, float y, float freq)
{
    float w = seedmap.getWidth();
    float h = seedmap.getHeight();

    float x1, x2, y1, y2;

    float pasx = trunc(w/freq);
    float pasy = trunc(h/freq);

    x1 = trunc(x/pasx) * pasx;
    y1 = trunc(y/pasy) * pasy;

    x2 = (x1+pasx);
    if(x2 >= w)
        x2 -= w;
    y2 = (y1+pasy);
    if(y2 >= h)
        y2 -= h;

    //fprintf(stdout, "%f;%f;%f;%f\n", x1, y1, x2, y2);

    float v11 = seedmap.getPixel(x1,y1).red;
    float v21 = seedmap.getPixel(x2,y1).red;
    float v12 = seedmap.getPixel(x1,y2).red;
    float v22 = seedmap.getPixel(x2,y2).red;

    //fprintf(stdout, "%f;%f;%f;%f\n", v11, v12, v21, v22);

    float inter1 = interpolate(v11, v21, (x-x1)/pasx);
    float inter2 = interpolate(v12, v22, (x-x1)/pasx);
    float result = interpolate(inter1, inter2, (y-y1)/pasy);

    //fprintf(stdout, "%f;%f;%f\n", inter1, inter2, result);

    return result;
}
