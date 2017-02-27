#include "Math/Rectangle.h"

IMPGEARS_BEGIN

//--------------------------------------------------------------
Rectangle::Rectangle()
	: _x1(0.0)
	, _y1(0.0)
	, _x2(0.0)
	, _y2(0.0)
{
	
}	

//--------------------------------------------------------------
Rectangle::Rectangle(const Rectangle& other)
	: _x1(other._x1)
	, _y1(other._y1)
	, _x2(other._x2)
	, _y2(other._y2)
{

}

//--------------------------------------------------------------
Rectangle::Rectangle(float x1, float y1, float x2, float y2)
	: _x1(x1)
	, _y1(y1)
	, _x2(x2)
	, _y2(y2)
{
	
}

//--------------------------------------------------------------
Rectangle::~Rectangle()
{
	
}

//--------------------------------------------------------------
void Rectangle::copy(const Rectangle& other)
{
	_x1 = other._x1;
	_y1 = other._y1;
	_x2 = other._x2;
	_y2 = other._y2;
}

//--------------------------------------------------------------
const Rectangle& Rectangle::operator=(const Rectangle& other)
{
	copy(other);
	return *this;
}
//--------------------------------------------------------------
Rectangle Rectangle::operator+(const Rectangle& other) const
{
	return Rectangle(_x1+other._x1, _y1+other._y1, _x2+other._x2, _y2+other._y2);
}

//--------------------------------------------------------------
void Rectangle::expendBy(const Rectangle& other)
{
	float w = _x2-_x1;
	float h = _y2-_y1;
	if(w <= 0.0 || h <= 0.0)
	{
		copy(other);
	}
	else
	{
		float ow = other._x2-other._x1;
		float oh = other._y2-other._y1;
		if(ow <= 0.0 || oh <= 0.0)
		{
			_x1 = 0.0;
			_y1 = 0.0;
			_x2 = 0.0;
			_y2 = 0.0;
			return;
		}
		
		if(other._x1 < _x1)
			_x1 = other._x1;
		if(other._y1 < _y1)
			_y1 = other._y1;
		if(other._x2 > _x2)
			_x2 = other._x2;
		if(other._y2 > _y2)
			_y2 = other._y2;
	}
}

//--------------------------------------------------------------
void Rectangle::intersect(const Rectangle& other)
{
	bool disjonct = (_x2 < other._x1 || _x1 > other._x2 || _y2 < other._y1 || _y1 > other._y2);

	if(disjonct)
	{
		_x1 = 0.0;
		_y1 = 0.0;
		_x2 = 0.0;
		_y2 = 0.0;
	}
	else
	{
		if(other._x1 > _x1)
			_x1 = other._x1;
		if(other._y1 > _y1)
			_y1 = other._y1;
		if(other._x2 < _x2)
			_x2 = other._x2;
		if(other._y2 < _y2)
			_y2 = other._y2;
	}
}

//--------------------------------------------------------------
bool Rectangle::contains(float x, float y) const
{
	return _x1<=x && x<_x2 && _y1<=y && y<_y2;
}

IMPGEARS_END
