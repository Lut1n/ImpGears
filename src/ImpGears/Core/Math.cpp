#include <Core/Math.h>

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
    std::int64_t iMin = (std::int64_t)(min/precision);
    std::int64_t iMax = (std::int64_t)(max/precision);

    std::int64_t iRand = rand()%(iMax-iMin)+iMin;

    return (float)(iRand)*precision;
}

//--------------------------------------------------------------
float lerp(float a, float b, float value)
{
    return a + value*(b-a);
}

IMPGEARS_END
