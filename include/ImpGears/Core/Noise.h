#ifndef IMP_NOISE_H
#define IMP_NOISE_H

#include <ImpGears/Core/Object.h>

#include <cstring>
#include <cmath>
#include <vector>

IMPGEARS_BEGIN


double IMP_API perlin(double x, double y, double z, int tileSize);

double IMP_API fbmPerlin(double x, double y, double z, unsigned int octaveCount, double persistence, double freq = 1.0, double tiles = 1.0);

double IMP_API simplex(double x, double y, int tileSize);

double IMP_API fbmSimplex(double x, double y, double z, unsigned int octaveCount, double persistence, double freq = 1.0, double tiles = 1.0);

IMPGEARS_END

#endif // IMP_NOISE_H
