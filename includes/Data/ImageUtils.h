#ifndef IMP_IMAGE_UTILS_H
#define IMP_IMAGE_UTILS_H


#include "Core/impBase.h"
#include "Graphics/ImageData.h"

#include <ctime>
#include <cmath>
#include <cstdlib>

IMPGEARS_BEGIN

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

void IMP_API applyBilinearInterpo(ImageData& bitmap, float frqX, float frqY);

inline imp::Pixel IMP_API mirGet(imp::ImageData& img, unsigned int x, unsigned int y);

void IMP_API drawCosL(imp::ImageData& img, float frq, float ampl, double perturb);

void IMP_API drawCos(imp::ImageData& img, float frq, float ampl, double perturb);

void IMP_API applyPerturbation(imp::ImageData& img, const imp::ImageData& normals, float intensity);

void IMP_API applyColorization(imp::ImageData& img, const imp::Pixel& color1, const imp::Pixel& color2);

IMPGEARS_END

#endif // IMP_IMAGE_UTILS_H
