#include <Gui/GuiBox.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
GuiBox::GuiBox(const std::string& title)
	: _scrollBar(IMP_NULL)
{
	class BoxHeaderBehaviour : public GuiEventHandler
	{
		public:
		BoxHeaderBehaviour(GuiBox* box)
			: _box(box)
			, _startX(0.0)
			, _startY(0.0)
			, _oldPositionX(0.0)
			, _oldPositionY(0.0)
			, _drag(false)
		{
		}
		~BoxHeaderBehaviour(){}
		
		bool onMousePressed(GuiEventSource* component, bool over, int buttonID, float x, float y)
		{
			if(over)
			{
				_drag = true;
				_startX = x;
				_startY = y;
				_oldPositionX = _box->getPositionX();
				_oldPositionY = _box->getPositionY();
			}
			return false;
		}
		
		bool onMouseReleased(GuiEventSource* component, bool over, int buttonID, float x, float y)
		{
			_drag = false;
			return false;
		}
		
		bool onMouseMoved(GuiEventSource* component, float x, float y)
		{
			if(_drag)
				_box->setPosition((_oldPositionX + (x - _startX)), (_oldPositionY + (y - _startY)) );
			return false;
		}
		
		protected:
		
		GuiBox* _box;
		float _startX;
		float _startY;
		float _oldPositionX;
		float _oldPositionY;
		bool _drag;
	};
	
	_background = new GuiPanel();
	_background->setBackgroundColor(imp::Vector3(0.0,0.0,0.1));
	Layout::Parameters backParam;
	backParam.setResizingX(Layout::Resizing::Resizing_Fill);
	backParam.setResizingY(Layout::Resizing::Resizing_Fill);
	_background->setLayoutParameters(backParam);
	compose(_background);
	
	_header = new GuiButton();
	_header->setPosition(0.0, -_header->getSizeY());
	Layout::Parameters headerParam;
	headerParam.setResizingX(Layout::Resizing_Fill);
	_header->setLayoutParameters(headerParam);

	_header->addEventHandler(new BoxHeaderBehaviour(this));
	_header->setText("Click and drag");
	
	compose(_header);
	
	setTitle(title);
}

//--------------------------------------------------------------
GuiBox::~GuiBox()
{
	delete _header;
}

//--------------------------------------------------------------
void GuiBox::setTitle(const std::string& title)
{
	_header->setText(title);
}

//--------------------------------------------------------------
void GuiBox::enableScrollbar(bool enable)
{
	if(_scrollBar == IMP_NULL && enable)
	{
		Layout::Parameters param;
		param.setAlignementX(Layout::Alignement_End);
		param.setResizingY(Layout::Resizing_Fill);
		_scrollBar = new GuiScrollbar(this);
		_scrollBar->setLayoutParameters(param);
		compose(_scrollBar);
	}
	else if(_scrollBar != IMP_NULL && !enable)
	{
		// decompose(&_scrollBar);
	}
}

IMPGEARS_END
