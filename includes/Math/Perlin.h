#ifndef IMP_PERLINTECH_H
#define IMP_PERLINTECH_H

#include "Core/impBase.h"
#include <Graphics/ImageData.h>

#include <cstring>
#include <cmath>
#include <vector>

IMPGEARS_BEGIN

double IMP_API perlinMain(double x, double y, double z);

double IMP_API perlinOctave(double x, double y, double z, unsigned int octaveCount, double persistence);

IMPGEARS_END

#endif // IMP_PERLINTECH_H
