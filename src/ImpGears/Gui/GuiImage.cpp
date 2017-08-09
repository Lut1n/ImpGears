#include "Gui/GuiImage.h"
#include <SceneGraph/BmpLoader.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
GuiImage::GuiImage()
: GuiComponent(0.0,0.0,0.0,0.0)
{
	_texture = new Texture();
}

//--------------------------------------------------------------
GuiImage::~GuiImage()
{
	if(_texture)
		delete _texture;
}

//--------------------------------------------------------------
void GuiImage::setImage(const std::string& filename)
{
	if(_texture)
		delete _texture;
	_texture = BmpLoader::loadFromFile(filename.c_str());
}

//--------------------------------------------------------------
void GuiImage::setImage(ImageData* image)
{
	_texture->loadFromImageData(image);
	_texture->synchronize();
}

//--------------------------------------------------------------
void GuiImage::renderComponent(imp::Uint32 passID, float px, float py)
{
	GuiComponent::_guiComponentShader->setTextureParameter("u_texture", _texture, 0);
	GuiComponent::_guiComponentShader->setVector3Parameter( "u_color", Vector3(1.0,1.0,1.0) );
	GuiComponent::_guiComponentShader->setFloatParameter("u_opacity", 1.0);
	GuiComponent::_guiComponentShader->setFloatParameter( "u_type", 0.0 );
	GuiComponent::_guiComponentShader->setFloatParameter("u_sizeX", getSizeX());
	GuiComponent::_guiComponentShader->setFloatParameter("u_sizeY", getSizeY());
	_quad.draw();
	GuiComponent::renderComponent(passID, px, py);
}

IMPGEARS_END
