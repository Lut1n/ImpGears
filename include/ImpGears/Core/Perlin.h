#ifndef IMP_PERLINTECH_H
#define IMP_PERLINTECH_H

#include <Core/Object.h>
#include <SceneGraph/ImageData.h>

#include <cstring>
#include <cmath>
#include <vector>

IMPGEARS_BEGIN


double IMP_API perlinMain(double x, double y, double z, int tileSize);

double IMP_API perlinOctave(double x, double y, double z, unsigned int octaveCount, double persistence, double freq = 1.0, double tiles = 1.0);

double IMP_API simplexMain(double x, double y, int tileSize);

double IMP_API simplexOctave(double x, double y, double z, unsigned int octaveCount, double persistence, double freq = 1.0, double tiles = 1.0);

IMPGEARS_END

#endif // IMP_PERLINTECH_H
