#include "Gui/GuiScrollbar.h"

IMPGEARS_BEGIN

//--------------------------------------------------------------
GuiScrollbar::GuiScrollbar(GuiContainer* box, GuiSlider::Orientation orientation)
	: GuiSlider()
	, _box(box)
	, _windowSize(0.0)
	, _windowPosition(0.0)
	, _containedSize(0.0)
{
	setOrientation(orientation);
	updateFieldView();
}

//--------------------------------------------------------------
GuiScrollbar::~GuiScrollbar()
{
}

//--------------------------------------------------------------
float GuiScrollbar::getNormalizedSize()
{
	Rectangle bounds;
	_box->computeContentBounds(bounds);
	
	Rectangle boxSize(0.0,0.0,_box->getSizeX(), _box->getSizeY());
	bounds.expendBy(boxSize);
	
	if(getOrientation() == Orientation_Vertical)
		return (bounds.getY2() - bounds.getY1());
	else
		return (bounds.getX2() - bounds.getX1());
}

//--------------------------------------------------------------
void GuiScrollbar::setContainedPosition(float position)
{
	_containedSize = getNormalizedSize();
	_windowSize = (getOrientation() == Orientation_Vertical) ? _box->getSizeY() : _box->getSizeX();
	
	float movingRange =_containedSize - _windowSize; // course
	if(movingRange > 0.0)
	{
		if(getOrientation() == Orientation_Vertical)
			_box->setContentPosition( _box->getContentPositionX(), -position * movingRange );
		else
			_box->setContentPosition( -position * movingRange, _box->getContentPositionY() );
	}
	else
	{
		if(getOrientation() == Orientation_Vertical)
			_box->setContentPosition( _box->getContentPositionX(), 0.0 );
		else
			_box->setContentPosition( 0.0, _box->getContentPositionY() );
	}
}

//--------------------------------------------------------------
void GuiScrollbar::updateFieldView()
{
	_windowSize = (getOrientation() == Orientation_Vertical) ? _box->getSizeY() : _box->getSizeX();
	float sliderSize = (getOrientation() == Orientation_Vertical) ? getSizeY() : getSizeX();
	
	_containedSize = getNormalizedSize();
	
	// update bar size
	float rel = _windowSize/_containedSize;
	if(rel < 0.1) rel = 0.1;else if(rel > 1.0) rel = 1.0;
	resizeBar( rel * sliderSize );
	
	// update bar pos
	float value = getValue()/100.0;
	if(value < 0.0) value = 0.0;else if(value > 1.0) value = 1.0;
	setContainedPosition( value );
}

//--------------------------------------------------------------
void GuiScrollbar::renderComponent(float parentX, float parentY)
{
	updateFieldView();
	
	GuiSlider::renderComponent(parentX, parentY);
}


IMPGEARS_END
