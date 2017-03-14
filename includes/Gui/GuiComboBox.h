#ifndef IMP_GUI_COMBOBOX_H
#define IMP_GUI_COMBOBOX_H

#include "Core/impBase.h"
#include <Gui/GuiComponent.h>
#include <Gui/GuiList.h>
#include <Gui/GuiText.h>
#include <Gui/GuiButton.h>

#include <vector>

IMPGEARS_BEGIN

class IMP_API GuiComboBox
	: public GuiComponent
{
	public:
	GuiComboBox();
	virtual ~GuiComboBox();

	virtual void renderComponent(imp::Uint32 passID, float parentX = 0.0, float parentY = 0.0);

	void select(unsigned int selectedID);
	void openList();
	void closeList();

	void addList(const char* text);

	private:

	std::vector<std::string> _strList;

	GuiText* _selected;
	GuiButton* _openButton;
	GuiList* _list;
};


IMPGEARS_END

#endif // IMP_GUI_COMBOBOX_H
