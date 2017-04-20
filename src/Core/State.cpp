#include "Core/State.h"

#include <cstdlib>
#include <cstdio>
#include <iostream>

IMPGEARS_BEGIN

//--------------------------------------------------------------
State* State::m_singleton = IMP_NULL;

char getKeyValue(Event::Key k, bool shift)
{
	switch(k)
	{
		case Event::A: {return shift? 'A' : 'a';  }
		case Event::B: {return shift?  'B' : 'b';  }
		case Event::C: {return shift?  'C' : 'c';  }
		case Event::D: {return shift?  'D' : 'd';  }
		case Event::E: {return shift?  'E' : 'e';  }
		case Event::F: {return shift?  'F' : 'f';  }
		case Event::G: {return shift?  'G' : 'g';  }
		case Event::H: {return shift?  'H' : 'h';  }
		case Event::I: {return shift?  'I' : 'i';  }
		case Event::J: {return shift?  'J' : 'j';  }
		case Event::K: {return shift?  'K' : 'k';  }
		case Event::L: {return shift?  'L' : 'l';  }
		case Event::M: {return shift?  'M' : 'm';  }
		case Event::N: {return shift?  'N' : 'n';  }
		case Event::O: {return shift?  'O' : 'o';  }
		case Event::P: {return shift?  'P' : 'p';  }
		case Event::Q: {return shift?  'Q' : 'q';  }
		case Event::R: {return shift?  'R' : 'r';  }
		case Event::S: {return shift?  'S' : 's';  }
		case Event::T: {return shift?  'T' : 't';  }
		case Event::U: {return shift?  'U' : 'u';  }
		case Event::V: {return shift?  'V' : 'v';  }
		case Event::W: {return shift?  'W' : 'w';  }
		case Event::X: {return shift?  'X' : 'x';  }
		case Event::Y: {return shift?  'Y' : 'y';  }
		case Event::Z: {return shift?  'Z' : 'z';  }
		case Event::Num0       : return '0';
        case Event::Num1       : return '1';
        case Event::Num2       : return '2';
        case Event::Num3       : return '3';
        case Event::Num4       : return '4';
        case Event::Num5       : return '5';
        case Event::Num6       : return '6';
        case Event::Num7       : return '7';
        case Event::Num8       : return '8';
        case Event::Num9       : return '9';
        case Event::LBracket   : return '{';
	case Event::RBracket   : return '}';
        case Event::SemiColon  : return ';';
        case Event::Comma      : return ',';
        //case Event::Period     : return ' ';
        case Event::Quote      : return '\'';
        case Event::Slash      : return '/';
        case Event::BackSlash  : return '\\';
        case Event::Tilde      : return '~';
        case Event::Equal      : return '=';
        //case Event::Dash       : return '.';
        case Event::Space      : return ' ';
        case Event::Return     : return '\n';
        case Event::Tab        : return '\t';
        case Event::Add        : return '+';
        case Event::Subtract   : return '-';
        case Event::Multiply   : return '*';
        case Event::Divide     : return '/';
        case Event::Numpad0    : return '0';
        case Event::Numpad1    : return '1';
        case Event::Numpad2    : return '2';
        case Event::Numpad3    : return '3';
        case Event::Numpad4    : return '4';
        case Event::Numpad5    : return '5';
        case Event::Numpad6    : return '6';
        case Event::Numpad7    : return '7';
        case Event::Numpad8    : return '8';
        case Event::Numpad9    : return '9';
		default : return ' ';
	}
	return ' ';
}

//--------------------------------------------------------------
State::State()
{

	for(Uint32 k=0; k<Event::KeyCount; ++k)
		m_pressedKeys[k] = ActionState_False;

	for(Uint32 b=0; b<Event::Mouse_ButtonCount; ++b)
	        m_pressedMouseButtons[b] = ActionState_False;

	xMouse = yMouse = 0.0f;
    xdep = ydep = 0.0f;

    if(m_singleton != IMP_NULL)
        delete m_singleton;

    m_singleton = this;

	_windowWidth = 800;
	_windowHeight = 600;
	_lastPressedKey = Event::Unknown;
	_lastReleasedKey = Event::Unknown;
	_mouseOverWindows = true;
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

	_lastPressedKey = Event::Unknown;
	_lastReleasedKey = Event::Unknown;
	
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
			_lastPressedKey = event.getKeyboard().keyCode;
			_keyValue = getKeyValue(_lastPressedKey, event.getKeyboard().shiftPressed);
			m_pressedKeys[event.getKeyboard().keyCode] = ActionState_Pressed;
		}
	}
	else if(event.getType() == imp::Event::Type_KeyReleased)
	{
		if(event.getKeyboard().keyCode >= 0
			&& event.getKeyboard().keyCode < imp::Event::KeyCount)
		{
			_lastReleasedKey = event.getKeyboard().keyCode;
			_keyValue = getKeyValue(_lastReleasedKey, event.getKeyboard().shiftPressed);
			m_pressedKeys[event.getKeyboard().keyCode] = ActionState_Released;
		}
	}
	
	if(event.getType() == imp::Event::Type_MousePressed)
	{
		if(event.getMouse().button >= 0
				&& event.getMouse().button < imp::Event::Mouse_ButtonCount)
		{
				m_pressedMouseButtons[event.getMouse().button] = ActionState_Pressed;
		}
	}
	else if(event.getType() == imp::Event::Type_MouseReleased)
	{
		if(event.getMouse().button >= 0
				&& event.getMouse().button < imp::Event::Mouse_ButtonCount)
		{
			m_pressedMouseButtons[event.getMouse().button] = ActionState_Released;
		}
	}
	
	if(event.getType() == imp::Event::Type_MouseEntered)
	{
		_mouseOverWindows = true;
	}
	else if(event.getType() == imp::Event::Type_MouseLeft)
	{
		_mouseOverWindows = false;
	}

    if(event.getType() == imp::Event::Type_MouseMoved && _mouseOverWindows)
    {
		xMouse = event.getMouse().x;
		yMouse = event.getMouse().y;
        double nxdep = (event.getMouse().x - _windowWidth/2.0) * SENSIBILITY;
        double nydep = (event.getMouse().y - _windowHeight/2.0) * SENSIBILITY;
		
        if( (nxdep < 0.0 && nxdep<xdep)
        || (nxdep > 0.0 && nxdep>xdep) )xdep = nxdep;

        if( (nydep < 0.0 && nydep<ydep)
        || (nydep > 0.0 && nydep>ydep) )ydep = nydep;
    }
}

IMPGEARS_END
