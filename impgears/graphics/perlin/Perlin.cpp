#include "Perlin.h"

Perlin::Perlin(int w, int h, int vmin, int vmax)
{
    //ctor
    seedmap.width = w;
    seedmap.height = h;
    seedmap.data = new float[w*h];

    for(int i=0; i<PERLIN_OCTAVE_COUNT; ++i)
    {
        octavemap[i].width = w;
        octavemap[i].height = h;
        octavemap[i].data = new float[w*h];
    }

    resultmap.width = w;
    resultmap.height = h;
    resultmap.data = new float[w*h];

    this->vmin = vmin;
    this->vmax = vmax;

    srand(time(NULL));
}

Perlin::~Perlin()
{
    //dtor
    delete [] seedmap.data;

    for(int i=0; i<PERLIN_OCTAVE_COUNT; ++i)
        delete [] octavemap[i].data;

    delete [] resultmap.data;
}

void Perlin::generateSeedMap()
{
    for(int x=0; x<seedmap.width; ++x)
    {
        for(int y=0; y<seedmap.height; ++y)
        {
            seedmap.data[INDEX_XY(x,y, seedmap.width, seedmap.height)] = rand()%(vmax - vmin) + vmin;
        }
    }
}

void Perlin::smoothSeedMap()
{
    int w = seedmap.width;
    int h = seedmap.height;

    float* smoothdata = new float[w*h];

    for(int x=0;x<w; ++x)
    {
        for(int y=0; y<h; ++y)
        {
            smoothdata[INDEX_XY(x,y,w,h)] = smoothPoint(x,y);
        }
    }

    delete [] seedmap.data;
    seedmap.data = smoothdata;
}

float Perlin::smoothPoint(int x, int y)
{
    float total = 0.f;

    int i2,j2;

    for(int i=x; i<x+3; ++i)
    {
        i2 = i;
        if(i2 >= seedmap.width)
            i2 -= seedmap.width;

        for(int j=y; j<y+3; ++j)
        {
            j2 = j;
            if(j2 >= seedmap.height)
                j2 -= seedmap.height;

            total += seedmap.data[INDEX_XY(i2,j2, seedmap.width, seedmap.height)];
        }
    }

    return total/9;
}

void Perlin::generateOctaveMap(int i)
{
    float freq = pow(2.f, (float)i+2);
    float persistence = 0.75f;
    float amplitude = pow(persistence, (float)i);

    for(int x=0; x<octavemap[i].width; ++x)
    {
        for(int y=0; y<octavemap[i].height; ++y)
        {
            octavemap[i].data[INDEX_XY(x,y,octavemap[i].width,octavemap[i].height)] = interpolatedSeed(x,y,freq) * amplitude;
        }
    }
}

void Perlin::compileResult()
{
    float maxvalue = 0;
    float minvalue = 255;

    for(int x=0; x<resultmap.width; ++x)
    {
        for(int y=0; y<resultmap.height; ++y)
        {
            float total = 0;
            for(int i=0; i<PERLIN_OCTAVE_COUNT; ++i)
            {
                total += octavemap[i].data[INDEX_XY(x,y,resultmap.width, resultmap.height)];
            }

            if(total > maxvalue)maxvalue = total;
            if(total < minvalue)minvalue = total;

            resultmap.data[INDEX_XY(x,y,resultmap.width, resultmap.height)] = total;
        }
    }

    for(int i=0; i<resultmap.width*resultmap.height; ++i)
    {
        float currentvalue = resultmap.data[i];

        currentvalue = (currentvalue-minvalue)/(maxvalue - minvalue);
        currentvalue = currentvalue * (vmax - vmin) + vmin;

        resultmap.data[i] = currentvalue;
    }
}

t_perlinMap Perlin::getSeedMap()
{
    return seedmap;
}

t_perlinMap Perlin::getOctave(int i)
{
    return octavemap[i];
}

t_perlinMap Perlin::getResult()
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
    float w = seedmap.width;
    float h = seedmap.height;

    float x1, x2, y1, y2;

    float pasx = trunc(w/freq);
    float pasy = trunc(h/freq);

    x1 = trunc(x/pasx) * pasx;
    y1 = trunc(y/pasy) * pasy;

    x2 = (x1+pasx);
    if(x2 >= seedmap.width)
        x2 -= seedmap.width;
    y2 = (y1+pasy);
    if(y2 >= seedmap.height)
        y2 -= seedmap.height;

    //fprintf(stdout, "%f;%f;%f;%f\n", x1, y1, x2, y2);

    float v11 = seedmap.data[INDEX_XY(x1,y1,w,h)];
    float v21 = seedmap.data[INDEX_XY(x2,y1,w,h)];
    float v12 = seedmap.data[INDEX_XY(x1,y2,w,h)];
    float v22 = seedmap.data[INDEX_XY(x2,y2,w,h)];

    //fprintf(stdout, "%f;%f;%f;%f\n", v11, v12, v21, v22);

    float inter1 = interpolate(v11, v21, (x-x1)/pasx);
    float inter2 = interpolate(v12, v22, (x-x1)/pasx);
    float result = interpolate(inter1, inter2, (y-y1)/pasy);

    //fprintf(stdout, "%f;%f;%f\n", inter1, inter2, result);

    return result;
}
