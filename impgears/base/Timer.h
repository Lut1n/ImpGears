#ifndef TIMER_H
#define TIMER_H

#include <ctime>

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

#endif // TIMER_H
