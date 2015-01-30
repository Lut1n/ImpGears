#include "State.h"

#include <cstdlib>
#include <cstdio>

IMPGEARS_BEGIN

//--------------------------------------------------------------
State* State::m_singleton = IMP_NULL;

//--------------------------------------------------------------
State::State()
{

    key_up = key_down = key_left = key_right = mapUp_down = mapDown_down = false;
    key_shift = key_ctrl = false;
    zoomUp_down = zoomDown_down = false;
    save_down = false;
    xdep = ydep = 0.0f;

    if(m_singleton != IMP_NULL)
        delete m_singleton;

    m_singleton = this;
}

//--------------------------------------------------------------
State::~State()
{
    //dtor
}

//--------------------------------------------------------------
void State::onEvent(const imp::Event& event)
{
    imp::EvnContextInterface* evnContext = imp::EvnContextInterface::getInstance();

    // Close window : exit
    if(event.getType() == imp::Event::Type_WindowClose)
        evnContext->destroyWindow(0);
    if(event.getType() == imp::Event::Type_KeyPressed){
        if(event.getKeyboard().keyCode == imp::Event::Q){
            //rotationY-=1.f;
            key_left = true;
        }
        else if(event.getKeyboard().keyCode == imp::Event::D){
            //rotationY+=1.f;
            key_right = true;
        }
        if(event.getKeyboard().keyCode == imp::Event::S){
            //rotationX-=1.f;
            key_down = true;
        }
        else if(event.getKeyboard().keyCode == imp::Event::Z){
            //rotationX+=1.f;
            key_up = true;
        }
        if(event.getKeyboard().keyCode == imp::Event::LShift){
            key_shift = true;
        }
        if(event.getKeyboard().keyCode == imp::Event::LControl){
            key_ctrl = true;
        }
        if(event.getKeyboard().keyCode == imp::Event::Escape){
            exit(0);
        }
        if(event.getKeyboard().keyCode == imp::Event::PageDown)
        {
            mapDown_down = true;
        }
        if(event.getKeyboard().keyCode == imp::Event::PageUp)
        {
            mapUp_down = true;
        }
        if(event.getKeyboard().keyCode == imp::Event::Up)
        {
            zoomUp_down = true;
        }
        if(event.getKeyboard().keyCode == imp::Event::Down)
        {
            zoomDown_down = true;
        }
        if(event.getKeyboard().keyCode == imp::Event::F)
        {
            save_down = true;
        }
    }
    if(event.getType() == imp::Event::Type_KeyReleased){
        if(event.getKeyboard().keyCode == imp::Event::Q){
            key_left = false;
        }
        else if(event.getKeyboard().keyCode == imp::Event::D){
            key_right = false;
        }
        if(event.getKeyboard().keyCode == imp::Event::S){
            key_down = false;
        }
        else if(event.getKeyboard().keyCode == imp::Event::Z){
            key_up = false;
        }
        if(event.getKeyboard().keyCode == imp::Event::LShift){
            key_shift = false;
        }
        if(event.getKeyboard().keyCode == imp::Event::LControl){
            key_ctrl = false;
        }
        if(event.getKeyboard().keyCode == imp::Event::PageDown)
        {
            mapDown_down = false;
        }
        if(event.getKeyboard().keyCode == imp::Event::PageUp)
        {
            mapUp_down = false;
        }
        if(event.getKeyboard().keyCode == imp::Event::Up)
        {
            zoomUp_down = false;
        }
        if(event.getKeyboard().keyCode == imp::Event::Down)
        {
            zoomDown_down = false;
        }
        if(event.getKeyboard().keyCode == imp::Event::F)
        {
            save_down = false;
        }
    }
    if(event.getType() == imp::Event::Type_MouseMoved)
    {
        double nxdep = (event.getMouse().x - WIN_W/2.0) * SENSIBILITY;
        double nydep = (event.getMouse().y - WIN_H/2.0) * SENSIBILITY;

        if( (nxdep < 0.0 && nxdep<xdep)
        || (nxdep > 0.0 && nxdep>xdep) )xdep = nxdep;

        if( (nydep < 0.0 && nydep<ydep)
        || (nydep > 0.0 && nydep>ydep) )ydep = nydep;
    }
}

IMPGEARS_END
