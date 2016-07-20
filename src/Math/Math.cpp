#include "Core/Math.h"

#include <cstdlib>
#include <ctime>

IMPGEARS_BEGIN

//--------------------------------------------------------------
void initRandom()
{
    srand(time(NULL));
}

//--------------------------------------------------------------
float random(float min, float max, float precision)
{
    Int64 iMin = (Int64)(min/precision);
    Int64 iMax = (Int64)(max/precision);

    Int64 iRand = rand()%(iMax-iMin)+iMin;

    return (float)(iRand)*precision;
}

//--------------------------------------------------------------
float lerp(float a, float b, float value)
{
    return a + value*(b-a);
}

IMPGEARS_END
