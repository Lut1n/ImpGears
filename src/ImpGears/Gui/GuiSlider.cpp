#include "Gui/GuiSlider.h"

IMPGEARS_BEGIN

//--------------------------------------------------------------
GuiSlider::GuiSlider()
	: GuiComponent(0.0,0.0,0.0,0.0)
	, _min(0.0)
	, _max(100.0)
	, _value(0.0)
	, _orientation(Orientation_Vertical)
	, _barSize(50.0)
{
	class SliderBarBehaviour : public GuiEventHandler
	{
		public:
		SliderBarBehaviour(GuiSlider* slider)
			: _startPos(0.0)
			, _drag(false)
			, _slider(slider)
		{
		}
		~SliderBarBehaviour(){}
		
		bool onMousePressed(GuiEventSource* component, bool over, int buttonID, float x, float y, bool action)
		{
			if(over)
			{
				_drag = true;
				if(_slider->getOrientation() == GuiSlider::Orientation_Vertical)
					_startPos = y;
				else if(_slider->getOrientation() == GuiSlider::Orientation_Horizontal)
					_startPos = x;
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
			if(_drag)
			{
				if(_slider->getOrientation() == GuiSlider::Orientation_Vertical)
				{
					_slider->slideAction( y-_startPos );
					_startPos = y;
				}
				else if(_slider->getOrientation() == GuiSlider::Orientation_Horizontal)
				{
					_slider->slideAction( x-_startPos );
					_startPos = x;
				}
			}
			return false;
		}
		
		protected:
		
		float _startPos;
		bool _drag;
		GuiSlider* _slider;
	};
	
	_barButton = new GuiButton();
	_barButton->addEventHandler(new SliderBarBehaviour(this));
	//_barButton->setLayoutParameters(_barParam);
	compose(_barButton);
	
	// force update of bar params
	setOrientation(_orientation);
}

//--------------------------------------------------------------
GuiSlider::~GuiSlider()
{
}

//--------------------------------------------------------------
void GuiSlider::renderComponent(imp::Uint32 passID, float parentX, float parentY)
{
	float normalized = _value / (_max-_min);
	if(normalized < 0.0)normalized=0.0; else if(normalized>1.0)normalized=1.0;
	
	if(_orientation == Orientation_Vertical)
	{
		float barPos = normalized * ( getSizeY() - _barButton->getSizeY() );
		if(barPos < 0.0) barPos = 0.0;
		_barButton->setPosition(0.0, barPos);
	}
	else if(_orientation == Orientation_Horizontal)
	{
		float barPos = normalized * ( getSizeX() - _barButton->getSizeX() );
		if(barPos < 0.0) barPos = 0.0;
		_barButton->setPosition(barPos, 0.0);
	}
	
	GuiComponent::renderComponent(passID, parentX, parentY);
}

//--------------------------------------------------------------
void GuiSlider::slideAction(float moving)
{
	float movingRange = (_orientation == Orientation_Vertical) ? (getSizeY() - _barButton->getSizeY()) : (getSizeX() - _barButton->getSizeX());
	if(movingRange != 0.0)
	{
		if(_orientation == Orientation_Vertical)
			moving = moving/( getSizeY() - _barButton->getSizeY() );
		else if(_orientation == Orientation_Horizontal)
			moving = moving/( getSizeX() - _barButton->getSizeX() );
	}
	else
	{
		moving = 0.0;
	}
	
	_value += moving*(_max-_min);
	
	// clamp
	if(_value < _min) _value = _min;
	else if(_value > _max) _value = _max;
}

//--------------------------------------------------------------
void GuiSlider::setOrientation(Orientation ori)
{
	if(ori != _orientation)
	{
		_barButton->setPosition(0.0, 0.0);
	}
	_orientation = ori;
	
	if(_orientation == GuiSlider::Orientation_Vertical)
	{
		_barButton->setSize(10.0, _barSize);
	}
	else if(_orientation == GuiSlider::Orientation_Horizontal)
	{
		_barButton->setSize(_barSize, 10.0);
	}
}

//--------------------------------------------------------------
void GuiSlider::setRange(float minv, float maxv)
{
	float normalized = _value / (_max-_min);
	
	_min = minv;
	_max = maxv;
	
	_value = normalized * (_max-_min);
}

//--------------------------------------------------------------
void GuiSlider::resizeBar(float size)
{
	_barSize = size;
	
	setOrientation(_orientation);
}


IMPGEARS_END
