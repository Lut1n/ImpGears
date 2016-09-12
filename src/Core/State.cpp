#include "Core/State.h"

#include <cstdlib>
#include <cstdio>

IMPGEARS_BEGIN

//--------------------------------------------------------------
State* State::m_singleton = IMP_NULL;

//--------------------------------------------------------------
State::State()
{

	for(Uint32 k=0; k<Event::KeyCount; ++k)
		m_pressedKeys[k] = false;

	for(Uint32 b=0; b<Event::Mouse_ButtonCount; ++b)
	        m_pressedMouseButtons[b] = false;

    xdep = ydep = 0.0f;

    if(m_singleton != IMP_NULL)
        delete m_singleton;

    m_singleton = this;

	_windowWidth = 800;
	_windowHeight = 600;
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
	if(event.getType() == imp::Event::Type_WindowClose
		&& event.getKeyboard().keyCode == imp::Event::Escape)
	{
		evnContext->destroyWindow(0);
	}

	if(event.getType() == imp::Event::Type_KeyPressed)
	{
		if(event.getKeyboard().keyCode >= 0
			&& event.getKeyboard().keyCode < imp::Event::KeyCount)
		{
			m_pressedKeys[event.getKeyboard().keyCode] = true;
		}
	}
	else if(event.getType() == imp::Event::Type_KeyReleased)
	{
		if(event.getKeyboard().keyCode >= 0
			&& event.getKeyboard().keyCode < imp::Event::KeyCount)
		{
			m_pressedKeys[event.getKeyboard().keyCode] = false;
		}
	}
       if(event.getType() == imp::Event::Type_MousePressed)
       {
		fprintf(stdout, "m pressed\n");
       		if(event.getMouse().button >= 0
            		&& event.getMouse().button < imp::Event::Mouse_ButtonCount)
        	{
            		m_pressedMouseButtons[event.getMouse().button] = true;
        	}
       }
       else if(event.getType() == imp::Event::Type_MouseReleased)
       {
		fprintf(stdout, "m released\n");
           	if(event.getMouse().button >= 0
            		&& event.getMouse().button < imp::Event::Mouse_ButtonCount)
        	{
            	m_pressedMouseButtons[event.getMouse().button] = false;
        	}
	}

    if(event.getType() == imp::Event::Type_MouseMoved)
    {
        double nxdep = (event.getMouse().x - _windowWidth/2.0) * SENSIBILITY;
        double nydep = (event.getMouse().y - _windowHeight/2.0) * SENSIBILITY;

        if( (nxdep < 0.0 && nxdep<xdep)
        || (nxdep > 0.0 && nxdep>xdep) )xdep = nxdep;

        if( (nydep < 0.0 && nydep<ydep)
        || (nydep > 0.0 && nydep>ydep) )ydep = nydep;
    }
}

IMPGEARS_END
