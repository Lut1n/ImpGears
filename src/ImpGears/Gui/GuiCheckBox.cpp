#include <Gui/GuiCheckBox.h>

IMPGEARS_BEGIN


//--------------------------------------------------------------
GuiCheckBox::GuiCheckBox()
	: GuiComponent(0.0,0.0,0.0,0.0)
	, _checked(false)
	, check(IMP_NULL)
	, rightComponent(IMP_NULL)
{
	check = new GuiButton();
	rightComponent = new GuiText();
	rightComponent->setText("checkbox");

	compose(check);
	compose(rightComponent);

	class CheckBoxBehaviour : public GuiEventHandler
	{
		public:
		CheckBoxBehaviour(GuiCheckBox* check)
			: _check(check)
			, _pressed(false)
		{}
		~CheckBoxBehaviour(){}
		
		bool onMousePressed(GuiEventSource* component, bool over, int buttonID, float x, float y, bool action)
		{
			if(over)
			{
				_pressed = true;
				return true;
			}
			return false;
		}
		
		bool onMouseReleased(GuiEventSource* component, bool over, int buttonID, float x, float y, bool action)
		{
			if(over && _pressed)
			{
				_check->setChecked( !_check->isChecked() );
			}
			_pressed = false;
			return false;
		}

		private:
		GuiCheckBox* _check;
		bool _pressed;
	};

	addEventHandler(new CheckBoxBehaviour(this));
}

//--------------------------------------------------------------
GuiCheckBox::~GuiCheckBox()
{
}

//--------------------------------------------------------------
void GuiCheckBox::renderComponent(float parentX, float parentY)
{	
	if(isChecked())
	{
		check->getBackground()->setBackgroundColor(Vector3(0.0,0.0,1.0));
	}
	else
	{
		check->getBackground()->setBackgroundColor(Vector3(1.0,1.0,1.0));
	}

	GuiComponent::renderComponent(parentX, parentY);
	
	float h = rightComponent->getSizeY();
	check->setSize(h,h);
	rightComponent->setPosition(h*1.5, 0.0);

	setSize(h*1.5 + rightComponent->getSizeX(), h);
}

IMPGEARS_END
