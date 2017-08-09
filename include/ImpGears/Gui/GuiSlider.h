#ifndef IMP_GUI_SLIDER_H
#define IMP_GUI_SLIDER_H

#include "Core/impBase.h"
#include "Gui/GuiComponent.h"
#include "Gui/GuiButton.h"

IMPGEARS_BEGIN

//--------------------------------------------------------------
class IMP_API GuiSlider : public GuiComponent
{
	public:

	enum Orientation
	{
		Orientation_Horizontal = 0,
		Orientation_Vertical
	};
	
	GuiSlider();
	virtual ~GuiSlider();
	
	virtual void renderComponent(imp::Uint32 passID, float parentX = 0.0, float parentY = 0.0);
	
	void slideAction(float moving);

	void setOrientation(Orientation ori);

	Orientation getOrientation() {return _orientation;}

	void setRange(float minv, float maxv);
	
	float getValue() const {return _value;}
	
	void resizeBar(float size);
	
	private:
	
	GuiButton* _barButton;
	Layout::Parameters _barParam;
	float _min;
	float _max;
	float _value;
	Orientation _orientation;
	float _barSize;
};

IMPGEARS_END

#endif // IMP_GUI_SLIDER_H
