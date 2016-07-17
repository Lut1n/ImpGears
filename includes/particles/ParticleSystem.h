#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include "base/impBase.h"

#include <GL/glew.h>
#include <vector>
#include <ctime>
#include <stdlib.h>
#include <cmath>
#include "base/Timer.h"

#define NB_PARTICLES 10000

#define UPDATE_MIN_LIMIT 15

#define GRAVITY 0.000005f

struct pos_t
{
    float x;
    float y;
    float z;
} ;

typedef std::vector<bool> boolVector_t;
typedef std::vector<pos_t> posVector_t;
typedef std::vector<unsigned int> intVector_t;


class ParticleSystem
{
    public:

        enum RenderType
        {
                RenderType_POINTS,
                RenderType_LINES
        };


        ParticleSystem(RenderType renderType = RenderType_LINES, float pps = 1000.f, GLuint tx = 0);
        virtual ~ParticleSystem();

        void update();
        void draw();

        static int randValue(int min, int max);
    protected:
    private:
    RenderType renderType;
    float timeBetweenP;
    GLuint tx;
    imp::Timer timectrl;
    imp::Timer timer;

        pos_t nullpos;

    boolVector_t actives;
    posVector_t positions;
    posVector_t speeds;
    intVector_t ttls;
};

#endif // PARTICLESYSTEM_H
