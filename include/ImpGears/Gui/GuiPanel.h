#ifndef IMP_GUI_PANEL_H
#define IMP_GUI_PANEL_H

#include "Core/impBase.h"
#include "Gui/GuiComponent.h"

IMPGEARS_BEGIN

//--------------------------------------------------------------
class IMP_API GuiPanel : public GuiComponent
{
	public:

	GuiPanel();
	virtual ~GuiPanel();
	
	virtual void renderComponent(imp::Uint32 passID, float parentX = 0.0, float parentY = 0.0);
	
	void setBackgroundColor(const Vector3& bgColor);
	
	private:
	
	Vector3 _backgroundColor;
};

IMPGEARS_END

#endif // IMP_GUI_PANEL_H
