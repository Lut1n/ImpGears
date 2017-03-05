#include "Gui/GuiPanel.h"

IMPGEARS_BEGIN

//--------------------------------------------------------------
GuiPanel::GuiPanel()
 : GuiComponent(0.0,0.0,0.0,0.0)
{
}

//--------------------------------------------------------------
GuiPanel::~GuiPanel()
{
}

//--------------------------------------------------------------
void GuiPanel::renderComponent(imp::Uint32 passID, float parentX, float parentY)
{
	GuiComponent::_guiComponentShader->setVector3Parameter( "u_color", _backgroundColor );
	GuiComponent::_guiComponentShader->setFloatParameter( "u_type", 1.0 );
	GuiComponent::_guiComponentShader->setFloatParameter("u_opacity", 0.7);
	GuiComponent::_guiComponentShader->setFloatParameter("u_sizeX", getSizeX());
	GuiComponent::_guiComponentShader->setFloatParameter("u_sizeY", getSizeY());

	_quad.draw();
	GuiComponent::renderComponent(passID, parentX, parentY);
}

//--------------------------------------------------------------
void GuiPanel::setBackgroundColor(const Vector3& bgColor)
{
	_backgroundColor = bgColor;
}


IMPGEARS_END
