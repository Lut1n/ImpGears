#include "Core/State.h"

#include <cstdlib>
#include <cstdio>
#include <iostream>

IMPGEARS_BEGIN

//--------------------------------------------------------------
State* State::m_singleton = IMP_NULL;

//--------------------------------------------------------------
State::State()
{

	xMouse = yMouse = 0.0f;
    xdep = ydep = 0.0f;

    if(m_singleton != IMP_NULL)
        delete m_singleton;

    m_singleton = this;

	_windowWidth = 800;
	_windowHeight = 600;
	_mouseOverWindows = true;
}

//--------------------------------------------------------------
State::~State()
{
    //dtor
}

IMPGEARS_END
