#ifndef PERLIN_H
#define PERLIN_H

#include "base/impBase.h"

#include <ctime>
#include <cstdlib>
#include <iostream>
#include <cmath>

#define PERLIN_OCTAVE_COUNT 6

#define INDEX_XY(x,y,w,h) ((int)(x*h+y))

struct IMP_API t_perlinMap
{
    int width;
    int height;
    float* data;
};


class IMP_API Perlin
{
    public:
        Perlin(int w, int h, int vmin, int vmax);
        virtual ~Perlin();

        void generateSeedMap();
        void smoothSeedMap();
        float smoothPoint(int x, int y);
        void generateOctaveMap(int i);
        void compileResult();

        t_perlinMap getSeedMap();
        t_perlinMap getOctave(int i);
        t_perlinMap getResult();
    protected:

        float interpolate(float a, float b, float x);
        float interpolatedSeed(float x, float y, float freq);
    private:

        int vmin, vmax;

        t_perlinMap seedmap;
        t_perlinMap octavemap[PERLIN_OCTAVE_COUNT];
        t_perlinMap resultmap;
};

#endif // PERLIN_H
