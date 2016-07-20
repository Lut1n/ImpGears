#include "Graphics/ParticleSystem.h"

ParticleSystem::ParticleSystem(RenderType renderType, float pps, GLuint tx):
    renderType(renderType), timeBetweenP(1.f/pps), tx(tx)
{
    //ctor
        actives.resize(NB_PARTICLES, false);
        nullpos.x = nullpos.y = nullpos.z = 0.f;
        positions.resize(NB_PARTICLES, nullpos);
        speeds.resize(NB_PARTICLES, nullpos);
        ttls.resize(NB_PARTICLES, 0);

        srand(time(NULL));
}

ParticleSystem::~ParticleSystem()
{
    //dtor
}

        void ParticleSystem::update()
        {

        }
        void ParticleSystem::draw()
        {

        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
            unsigned int elapsed = timectrl.getTime();
            for(int i = 0; i<NB_PARTICLES; i++)
            {
                if(actives[i])
                {
                    if(renderType == RenderType_LINES)
                    {
                        glBegin(GL_LINES);
                        glColor4f(0.f, 0.f, 0.f, 0.f);
                        glVertex3f(0.f, 0.f, 0.f);
                        glColor4f(0.2f, 0.3f, 1.f, 1.f);
                        glVertex3f(positions[i].x, positions[i].y, positions[i].z);
                        glEnd();
                    }
                    else
                    {
                        glBegin(GL_POINTS);
                        glColor4f(0.2f, 0.3f, 1.f, 1.f);
                        glVertex3f(positions[i].x, positions[i].y, positions[i].z);
                        glEnd();
                    }

                    if(elapsed > UPDATE_MIN_LIMIT)
                    {
                        positions[i].x += speeds[i].x * (float)elapsed;
                        positions[i].y += speeds[i].y * (float)elapsed;
                        positions[i].z += speeds[i].z * (float)elapsed;

                        //speeds[i].x *= 0.9995 * elapsed;
                        //speeds[i].y *= 0.9995 * elapsed;
                        //speeds[i].z *= 0.9995;
                        speeds[i].z -= GRAVITY * (float)elapsed;

                            if(ttls[i] < elapsed)
                            {
                                actives[i] = false;
                                continue;
                            }
                            else ttls[i] -= elapsed;

                    }
                }
                else
                {
                    if(timer.getTime() >= timeBetweenP)
                    {
                        actives[i] = true;
                        positions[i] = nullpos;
                        pos_t newpos;
                        newpos.x = randValue(-10, 10)/5000.f;
                        newpos.y = randValue(-10, 10)/5000.f;
                        newpos.z = randValue(0, 10)/5000.f;
                        speeds[i] = newpos;
                        ttls[i] = (unsigned int)randValue(7000, 10000);
                        timer.reset();
                    }
                }
            }

            if(elapsed > UPDATE_MIN_LIMIT) timectrl.reset();
        }

int ParticleSystem::randValue(int min, int max)
{
    return (rand()%(max - min) + min);
}
