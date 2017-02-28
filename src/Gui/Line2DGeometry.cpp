#include "Gui/Line2DGeometry.h"
#include "Gui/GuiState.h"

IMPGEARS_BEGIN

//--------------------------------------------------------------
Line2DGeometry::Line2DGeometry()
{
	_p1 = {0.0, 0.0};
	_p2 = {0.0, 0.0};

	update();
}
//--------------------------------------------------------------
Line2DGeometry::~Line2DGeometry()
{

}
//--------------------------------------------------------------
void Line2DGeometry::update()
{
	setPrimitive(Primitive_Lines);
	if( getVBOID() == 0 )
	{
		_vertexBuffSize= 6 * sizeof(float);

		requestVBO(_vertexBuffSize);
	}

	float x1 = _p1._x;
	float x2 = _p2._x;
	float resX = 0.0, resY = 0.0;
	GuiState::getInstance()->getResolution(resX,resY);
	float y1 = (resY-_p1._y);
	float y2 = (resY-_p2._y);

	const float vertex[6] =
	{
	  x1, y1, 0.f,
	  x2, y2, 0.f,
	};

	setVertices(vertex, _vertexBuffSize);

	_needUpdate = false;
}
//--------------------------------------------------------------
void Line2DGeometry::draw()
{
	if(_needUpdate)
		update();

	drawVBO();
}
//--------------------------------------------------------------
void Line2DGeometry::setLine(Point2d& p1, Point2d& p2)
{
	_needUpdate = p1._x != _p1._x || p1._y != _p1._y || p2._x != _p2._x || p2._y != _p2._y;
	_p1 = p1;
	_p2 = p2;
}

IMPGEARS_END
