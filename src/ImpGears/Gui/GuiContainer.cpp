#include <Gui/GuiContainer.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
GuiContainer::GuiContainer()
	: GuiComponent(0.0,0.0,100.0,100.0)
	, _background(IMP_NULL)
	, _content(IMP_NULL)
{
	_background = new GuiPanel();
	Layout::Parameters bgParam;
	bgParam.setResizingX(Layout::Resizing_Fill);
	bgParam.setResizingY(Layout::Resizing_Fill);
	_background->setLayoutParameters(bgParam);
	compose(_background);

	_content = new GuiComponent(0.0, 0.0, 100.0, 100.0);
	_content->setLayoutParameters(bgParam);
}

//--------------------------------------------------------------
GuiContainer::~GuiContainer()
{
	delete _content;
}

//--------------------------------------------------------------
void GuiContainer::renderComponent(float parentX, float parentY)
{
	
	if(_visible == false) return;

	GuiComponent::renderComponent(parentX, parentY);

	// apply render on content
	Rectangle clippingRect = Rectangle(
									parentX+getPositionX(),
									parentY+getPositionY(),
									parentX+getPositionX()+getSizeX(),
									parentY+getPositionY()+getSizeY());
	clippingRect.intersect( getClippingRect() );

	_content->setClippingRect( clippingRect );

	Rectangle compsurface(
		_content->getPositionX(),
		_content->getPositionY(),
		_content->getPositionX()+_content->getSizeX(),
		_content->getPositionY()+_content->getSizeY() );

	_layout.applyParameters(_content->getLayoutParameters(), compsurface);
	_content->setPosition(compsurface.getX1(), compsurface.getY1());
	_content->setSize(compsurface.getX2() - compsurface.getX1(), compsurface.getY2() - compsurface.getY1());
	
	if( _content->isVisible() )
	{
		_content->renderComponent(parentX+getPositionX(), parentY+getPositionY());
	}
}

//--------------------------------------------------------------
bool GuiContainer::event(imp::State* state, float x, float y)
{
	bool result = GuiComponent::event(state,x,y);

	// event on content
	Rectangle compsurface(
		_content->getPositionX(),
		_content->getPositionY(),
		_content->getPositionX()+_content->getSizeX(),
		_content->getPositionY()+_content->getSizeY() );

	_layout.applyParameters(_content->getLayoutParameters(), compsurface);
	_content->setPosition(compsurface.getX1(), compsurface.getY1());
	_content->setSize(compsurface.getX2() - compsurface.getX1(), compsurface.getY2() - compsurface.getY1());
	
	if( result == false && _content->isVisible() )
	{
		result |= _content->event(state, x+getPositionX(), y+getPositionY());
	}

	return result;
}

//--------------------------------------------------------------
void GuiContainer::setContentPosition(float x, float y)
{
	_content->setPosition(x,y);
}

//--------------------------------------------------------------
float GuiContainer::getContentPositionX() const
{
	return _content->getPositionX();
}

//--------------------------------------------------------------
float GuiContainer::getContentPositionY() const
{
	return _content->getPositionY();
}

//--------------------------------------------------------------
void GuiContainer::computeContentBounds(Rectangle& bounds) const
{
	_content->computeBounds(bounds);
}

//--------------------------------------------------------------
void GuiContainer::addComponent(GuiComponent* comp)
{
	_content->compose(comp);
}

//--------------------------------------------------------------
void GuiContainer::removeComponent(GuiComponent* comp)
{
	_content->decompose(&comp);
}

IMPGEARS_END
