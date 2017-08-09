#ifndef IMP_GUI_BUTTON_H
#define IMP_GUI_BUTTON_H

#include "Core/impBase.h"
#include "Gui/GuiPanel.h"
#include "Gui/GuiComponent.h"

IMPGEARS_BEGIN

//--------------------------------------------------------------
class IMP_API GuiButton : public GuiComponent
{
	public:

	GuiButton();
	virtual ~GuiButton();
	
	void setText(const std::string& text);
	void setIcon(const std::string& iconfn);

	void setContent(GuiComponent* content);

	GuiPanel* getBackground() {return _background;}
	
	private:
	
	GuiPanel* _background;
	GuiComponent* _content;
};

IMPGEARS_END

#endif // IMP_GUI_BUTTON_H
