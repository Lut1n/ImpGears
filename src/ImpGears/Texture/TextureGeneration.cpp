#include <Texture/TextureGeneration.h>

#include <iostream>
#include <cstdlib>
#include <ctime>

IMPGEARS_BEGIN

NoiseGenerator::NoiseGenerator()
	: _interpolator(nullptr)
{

}

NoiseGenerator::NoiseGenerator(const Config& config)
	: _interpolator(nullptr)
{
	setConfig(config);
}

void NoiseGenerator::setConfig(const Config& config)
{
	_config = config;

	seedmap = new FloatMap(_config.resolutionX, _config.resolutionY);

	octavemap.resize(config.octaveCount);

    for(unsigned int i=0; i<octavemap.size(); ++i)
    {
        octavemap[i] = new FloatMap(_config.resolutionX, _config.resolutionY);
    }

    resultmap.build(_config.resolutionX, _config.resolutionY, PixelFormat_RGB8);

    this->vmin = _config.valueMin;
    this->vmax = _config.valueMax;

    srand(time(NULL));

	if(_interpolator)
		delete _interpolator;

	_interpolator = new Coserp();
}

NoiseGenerator::~NoiseGenerator()
{
	delete seedmap;

    for(unsigned int i=0; i<octavemap.size(); ++i)
        delete octavemap[i];

    resultmap.destroy();

	if(_interpolator)
		delete _interpolator;
}

void NoiseGenerator::generateSeedMap()
{
    for(unsigned int x=0; x<_config.resolutionX; ++x)
    {
        for(unsigned int y=0; y<_config.resolutionY; ++y)
        {
			double val = rand()%(vmax - vmin) + vmin;
            seedmap->set(x,y,val);
        }
    }
}

void NoiseGenerator::smoothSeedMap()
{

    FloatMap smoothdata(_config.resolutionX, _config.resolutionY);

    for(unsigned int x=0;x<_config.resolutionX; ++x)
    {
        for(unsigned int y=0; y<_config.resolutionY; ++y)
        {
			smoothdata.set(x,y,smoothPoint(x,y));
        }
    }

    seedmap->clone(smoothdata);
}

float NoiseGenerator::smoothPoint(unsigned int x, unsigned int y)
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
        }
    }

    return total/9;
}

void NoiseGenerator::apply()
{
	generateSeedMap();
	smoothSeedMap();
	for(unsigned int octave=0; octave<octavemap.size(); ++octave)
		generateOctaveMap(octave);

	compileResult();
}

void NoiseGenerator::generateOctaveMap(int i)
{
    float freq = pow(2.f, (float)i+2);
    float persistence = 0.75f;
    float amplitude = pow(persistence, (float)i);

    for(unsigned int x=0; x<_config.resolutionX; ++x)
    {
        for(unsigned int y=0; y<_config.resolutionY; ++y)
        {
            octavemap[i]->set(x,y,interpolatedSeed(x,y,freq) * amplitude);
        }
    }
}

void NoiseGenerator::compileResult()
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
        }
    }

    for(unsigned int x=0; x<_config.resolutionX; ++x)
	for(unsigned int y=0; y<_config.resolutionY; ++y)
    {
        float currentvalue = result.get(x,y);

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

NoiseGenerator::FloatMap* NoiseGenerator::getSeedMap()
{
    return seedmap;
}

NoiseGenerator::FloatMap* NoiseGenerator::getOctave(int i)
{
    return octavemap[i];
}

ImageData& NoiseGenerator::getResult()
{
    return resultmap;
}

float NoiseGenerator::interpolatedSeed(float x, float y, float freq)
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

    float v11 = seedmap->get(x1,y1);
    float v21 = seedmap->get(x2,y1);
    float v12 = seedmap->get(x1,y2);
    float v22 = seedmap->get(x2,y2);

    float inter1 = (*_interpolator)(v11, v21, (x-x1)/pasx);
    float inter2 = (*_interpolator)(v12, v22, (x-x1)/pasx);
    float result = (*_interpolator)(inter1, inter2, (y-y1)/pasy);

    return result;
}

IMPGEARS_END
