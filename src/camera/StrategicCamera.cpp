#include "camera/StrategicCamera.h"
#include "base/State.h"

IMPGEARS_BEGIN

//--------------------------------------------------------------
StrategicCamera::StrategicCamera()
{
    setPosition(imp::Vector3(100.f, 100.f, 100.f));
    setTarget(getPosition()+Vector3(0.f, 1.f, -1.f));
	setDefaultKeyBinding();
    updateFov();
}

//--------------------------------------------------------------
StrategicCamera::~StrategicCamera()
{
    //dtor
}

//--------------------------------------------------------------
void StrategicCamera::initialize(){
}

//--------------------------------------------------------------
void StrategicCamera::onEvent(const imp::Event& evn){
}

//--------------------------------------------------------------
void StrategicCamera::update(){
    float speed = 0.4f;

    State* state = State::getInstance();

    if(state->m_pressedKeys[m_forwardKey] && !state->m_pressedKeys[m_backKey]){
        move(imp::Vector3(0.f, speed, 0.f));
    }
    else if(state->m_pressedKeys[m_backKey] && !state->m_pressedKeys[m_forwardKey]){
        move(imp::Vector3(0.f, -speed, 0.f));
    }

    if(state->m_pressedKeys[m_leftKey] && !state->m_pressedKeys[m_rightKey]){
        move(imp::Vector3(-speed, 0.f, 0.f));
    }
    else if(state->m_pressedKeys[m_rightKey] && !state->m_pressedKeys[m_leftKey]){
        move(imp::Vector3(speed, 0.f, 0.f));
    }

    if(state->m_pressedKeys[m_zoomOutKey])
    {
        move(imp::Vector3(0.f, 0.f, 1.f));
    }
    else if(state->m_pressedKeys[m_zoomInKey])
    {
        move(imp::Vector3(0.f, 0.f, -1.f));
    }

    updateFov();
}

//--------------------------------------------------------------
void StrategicCamera::setKeyBindingConfig(const KeyBindingConfig& binding)
{
	m_forwardKey = binding.getKey("forward");
	m_backKey = binding.getKey("back");
	m_leftKey = binding.getKey("left");
	m_rightKey = binding.getKey("right");
	m_zoomInKey = binding.getKey("zoomIn");
	m_zoomOutKey = binding.getKey("zoomOut");
}

//--------------------------------------------------------------
void StrategicCamera::setDefaultKeyBinding()
{
	m_forwardKey = Event::Z;
	m_backKey = Event::S;
	m_leftKey = Event::Q;
	m_rightKey = Event::D;
	m_zoomInKey = Event::R;
	m_zoomOutKey = Event::F;
}

IMPGEARS_END
