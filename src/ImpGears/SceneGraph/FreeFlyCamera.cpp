#include <SceneGraph/FreeFlyCamera.h>
#include <SceneGraph/OpenGL.h>
#include "Core/State.h"
#include <Utils/EvnContextInterface.h>

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
	setDefaultKeyBinding();

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
        if(state->m_pressedKeys[m_accelerateKey] == imp::State::ActionState_True)
			speed *= 2;
        else if(state->m_pressedKeys[m_decelerateKey] == imp::State::ActionState_True)
			speed *= 0.2f;

        if(state->m_pressedKeys[m_backKey] == imp::State::ActionState_True)
			move(getForwardVector() * -speed * (elapsed/TIME_UPDATE));
        if(state->m_pressedKeys[m_forwardKey] == imp::State::ActionState_True)
			move(getForwardVector() * speed * (elapsed/TIME_UPDATE));
        if(state->m_pressedKeys[m_leftKey] == imp::State::ActionState_True)
			move(getLateralVector() * -speed * (elapsed/TIME_UPDATE));
        if(state->m_pressedKeys[m_rightKey] == imp::State::ActionState_True)
			move(getLateralVector() * speed * (elapsed/TIME_UPDATE));

        updateFov();

        imp::EvnContextInterface::getInstance()->setCursorPosition(0, xref, yref);

        m_cursorTimer.reset();
    }
}

//--------------------------------------------------------------
void FreeFlyCamera::setKeyBindingConfig(const KeyBindingConfig& binding)
{
	m_forwardKey = binding.getKey("forward");
	m_backKey = binding.getKey("back");
	m_leftKey = binding.getKey("left");
	m_rightKey = binding.getKey("right");
	m_accelerateKey = binding.getKey("accelerate");
	m_decelerateKey = binding.getKey("decelerate");
}

//--------------------------------------------------------------
void FreeFlyCamera::setDefaultKeyBinding()
{
	m_forwardKey = Event::Z;
	m_backKey = Event::S;
	m_leftKey = Event::Q;
	m_rightKey = Event::D;
	m_accelerateKey = Event::LShift;
	m_decelerateKey = Event::LControl;
}

IMPGEARS_END
