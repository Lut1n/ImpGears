#include <Gui/GuiComponent.h>


IMPGEARS_BEGIN

//--------------------------------------------------------------
Shader* GuiComponent::_guiComponentShader = IMP_NULL;

//--------------------------------------------------------------
GuiComponent::GuiComponent(float x, float y, float w, float h)
	: _visible(true)
	, _positionX(x)
	, _positionY(y)
	, _absolutePositionX(0.0)
	, _absolutePositionY(0.0)
{
	setPosition(x,y);
	setSize(w, h);
}

//--------------------------------------------------------------
GuiComponent::~GuiComponent()
{

}

//--------------------------------------------------------------
void GuiComponent::setSize(float w, float h)
{
	_quad.resize(w, h);
}

//--------------------------------------------------------------
void GuiComponent::setPosition(float x, float y)
{
	_positionX = x;
	_positionY = y;
}

//--------------------------------------------------------------
void GuiComponent::setAbsPosition(float x, float y)
{
	_absolutePositionX = x;
	_absolutePositionY = y;
	_quad.setPosition(_absolutePositionX, _absolutePositionY);
}

//--------------------------------------------------------------
void GuiComponent::getAbsPosition(float& x, float& y)
{
	x = _absolutePositionX;
	y = _absolutePositionY;
}

//--------------------------------------------------------------
float GuiComponent::getPositionX() const
{
	return _positionX;
}

//--------------------------------------------------------------
float GuiComponent::getPositionY() const
{
	return _positionY;
}

//--------------------------------------------------------------
float GuiComponent::getSizeX() const
{
	return _quad.getDimension().getX2() - _quad.getDimension().getX1();
}

//--------------------------------------------------------------
float GuiComponent::getSizeY() const
{
	return _quad.getDimension().getY2() - _quad.getDimension().getY1();
}

//--------------------------------------------------------------
void GuiComponent::setClippingRect(const Rectangle& clippingRect)
{
	_quad.setClippingRect(clippingRect);
}

//--------------------------------------------------------------
const Rectangle& GuiComponent::getClippingRect() const
{
	return _quad.getClippingRect();
}

//--------------------------------------------------------------
void GuiComponent::addEventHandler(GuiEventHandler* handler)
{
	_eventHandlers.push_back(handler);
}

//--------------------------------------------------------------
void GuiComponent::removeEventHandler(GuiEventHandler* handler)
{
	// to implement.
}

//--------------------------------------------------------------
void GuiComponent::render(imp::Uint32 passID)
{
	renderComponent(passID, 0.0, 0.0);
}

//--------------------------------------------------------------
void GuiComponent::renderComponent(imp::Uint32 passID, float parentX, float parentY)
{
	if(_visible == false) return;

	_absolutePositionX = parentX+getPositionX();
	_absolutePositionY = parentY+getPositionY();
	_quad.setPosition(_absolutePositionX, _absolutePositionY);

	_layout.resetFreePosition();
	_layout.setParentSurface( Rectangle(getPositionX(), getPositionY(), getPositionX()+getSizeX(), getPositionY()+getSizeY()) );
	
	for(unsigned int i=0; i<_components.size(); ++i)
	{
		_components[i]->setClippingRect( getClippingRect() );

		Rectangle compsurface(
			_components[i]->getPositionX(),
			_components[i]->getPositionY(),
			_components[i]->getPositionX()+_components[i]->getSizeX(),
			_components[i]->getPositionY()+_components[i]->getSizeY() );

		_layout.applyParameters(_components[i]->getLayoutParameters(), compsurface);
		_components[i]->setPosition(compsurface.getX1(), compsurface.getY1());
		_components[i]->setSize(compsurface.getX2() - compsurface.getX1(), compsurface.getY2() - compsurface.getY1());
		
		if( _components[i]->isVisible() )
		{
			_components[i]->renderComponent(passID, parentX+getPositionX(), parentY+getPositionY());
		}
	}
}

//--------------------------------------------------------------
bool GuiComponent::event(imp::State* state, float x, float y)
{

	bool result = false;
	_layout.resetFreePosition();
	_layout.setParentSurface( Rectangle(getPositionX(), getPositionY(), getPositionX()+getSizeX(), getPositionY()+getSizeY()) );

	for(unsigned int i=0; i<_components.size(); ++i)
	{

		Rectangle compsurface(
			_components[i]->getPositionX(),
			_components[i]->getPositionY(),
			_components[i]->getPositionX()+_components[i]->getSizeX(),
			_components[i]->getPositionY()+_components[i]->getSizeY() );

		_layout.applyParameters(_components[i]->getLayoutParameters(), compsurface);
		_components[i]->setPosition(compsurface.getX1(), compsurface.getY1());
		_components[i]->setSize(compsurface.getX2() - compsurface.getX1(), compsurface.getY2() - compsurface.getY1());
		
		if( _components[i]->isVisible() )
		{
			result |= _components[i]->event(state, x+getPositionX(), y+getPositionY());
		}
	}
	
	if(result == false)
	{
		if(_visible == false) return false;

		if(_eventHandlers.empty()) return false;
	
		double absPositionX = _absolutePositionX;
		double absPositionY = _absolutePositionY;

		double mouseX = state->xMouse;
		double mouseY = state->yMouse;
	
		bool mouseClipped = mouseX < getClippingRect().getX1()
							|| mouseX > getClippingRect().getX2()
							|| mouseY < getClippingRect().getY1()
							|| mouseY > getClippingRect().getY2();
	
		bool mouseMove = (mouseX != _previousEventState._mouseX || mouseY != _previousEventState._mouseY);
		bool mouseOver = (true // !mouseClipped
						&& mouseX > absPositionX
						&& mouseX<= absPositionX+getSizeX()
						&& mouseY > absPositionY
						&& mouseY <= absPositionY+getSizeY());
	
		mouseOver = !mouseClipped && mouseOver;
	
		bool pressed = false;
		int button = 0;
		if( state->m_pressedMouseButtons[imp::Event::Mouse_LeftButton] )
		{
			button = 0;
			pressed = true;
		}
		else if( state->m_pressedMouseButtons[imp::Event::Mouse_RightButton] )
		{
			button = 1;
			pressed = true;
		}
	
	
		char key = ' ';
		//bool keyPressed = state->_lastPressedKey != imp::Event::Unknown;
		//if(keyPressed) key = state->_keyValue;
		bool keyReleased = state->_lastReleasedKey != imp::Event::Unknown;
		if(keyReleased) key = state->_keyValue;
	
		bool result = false;
	
		for(unsigned int i=0; i<_eventHandlers.size(); ++i)
		{
			if( mouseMove ) result |= _eventHandlers[i]->onMouseMoved(this, mouseX, mouseY);
			if(mouseOver == true && _previousEventState._mouseOver == false )
				result |= _eventHandlers[i]->onMouseEnter(this, mouseX, mouseY);
			if(mouseOver == false && _previousEventState._mouseOver == true )
				result |= _eventHandlers[i]->onMouseExit(this, mouseX, mouseY);
			if( pressed == true && _previousEventState._pressed == false )
				result |= _eventHandlers[i]->onMousePressed(this, mouseOver, button, mouseX, mouseY);
			if( pressed == false && _previousEventState._pressed == true )
				result |= _eventHandlers[i]->onMouseReleased(this, mouseOver, _previousEventState._button, mouseX, mouseY);
			//if( keyPressed == true ) result |= _eventHandlers[i]->onPressedKey(state->_lastPressedKey, key);
			if( keyReleased == true ) result |= _eventHandlers[i]->onReleasedKey(state->_lastReleasedKey, key);
		}
	
		_previousEventState._mouseX = mouseX;
		_previousEventState._mouseY = mouseY;
		_previousEventState._mouseOver = mouseOver;
		_previousEventState._pressed = pressed;
		if( pressed == true && _previousEventState._pressed == false ) _previousEventState._button = button;
	}
	
	return result;
}

//--------------------------------------------------------------
void GuiComponent::computeBounds(Rectangle& bounds) const
{
	/*for(unsigned int i=0; i<_components.size(); ++i)
	{
		if( _components[i]->isVisible() )
		{
			Rectangle cbounds;
			_components[i]->computeBounds(cbounds);
			bounds.expendBy(cbounds);
		}
	}*/

	bounds = Rectangle(getPositionX(),getPositionY(),getPositionX(),getPositionY()) + bounds;
}

//--------------------------------------------------------------
void GuiComponent::compose(GuiComponent* comp)
{
	_components.push_back(comp);
}

IMPGEARS_END
