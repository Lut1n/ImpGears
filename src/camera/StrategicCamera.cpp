#include "camera/StrategicCamera.h"
#include "base/State.h"

IMPGEARS_BEGIN

//--------------------------------------------------------------
StrategicCamera::StrategicCamera()
{
    setPosition(imp::Vector3(100.f, 100.f, 100.f));
    setTarget(getPosition()+Vector3(0.f, 1.f, -1.f));
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

    if(state->key_up && !state->key_down){
        move(imp::Vector3(0.f, speed, 0.f));
    }
    else if(state->key_down && !state->key_up){
        move(imp::Vector3(0.f, -speed, 0.f));
    }

    if(state->key_left && !state->key_right){
        move(imp::Vector3(-speed, 0.f, 0.f));
    }
    else if(state->key_right && !state->key_left){
        move(imp::Vector3(speed, 0.f, 0.f));
    }

    if(state->zoomUp_down)
    {
        move(imp::Vector3(0.f, 0.f, 1.f));
    }
    else if(state->zoomDown_down)
    {
        move(imp::Vector3(0.f, 0.f, -1.f));
    }

    updateFov();
}

IMPGEARS_END
