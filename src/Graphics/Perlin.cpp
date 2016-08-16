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
	seedmap = new FloatMap(_config.resolutionX, _config.resolutionY); //.create(config.resolutionX, config.resolutionY, PixelFormat_RGB8);

	octavemap.resize(config.octaveCount);

    for(unsigned int i=0; i<octavemap.size(); ++i)
    {
        octavemap[i] = new FloatMap(_config.resolutionX, _config.resolutionY); //.create(config.resolutionX, config.resolutionY, PixelFormat_RGB8);
    }

    resultmap.create(_config.resolutionX, _config.resolutionY, PixelFormat_RGB8);

    this->vmin = _config.valueMin;
    this->vmax = _config.valueMax;

    srand(time(NULL));
}

Perlin::~Perlin()
{
    //dtor
	delete seedmap;//.destroy();

    for(unsigned int i=0; i<octavemap.size(); ++i)
        delete octavemap[i];//.destroy();

    resultmap.destroy();
}

void Perlin::generateSeedMap()
{
    for(unsigned int x=0; x<_config.resolutionX; ++x)
    {
        for(unsigned int y=0; y<_config.resolutionY; ++y)
        {
			double val = rand()%(vmax - vmin) + vmin;
			//Pixel pix;
			//pix.red = val;// * 255;
			//pix.green = val;// * 255;
			//pix.blue = val;// * 255;
            seedmap->set(x,y,val);//Pixel(x,y,pix);
        }
    }
}

void Perlin::smoothSeedMap()
{

    FloatMap smoothdata(_config.resolutionX, _config.resolutionY);
//	smoothdata.create(seedmap.getWidth(), seedmap.getHeight(), imp::PixelFormat_BGR8);

    for(unsigned int x=0;x<_config.resolutionX; ++x)
    {
        for(unsigned int y=0; y<_config.resolutionY; ++y)
        {
			//Pixel px;
			//px.red = smoothPoint(x,y);
			//px.green = px.red;
			//px.blue = px.red;
			smoothdata.set(x,y,smoothPoint(x,y));//Pixel(x,y,px);
        }
    }

    //seedmap.destroy();
    seedmap->clone(smoothdata);
}

float Perlin::smoothPoint(unsigned int x, unsigned int y)
{
    float total = 0.f;

unsigned int i2,j2;

    for(unsigned int i=x; i<x+3; ++i)
    {
        i2 = i;
        if(i2 >= _config.resolutionX)
            i2 -= _config.resolutionX;

        for(unsigned int j=y; j<y+3; ++j)
        {
            j2 = j;
            if(j2 >= _config.resolutionY)
                j2 -= _config.resolutionY;

			total += seedmap->get(i2,j2);
            //total += (float)(px.red)/255.0;
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

    for(unsigned int x=0; x<_config.resolutionX; ++x)
    {
        for(unsigned int y=0; y<_config.resolutionY; ++y)
        {
/*			Pixel pix;
			pix.red = ;
			pix.green = pix.red;
			pix.blue = pix.red;*/
            octavemap[i]->set(x,y,interpolatedSeed(x,y,freq) * amplitude);
        }
    }
}

void Perlin::compileResult()
{
    float maxvalue = 0;
    float minvalue = 255;

	FloatMap result(_config.resolutionX, _config.resolutionY);

    for(unsigned int x=0; x<_config.resolutionX; ++x)
    {
        for(unsigned int y=0; y<_config.resolutionY; ++y)
        {
            float total = 0;
            for(unsigned int i=0; i<octavemap.size(); ++i)
            {
                total += octavemap[i]->get(x,y);
            }

            if(total > maxvalue)maxvalue = total;
            if(total < minvalue)minvalue = total;

			result.set(x,y, total);
			//Pixel pixel;
			//pixel.red = total;
			//pixel.green = total;
			//pixel.blue = total;
            // resultmap.setPixel(x,y,pixel);
        }
    }

    for(unsigned int x=0; x<_config.resolutionX; ++x)
	for(unsigned int y=0; y<_config.resolutionY; ++y)
    {
        float currentvalue = result.get(x,y);// resultmap.getPixel(x,y).red;

        currentvalue = (currentvalue-minvalue)/(maxvalue - minvalue);
        currentvalue = currentvalue * (vmax - vmin) + vmin;

		std::cout << " " << currentvalue << " \n";

		Pixel px;
		px.red = currentvalue;
		px.green = currentvalue;
		px.blue = currentvalue;
        resultmap.setPixel(x,y,px);
    }
}

Perlin::FloatMap* Perlin::getSeedMap()
{
    return seedmap;
}

Perlin::FloatMap* Perlin::getOctave(int i)
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
    float w = _config.resolutionX;
    float h = _config.resolutionY;

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

    float v11 = seedmap->get(x1,y1);
    float v21 = seedmap->get(x2,y1);
    float v12 = seedmap->get(x1,y2);
    float v22 = seedmap->get(x2,y2);

    //fprintf(stdout, "%f;%f;%f;%f\n", v11, v12, v21, v22);

    float inter1 = interpolate(v11, v21, (x-x1)/pasx);
    float inter2 = interpolate(v12, v22, (x-x1)/pasx);
    float result = interpolate(inter1, inter2, (y-y1)/pasy);

    //fprintf(stdout, "%f;%f;%f\n", inter1, inter2, result);

    return result;
}
