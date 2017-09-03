#include <Gui/GuiComboBox.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
GuiComboBox::GuiComboBox()
	: GuiComponent(0.0,0.0,0.0,0.0)
{
	_selected = new GuiText();
	_openButton = new GuiButton();
	_list = new GuiList();
	_list->setVisible(false);

	//_openButton->setText("openList");
	_selected->setText("comboBox1");
	compose(_openButton);
	compose(_selected);
	compose(_list);

	class ComboBoxOpen : public GuiEventHandler
	{
		public:
		ComboBoxOpen(GuiComboBox* comboBox)
			: _pressed(false)
		{
			_comboBox = comboBox;
		}

		~ComboBoxOpen()
		{
		}
		
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
				_comboBox->openList();
			}
			_pressed = false;
			return false;
		}

		private:
		bool _pressed;
		GuiComboBox* _comboBox;
	};

	_openButton->addEventHandler(new ComboBoxOpen(this));
}

//--------------------------------------------------------------
GuiComboBox::~GuiComboBox()
{
}

//--------------------------------------------------------------
void GuiComboBox::renderComponent(float parentX, float parentY)
{
	_openButton->setSize( _selected->getSizeY(), _selected->getSizeY() );
	_selected->setPosition(_selected->getSizeY(), 0.0);
	_list->setPosition(_selected->getSizeY(), _selected->getSizeY() );
	_list->setSize(_selected->getSizeX(), _selected->getSizeY() * 5.0);

	GuiComponent::renderComponent(parentX, parentY);
}

//--------------------------------------------------------------
void GuiComboBox::select(unsigned int selectedID)
{
	closeList();

	if(selectedID < _strList.size())
	{
		std::string str = _strList[selectedID];
		_selected->setText( str );
	}
}

//--------------------------------------------------------------
void GuiComboBox::openList()
{
	_list->setVisible(true);
}

//--------------------------------------------------------------
void GuiComboBox::closeList()
{
	_list->setVisible(false);
}

//--------------------------------------------------------------
void GuiComboBox::addList(const char* text)
{
	unsigned int itemIndex = _strList.size();
	_strList.push_back(text);

	GuiText* ctext = new GuiText();
	ctext->setText(text);
	
	Layout::Parameters param;
	param.setPositionningY(Layout::Positionning_Auto);
	ctext->setLayoutParameters(param);
	_list->addComponent( ctext );

	class ItemBehaviour : public GuiEventHandler
	{
		public:
		ItemBehaviour(GuiComboBox* comboBox, unsigned int index)
			: _pressed(false)
		{
			_comboBox = comboBox;
			_index = index;
		}

		~ItemBehaviour()
		{
		}
		
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
				_comboBox->select(_index);
			}
			_pressed = false;
			return false;
		}

		private:
		bool			_pressed;
		GuiComboBox*		_comboBox;
		unsigned int	_index;
	};

	ctext->addEventHandler( new ItemBehaviour(this, itemIndex) );
}

IMPGEARS_END
