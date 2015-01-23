#ifndef TIMER_H
#define TIMER_H

#include "base/impBase.hpp"
#include <ctime>

IMPGEARS_BEGIN

class Timer
{
    public:
        Timer();
        virtual ~Timer();

        unsigned int getTime();
        void reset();
    protected:
    private:
    clock_t t;
//    IMPuint8 m;
};

IMPGEARS_END

#endif // TIMER_H
