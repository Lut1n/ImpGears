#ifndef IMP_TIMER_H
#define IMP_TIMER_H

#include "base/impBase.h"
#include <ctime>

IMPGEARS_BEGIN

class IMP_API Timer
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
