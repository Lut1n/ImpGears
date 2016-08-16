#ifndef PERLIN_H
#define PERLIN_H

#include "Core/impBase.h"
#include <Graphics/ImageData.h>

#include <ctime>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <vector>
#include <cstring>

#define PERLIN_OCTAVE_COUNT 6

#define INDEX_XY(x,y,w,h) ((int)(x*h+y))

class IMP_API Perlin
{
    public:

		class FloatMap
		{
			public:
			FloatMap(int w, int h) { _buff = new float[w*h]; raw=w; size=w*h; }
			~FloatMap() {delete [] _buff;}
			float get(int x, int y) { return _buff[y*raw+x]; }
			void set(int x, int y, float v) { _buff[y*raw+x] = v; }
			float* _buff;
			int raw;
			int size;
			void clone(FloatMap& other){memcpy(_buff, other._buff, size*sizeof(float));}
		};

		enum GenMethod
		{
			GenMethod_Undefined = 0,
			GenMethod_Scalar,
			GenMethod_Gradient
		};

		enum RandMethod
		{
			RandMethod_Undefined = 0,
			RandMethod_System,
			RandMethod_HugoElias,
			RandMethod_PermTable_System,
			RandMethod_PermTable_HugoElias
		};

		enum Interpo
		{
			Interpo_Undefined = 0,
			Interpo_Linear,
			Interpo_Cosinus,
			Interpo_Cubic,
			Interpo_Hermite,
			INterpo_Polynomial5
		};

		class Filter
		{
			public:
			int order;
			double frequency;
			double persistance;
		};

		class Config
		{
			public:

			GenMethod _genMethod;
			RandMethod _randMethod;
			Interpo _interpo;

			double valueMin;
			double valueMax;
			double resolutionX;
			double resolutionY;

			int octaveCount;
			std::vector<Filter> _octaves;
		};

		Perlin();
		Perlin(const Config& config);

		void setConfig(const Config& config);

        virtual ~Perlin();

        void generateSeedMap();
        void smoothSeedMap();
        float smoothPoint(unsigned int x, unsigned int y);
        void generateOctaveMap(int i);
        void compileResult();

		void apply();

        FloatMap* getSeedMap();
        FloatMap* getOctave(int i);
        imp::ImageData& getResult();
    protected:

        float interpolate(float a, float b, float x);
        float interpolatedSeed(float x, float y, float freq);
    private:

		Config _config;
        int vmin, vmax;

		int _mapSize;
        FloatMap* seedmap;
        std::vector<FloatMap*> octavemap;
        imp::ImageData resultmap;
};

#endif // PERLIN_H
