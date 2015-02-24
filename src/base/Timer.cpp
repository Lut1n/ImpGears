#include "base/Timer.h"

IMPGEARS_BEGIN

Timer::Timer()
{
    //ctor
    reset();
}

Timer::~Timer()
{
    //dtor
}

void Timer::reset()
{
    t = clock();
}

unsigned int Timer::getTime()
{
    return ((clock() - t) * 1000)/CLOCKS_PER_SEC; // 1000 for ms
}

IMPGEARS_END
