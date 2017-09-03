#ifndef IMP_GUI_CHECKBOX_H
#define IMP_GUI_CHECKBOX_H

#include "Core/impBase.h"
#include <Gui/GuiButton.h>
#include <Gui/GuiText.h>

IMPGEARS_BEGIN

class IMP_API GuiCheckBox
	: public GuiComponent
{
	public:
	GuiCheckBox();
	virtual ~GuiCheckBox();

	virtual void renderComponent(float parentX = 0.0, float parentY = 0.0);

	void setChecked(bool checked){_checked = checked;}
	bool isChecked() const {return _checked;}

	private:
	bool _checked;

	GuiButton* check;
	GuiText* rightComponent;
};

IMPGEARS_END

#endif // IMP_GUI_CHECKBOX_H
