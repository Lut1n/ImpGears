#ifndef IMP_GUI_IMAGE_H
#define IMP_GUI_IMAGE_H

#include "Core/impBase.h"
#include "Gui/GuiComponent.h"

IMPGEARS_BEGIN

//--------------------------------------------------------------
class IMP_API GuiImage : public GuiComponent
{
	public:

	GuiImage();
	virtual ~GuiImage();

	virtual void renderComponent(float px = 0.0, float py = 0.0);
	
	
	void setImage(const std::string& filename);
	void setImage(ImageData* image);

	private:

	Texture* _texture;
};

IMPGEARS_END

#endif // IMP_GUI_IMAGE_H
