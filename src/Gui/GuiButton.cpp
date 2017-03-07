#include "Gui/GuiButton.h"
#include "Gui/GuiText.h"
#include "Gui/GuiImage.h"

IMPGEARS_BEGIN

//--------------------------------------------------------------
GuiButton::GuiButton()
	: GuiComponent(0.0,0.0,0.0,0.0)
	, _background(IMP_NULL)
	, _content(IMP_NULL)
{
	class ButtonBehaviour : public GuiEventHandler
	{
		public:
		ButtonBehaviour(GuiButton* button)
			: _button(button)
			, _defaultColor(0.0,0.0,0.3)
			, _overColor(0.0,0.0,0.7)
			, _pressedColor(0.3,0.3,0.7)
			, _pressed(false)
		{}
		~ButtonBehaviour(){}
		
		bool onMousePressed(GuiEventSource* component, bool over, int buttonID, float x, float y)
		{
			if(over)
			{
				_pressed = true;
				_button->getBackground()->setBackgroundColor(_pressedColor);
				return true;
			}
			return false;
		}
		
		bool onMouseReleased(GuiEventSource* component, bool over, int buttonID, float x, float y)
		{
			_pressed = false;
			if(over)
				_button->getBackground()->setBackgroundColor(_overColor);
			else
				_button->getBackground()->setBackgroundColor(_defaultColor);
			return false;
		}
		
		bool onMouseMoved(GuiEventSource* component, float x, float y){return false;}
		
		bool onMouseEnter(GuiEventSource* component, float x, float y)
		{
			if(!_pressed)
				_button->getBackground()->setBackgroundColor(_overColor);
			return false;
		}
		
		bool onMouseExit(GuiEventSource* component, float x, float y)
		{
			if(!_pressed)
				_button->getBackground()->setBackgroundColor(_defaultColor);
			return false;
		}
		
		protected:
		
		GuiButton* _button;
		Vector3 _defaultColor;
		Vector3 _overColor;
		Vector3 _pressedColor;
		bool _pressed;
	};
	
	_background = new GuiPanel();
	Layout::Parameters backLayout ;
	backLayout.setResizingX(Layout::Resizing_Fill);
	backLayout.setResizingY(Layout::Resizing_Fill);
	_background->setLayoutParameters(backLayout);
	compose(_background);

	_background->setBackgroundColor( imp::Vector3(0.0,0.0,0.3) );
	addEventHandler(new ButtonBehaviour(this));

	setSize( 100.0, 30.0 );
}

//--------------------------------------------------------------
GuiButton::~GuiButton()
{
}

//--------------------------------------------------------------
void GuiButton::setText(const std::string& text)
{
	GuiText* textComp = new GuiText();
	textComp->setText(text);

	setContent(textComp);
}

//--------------------------------------------------------------
void GuiButton::setIcon(const std::string& iconfn)
{
	GuiImage* imgComp = new GuiImage();
	imgComp->setImage(iconfn);

	setContent(imgComp);
}

//--------------------------------------------------------------
void GuiButton::setContent(GuiComponent* content)
{
	if(_content)
	{
		decompose(&_content);
	}

	_content = content;
	Layout::Parameters param;
	param.setAlignementX(Layout::Alignement_Center);
	param.setAlignementY(Layout::Alignement_Center);

	_content->setLayoutParameters(param);

	compose(_content);
}

IMPGEARS_END
