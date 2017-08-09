#ifndef IMP_GUI_EVENT_H
#define IMP_GUI_EVENT_H

#include "Core/impBase.h"
#include <Utils/EvnContextInterface.h>

IMPGEARS_BEGIN


class IMP_API GuiEventSource
{
	public:
	
	GuiEventSource()
	{}

	virtual ~GuiEventSource()
	{}
};

class IMP_API GuiEventHandler
{
	public:
	
	GuiEventHandler();
	virtual ~GuiEventHandler();
	
	virtual bool onMousePressed(GuiEventSource* evnSrc, bool over, int buttonID, float x, float y, bool action = false);
	
	virtual bool onMouseReleased(GuiEventSource* evnSrc, bool over, int buttonID, float x, float y, bool action = false);
	
	virtual bool onMouseMoved(GuiEventSource* evnSrc, float x, float y);
	
	virtual bool onMouseEnter(GuiEventSource* evnSrc, float x, float y);
	
	virtual bool onMouseExit(GuiEventSource* evnSrc, float x, float y);
	
	virtual bool onPressedKey(imp::Event::Key, char v);
	
	virtual bool onReleasedKey(imp::Event::Key, char v);
};


IMPGEARS_END

#endif // IMP_GUI_EVENT_H
