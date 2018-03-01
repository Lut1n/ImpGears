#ifndef IMP_IMAGE_UTILS_H
#define IMP_IMAGE_UTILS_H

#include <vector>

#include <Core/Object.h>
#include <SceneGraph/ImageData.h>
#include <Core/Math.h>

#include <ctime>
#include <cmath>
#include <cstdlib>

IMPGEARS_BEGIN

enum SignalType
{
	SignalType_Unknown = 0,
	SignalType_Sinus,
	SignalType_Square,
	SignalType_Triangle,
};

class Distribution
{
	public:

	struct KV
	{
		double first;
		double second;
	};

	typedef std::vector<KV> KVVector;

	Distribution()
	{
	}

	double operator()(double t)
	{
		double result = 0.0;
		for(unsigned int i=0; i<_values.size(); ++i)
		{
			if(i+1 <_values.size() && _values[i].first <= t && _values[i+1].first > t)
			{
				double local = t - _values[i].first;
				double length = _values[i+1].first - _values[i].first;
				result = imp::Lerp(_values[i].second, _values[i+1].second, local/length);
				break;
			}
			else
			{
				result = _values[i].second;
			}
		}
		return result;
	}

	~Distribution()
	{
	}

	KVVector _values;
};

class IMP_API Randomizer
{
public:
	Randomizer(int seed = 0)
	{
		std::srand(std::time(NULL));
	}

	~Randomizer(){}

	int pull()
	{
		return std::rand();
	}
};

static Randomizer randomizer;

struct IMP_API Vec3
{
	float x;
	float y;
	float z;
};

float IMP_API min(float a, float b);

float IMP_API max(float a, float b);

float IMP_API clamp(float x, float edge0, float edge1);

float IMP_API floor(float v);

float IMP_API frac(float v);

void IMP_API normalize(Vec3& v);

void IMP_API cross(Vec3 v1, Vec3 v2, Vec3& result);

float IMP_API smoothstep(char edge0, char edge1, float delta);

Pixel IMP_API lerp(Pixel a, Pixel b, float delta);

Pixel IMP_API bilerp(Pixel oo, Pixel xo, Pixel oy, Pixel xy, float deltaX, float deltaY);

void IMP_API heightToNormal(ImageData& in, ImageData& out, float force, float prec);

void IMP_API blend(ImageData& dst, ImageData& src, float alpha);

void IMP_API blend(ImageData& dst, ImageData& src, ImageData& alphaMap, double threshold);

void IMP_API applyBilinearInterpo(ImageData& bitmap, float frqX, float frqY);

inline imp::Pixel IMP_API mirGet(imp::ImageData& img, unsigned int x, unsigned int y);

void IMP_API drawDirectionnalSinus(imp::ImageData& img, double dirX, double dirY, float freq, float ampl, float maxPeriodRatio = 0.5, const imp::ImageData* perturbation = nullptr, float perturbIntensity = 1.0);

void IMP_API drawDirectionnalSignal(imp::ImageData& img, SignalType signaltype, double dirX, double dirY, float freq, float ampl, float maxPeriodRatio = 0.5, const imp::ImageData* perturbation = nullptr, float perturbIntensity = 1.0);

void IMP_API drawRadialSinus(imp::ImageData& img, double posX, double posY, float freq, float ampl, float maxPeriodRatio = 0.5, const imp::ImageData* perturbation = nullptr, float perturbIntensity = 1.0);

void IMP_API applyPerturbation(imp::ImageData& img, const imp::ImageData& normals, float intensity);

void IMP_API applyColorization(imp::ImageData& img, const imp::Pixel& color1, const imp::Pixel& color2);

void IMP_API applyColorization(imp::ImageData& img, const imp::Pixel& color1, const imp::Pixel& color2, Distribution& distrib);

void IMP_API applyMaximization(imp::ImageData& img);

void IMP_API drawCellularNoise(imp::ImageData& img, unsigned int cellcount, const imp::ImageData* noisemap);

IMPGEARS_END

#endif // IMP_IMAGE_UTILS_H
