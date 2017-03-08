#include "Gui/GuiEditText.h"

IMPGEARS_BEGIN

//--------------------------------------------------------------
GuiEditText::GuiEditText()
	: GuiComponent(0.0,0.0,0.0,0.0)
	, _background(IMP_NULL)
	, _text(IMP_NULL)
{
	_background = new GuiPanel();
	_background->setBackgroundColor( imp::Vector3(0.0,0.0,0.3) );
	Layout::Parameters backLayout ;
	backLayout.setResizingX(Layout::Resizing_Fill);
	backLayout.setResizingY(Layout::Resizing_Fill);
	_background->setLayoutParameters(backLayout);
	compose(_background);

	_text = new GuiText();
	Layout::Parameters param;
	param.setAlignementX(Layout::Alignement_Center);
	param.setAlignementY(Layout::Alignement_Center);
	_text->setLayoutParameters(param);
	compose(_text);

	setSize( 100.0, 30.0 );

	class GuiTextBehaviour : public GuiEventHandler
	{
		public:
		GuiTextBehaviour(GuiEditText* itext)
			: _itext(itext)
		{}
		~GuiTextBehaviour(){}
		
		bool onMousePressed(GuiEventSource* component, bool over, int buttonID, float x, float y)
		{
			_itext->setFocused(over);
			return false;
		}
		
		bool onReleasedKey(imp::Event::Key key, char value)
		{
			if(_itext->hasFocus())
			{
				std::string text = _itext->getEditText();
				if(key == imp::Event::BackSpace)
				{
					if(text.size() > 0)
					{
						text.resize(text.size()-1);
						_itext->setText(text);
					}
				}
				else if(key != imp::Event::Unknown && key != imp::Event::LShift && key != imp::Event::RShift)
				{
					text.push_back(value);
					_itext->setText(text);
				}
			}
			return false;
		}
		
		protected:
		GuiEditText* _itext;
	};
	
	setText("editText 1");
	addEventHandler(new GuiTextBehaviour(this));
}

//--------------------------------------------------------------
GuiEditText::~GuiEditText()
{
}

//--------------------------------------------------------------
void GuiEditText::setText(const std::string& text)
{
	_editText = text;
	
	if(_focused)
		_text->setText(text + " |");
	else
		_text->setText(text + " |");
}

//--------------------------------------------------------------
void GuiEditText::setFocused(bool focused)
{
	_focused = focused;
	
	setText(_editText);
}

IMPGEARS_END
