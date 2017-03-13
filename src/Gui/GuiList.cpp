#include <Gui/GuiList.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
GuiList::GuiList()
{
	Layout::Parameters param;
	param.setAlignementX(Layout::Alignement_End);
	param.setResizingY(Layout::Resizing_Fill);
	GuiScrollbar* scroll = new GuiScrollbar(this);
	scroll->setLayoutParameters(param);
	compose( scroll );
}

//--------------------------------------------------------------
GuiList::~GuiList()
{
}

IMPGEARS_END
