#include "base/State.h"

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
