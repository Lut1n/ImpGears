#include "Gui/GuiText.h"
#include "Graphics/TextRenderer.h"

IMPGEARS_BEGIN

//--------------------------------------------------------------
GuiText::GuiText()
: GuiComponent(0.0, 0.0, 0.0,0.0)
, _textColor(1.0,1.0,1.0)
, _textDirty(true)
{
	setTextSize( TextRenderer::getInstance()->getDefaultSize() );
	_texture = new Texture();
	setText("text1");
}

//--------------------------------------------------------------
GuiText::~GuiText()
{
}

//--------------------------------------------------------------
void GuiText::renderComponent(imp::Uint32 passID, float parentX, float parentY)
{
	if(_textDirty)
	{
		Pixel useless;
		float scale = _textSize/TextRenderer::getInstance()->getDefaultSize();
		
		TextRenderer::getInstance()->renderText(_text, &_textImage, useless, useless);
		_texture->loadFromImageData(&_textImage);
		_texture->synchronize();

		setSize(_textImage.getWidth()*scale, _textImage.getHeight()*scale);
		
		_textDirty = false;
	}

	GuiComponent::_guiComponentShader->setTextureParameter( "u_texture", _texture, 0 );
	GuiComponent::_guiComponentShader->setVector3Parameter( "u_color", _textColor );
	GuiComponent::_guiComponentShader->setFloatParameter("u_opacity", 1.0);
	GuiComponent::_guiComponentShader->setFloatParameter( "u_type", 0.0 );
	GuiComponent::_guiComponentShader->setFloatParameter("u_sizeX", getSizeX());
	GuiComponent::_guiComponentShader->setFloatParameter("u_sizeY", getSizeY());

	_quad.draw();
	GuiComponent::renderComponent(passID, parentX, parentY);
}

//--------------------------------------------------------------
void GuiText::setText(const std::string& text)
{
	this->_text = text;
	_textDirty = true;
}

//--------------------------------------------------------------
void GuiText::setTextSize(float size)
{
	_textSize = size;
	_textDirty = true;
}


IMPGEARS_END
