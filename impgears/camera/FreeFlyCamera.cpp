#include "FreeFlyCamera.h"
#include "graphics/GLcommon.h"
#include "base/State.h"
#include "EvnContextInterface.h"

#include <cmath>

IMPGEARS_BEGIN

#define TIME_UPDATE 10.f

#define SPEED 0.2f

//--------------------------------------------------------------
FreeFlyCamera::FreeFlyCamera(float xref, float yref, const imp::Vector3& position, const imp::Vector3& target)
{
    this->xref = xref;
    this->yref = yref;

    setPosition(imp::Vector3(100.f, 100.f, 100.f)); //temp

    m_cursorTimer.reset();
}

//--------------------------------------------------------------
FreeFlyCamera::~FreeFlyCamera()
{
    //dtor
}

//--------------------------------------------------------------
void FreeFlyCamera::initialize(){
}

//--------------------------------------------------------------
void FreeFlyCamera::onEvent(const imp::Event& evn){
}

//--------------------------------------------------------------
void FreeFlyCamera::update(){

    imp::State* state = imp::State::getInstance();

    float elapsed = m_cursorTimer.getTime();
    if( elapsed > TIME_UPDATE)
    {
        float theta = -(float)state->xdep * (elapsed/TIME_UPDATE);
        if( theta < 0.f ) theta += 3.14159*2.f;
        else if( theta > 3.14159*2.f ) theta -= 3.14159*2.f;

        float phi = -(float)state->ydep * (elapsed/TIME_UPDATE);
        if( phi <= -3.14159f/2.f || phi >= 3.14159f/2.f )phi += (float)state->ydep * (elapsed/TIME_UPDATE);

        rotate(theta, phi);

        state->xdep = state->ydep = 0.0;

        float speed = SPEED;
        if(state->key_shift)speed *= 2;

        if(state->key_down)move(getForwardVector() * -speed * (elapsed/TIME_UPDATE));
        if(state->key_up)move(getForwardVector() * speed * (elapsed/TIME_UPDATE));
        if(state->key_left)move(getLateralVector() * -speed * (elapsed/TIME_UPDATE));
        if(state->key_right)move(getLateralVector() * speed * (elapsed/TIME_UPDATE));

        updateFov();

        imp::EvnContextInterface::getInstance()->setCursorPosition(0, WIN_W/2.f, WIN_H/2.f);

        m_cursorTimer.reset();
    }
}

IMPGEARS_END
