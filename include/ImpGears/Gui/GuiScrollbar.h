#ifndef IMP_GUI_SCROLLBAR_H
#define IMP_GUI_SCROLLBAR_H

#include "Core/impBase.h"
#include "Gui/GuiSlider.h"
#include "Gui/GuiButton.h"
#include "Gui/GuiContainer.h"

IMPGEARS_BEGIN

//--------------------------------------------------------------
class IMP_API GuiScrollbar : public GuiSlider
{
	public:
	
	GuiScrollbar(GuiContainer* box, GuiSlider::Orientation orientation = GuiSlider::Orientation_Vertical);
	virtual ~GuiScrollbar();
	
	GuiContainer* getAssociatedComponent() {return _box;}
	
	virtual void renderComponent(imp::Uint32 passID, float parentX = 0.0, float parentY = 0.0);
	
	float getNormalizedSize();
	void setContainedPosition(float position);
	
	void updateFieldView();
	
	private:
	GuiContainer* _box;
	
	float _windowSize;
	float _windowPosition;
	float _containedSize;
};

IMPGEARS_END

#endif // IMP_GUI_SCROLLBAR_H
