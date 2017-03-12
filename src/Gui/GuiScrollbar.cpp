#include "Gui/GuiScrollbar.h"

IMPGEARS_BEGIN

//--------------------------------------------------------------
GuiScrollbar::GuiScrollbar(GuiContainer* box, GuiSlider::Orientation orientation)
	: GuiSlider()
	, _box(box)
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
	
	return (bounds.getY2());
}

//--------------------------------------------------------------
void GuiScrollbar::setContainedPosition(float position)
{
	float containedSize = getNormalizedSize() - _box->getSizeY();
	if(containedSize > 0.0)
		_box->setContentPosition( _box->getContentPositionX(), -position * containedSize );
	else
		_box->setContentPosition( _box->getContentPositionX(), 0.0 );
}

//--------------------------------------------------------------
void GuiScrollbar::updateFieldView()
{
	float containedSize = getNormalizedSize();
	if(containedSize > 0.0)
	{
		float rel = _box->getSizeY()/containedSize;
		if(rel < 0.1) rel = 0.1;
		else if(rel > 1.0) rel = 1.0;
		float barSize = rel * getSizeY();
		resizeBar(barSize);
	}
	else
	{
		resizeBar( getSizeY() );
	}
	
	float value = getValue()/100.0;
	if(value < 0.0) value = 0.0;
	else if(value > 1.0) value = 1.0;
	setContainedPosition( value );
}

//--------------------------------------------------------------
void GuiScrollbar::renderComponent(imp::Uint32 passID, float parentX, float parentY)
{
	updateFieldView();
	
	GuiSlider::renderComponent(passID, parentX, parentY);
}


IMPGEARS_END
