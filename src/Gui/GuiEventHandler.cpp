#include "Gui/GuiEventHandler.h"
#include "Core/impBase.h"

IMPGEARS_BEGIN

//--------------------------------------------------------------
GuiEventHandler::GuiEventHandler()
{
}

//--------------------------------------------------------------
GuiEventHandler::~GuiEventHandler()
{
}

//--------------------------------------------------------------
bool GuiEventHandler::onMousePressed(GuiEventSource* evnSrc, bool over, int buttonID, float x, float y)
{
	return false;
}

//--------------------------------------------------------------
bool GuiEventHandler::onMouseReleased(GuiEventSource* evnSrc, bool over, int buttonID, float x, float y)
{
	return false;
}

//--------------------------------------------------------------
bool GuiEventHandler::onMouseMoved(GuiEventSource* evnSrc, float x, float y)
{
	return false;
}

//--------------------------------------------------------------
bool GuiEventHandler::onMouseEnter(GuiEventSource* evnSrc, float x, float y)
{
	return false;
}

//--------------------------------------------------------------
bool GuiEventHandler::onMouseExit(GuiEventSource* evnSrc, float x, float y)
{
	return false;
}

//--------------------------------------------------------------
bool GuiEventHandler::onPressedKey(imp::Event::Key, char v)
{
	return false;
}

//--------------------------------------------------------------
bool GuiEventHandler::onReleasedKey(imp::Event::Key, char v)
{
	return false;
}


IMPGEARS_END
