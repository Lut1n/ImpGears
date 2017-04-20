#include <Gui/GuiBox.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
GuiBox::GuiBox(const std::string& title)
	: _scrollBar(IMP_NULL)
	, _bottomScrollBar(IMP_NULL)
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
			//, _over(false)
		{
		}
		~BoxHeaderBehaviour(){}
		
		bool onMousePressed(GuiEventSource* component, bool over, int buttonID, float x, float y, bool action)
		{
			if(over && action)
			{
				_drag = true;
				_startX = x;
				_startY = y;
				_oldPositionX = _box->getPositionX();
				_oldPositionY = _box->getPositionY();
				return true;
			}
			return false;
		}
		
		bool onMouseReleased(GuiEventSource* component, bool over, int buttonID, float x, float y, bool action)
		{
			_drag = false;
			return false;
		}
		
		bool onMouseMoved(GuiEventSource* component, float x, float y)
		{
			if(x>0.0 && y>0.0 && _drag)
			{
				float newPosX = (_oldPositionX + (x - _startX));
				float newPosY = (_oldPositionY + (y - _startY)) ;
				_box->setPosition(newPosX, newPosY);
			}
			return false;
		}
/*
		bool onMouseEnter(GuiEventSource* evnSrc, float x, float y)
		{
			_over = true;
			return false;
		}

		bool onMouseExit(GuiEventSource* evnSrc, float x, float y)
		{
			_over = false;
			return false;
		}*/
		
		protected:
		
		GuiBox* _box;
		float _startX;
		float _startY;
		float _oldPositionX;
		float _oldPositionY;
		bool _drag;
		//bool _over;
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
		
		Layout::Parameters param2;
		param2.setAlignementY(Layout::Alignement_End);
		param2.setResizingX(Layout::Resizing_Fill);
		//_bottomScrollBar = new GuiScrollbar(this);
		GuiScrollbar* _bottomScrollBar = new GuiScrollbar(this);
		_bottomScrollBar->setOrientation(GuiSlider::Orientation_Horizontal);
		_bottomScrollBar->setLayoutParameters(param2);
		compose(_bottomScrollBar);
	}
	else if(_scrollBar != IMP_NULL && !enable)
	{
		// decompose(&_scrollBar);
	}
}

IMPGEARS_END
