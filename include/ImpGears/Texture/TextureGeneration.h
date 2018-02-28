#ifndef IMP_TEXTURE_GENERATION_H
#define IMP_TEXTURE_GENERATION_H

#include <Core/Object.h>
#include <Core/Perlin.h>


IMPGEARS_BEGIN


class Interpolator
{
public:
	Interpolator() {}
	virtual ~Interpolator() {}

	virtual float operator() (float a, float b, float t) = 0;
};

class Coserp : public Interpolator
{
public:
	Coserp() {}
	virtual ~Coserp() {}

	virtual float operator() (float a, float b, float t)
	{
		float ft = t * 3.1415927f;
		float f = (1.f - cos(ft)) * .5f;
		return  a*(1.f-f) + b*f;
	}
};

class IMP_API NoiseGenerator
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

		NoiseGenerator();
		NoiseGenerator(const Config& config);

		void setConfig(const Config& config);

        virtual ~NoiseGenerator();

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

        float interpolatedSeed(float x, float y, float freq);
    private:

		Config _config;
        int vmin, vmax;

		int _mapSize;
        FloatMap* seedmap;
        std::vector<FloatMap*> octavemap;
        imp::ImageData resultmap;

		Interpolator* _interpolator;
};

IMPGEARS_END

#endif // IMP_TEXTURE_GENERATION_H
