#ifndef IMP_GUI_CONTAINER_H
#define IMP_GUI_CONTAINER_H

#include "Core/impBase.h"
#include <Gui/GuiComponent.h>
#include <Gui/GuiPanel.h>

IMPGEARS_BEGIN

class IMP_API GuiContainer
	: public GuiComponent
{
	public:

	GuiContainer();
	virtual ~GuiContainer();

	void addComponent(GuiComponent* comp);
	void removeComponent(GuiComponent* comp);

	virtual void renderComponent(imp::Uint32 passID, float parentX = 0.0, float parentY = 0.0);
	virtual bool event(imp::State* state, float x = 0.0, float y = 0.0);

	void setContentPosition(float x, float y);
	float getContentPositionX() const;
	float getContentPositionY() const;
	
	virtual void computeContentBounds(Rectangle& bounds) const;
	

	protected:

	GuiPanel* _background;
	GuiComponent* _content;
};


IMPGEARS_END

#endif // IMP_GUI_CONTAINER_H
