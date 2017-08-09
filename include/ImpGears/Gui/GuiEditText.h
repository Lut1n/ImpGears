#ifndef IMP_GUI_EDITTEXT_H
#define IMP_GUI_EDITTEXT_H

#include "Core/impBase.h"
#include "Gui/GuiPanel.h"
#include "Gui/GuiText.h"
#include "Gui/GuiComponent.h"

IMPGEARS_BEGIN

//--------------------------------------------------------------
class IMP_API GuiEditText : public GuiComponent
{
	public:

	GuiEditText();
	virtual ~GuiEditText();
	
	void setText(const std::string& text);

	GuiPanel* getBackground() {return _background;}

	const std::string& getEditText() const {return _editText;}
	
	void setFocused(bool focused);
	bool hasFocus() const {return _focused;}
	
	private:
	
	GuiPanel* _background;
	GuiText* _text;
	bool _focused;
	std::string _editText;
};

IMPGEARS_END

#endif // IMP_GUI_EDITTEXT_H
