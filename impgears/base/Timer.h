#ifndef IMP_TIMER_H
#define IMP_TIMER_H

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

#endif // IMP_TIMER_H
