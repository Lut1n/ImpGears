#ifndef IMP_GUI_LIST_H
#define IMP_GUI_LIST_H

#include "Core/impBase.h"
#include <Gui/GuiScrollbar.h>
#include <Gui/GuiContainer.h>

IMPGEARS_BEGIN

class IMP_API GuiList
	: public GuiContainer
{
	public:
	
	GuiList();	
	virtual ~GuiList();

	private:
	GuiScrollbar* _scrollBar;
};


IMPGEARS_END

#endif // IMP_GUI_LIST_H
