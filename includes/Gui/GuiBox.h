#ifndef IMP_GUI_BOX_H
#define IMP_GUI_BOX_H

#include "Core/impBase.h"
#include <Gui/GuiContainer.h>
#include <Gui/GuiPanel.h>
#include <Gui/GuiScrollbar.h>
#include <Gui/GuiButton.h>

IMPGEARS_BEGIN

class IMP_API GuiBox
	: public GuiContainer
{
	public:
	
	GuiBox(const std::string& title = "Box");
	
	virtual ~GuiBox();
	
	void setTitle(const std::string& title);
	
	void enableScrollbar(bool enable);
	
	private:
	GuiPanel* _background;
	GuiButton* _header;
	GuiScrollbar* _scrollBar;
	GuiScrollbar* _bottomScrollBar;
};


IMPGEARS_END

#endif // IMP_GUI_BOX_H
