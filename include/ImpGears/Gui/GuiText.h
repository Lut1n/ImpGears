#ifndef IMP_GUI_TEXT_H
#define IMP_GUI_TEXT_H

#include "Core/impBase.h"
#include "Gui/GuiComponent.h"

IMPGEARS_BEGIN

//--------------------------------------------------------------
class IMP_API GuiText : public GuiComponent
{
	public:

	GuiText();
	virtual ~GuiText();
	
	virtual void renderComponent(imp::Uint32 passID, float parentX = 0.0, float parentY = 0.0);
	
	
	void setText(const std::string& text);
	void setTextSize(float size);
	void setTextColor(const Vector3& color) { _textColor = color; }
	
	protected:

	Vector3 _textColor;
	std::string _text;
	ImageData _textImage;
	imp::Texture* _texture;
	float _textSize;
	bool _textDirty;
};

IMPGEARS_END

#endif // IMP_GUI_TEXT_H
