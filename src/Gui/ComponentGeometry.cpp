#include <Gui/ComponentGeometry.h>
#include <Gui/GuiState.h>

IMPGEARS_BEGIN

//--------------------------------------------------------------
ComponentGeometry::ComponentGeometry()
	: _vertexBuffSize(0)
	, _needUpdate(true)
{
	float resX = 0.0, resY = 0.0;
	GuiState::getInstance()->getResolution(resX, resY);
	_clippingRect = Rectangle(0.0,0.0,resX, resY);
	update();
}

//--------------------------------------------------------------
ComponentGeometry::~ComponentGeometry(){}

//--------------------------------------------------------------
void ComponentGeometry::draw()
{
	if(_needUpdate)
		update();
	
	drawVBO();
}

//--------------------------------------------------------------
void ComponentGeometry::update()
{
	setPrimitive(Primitive_Quads);
	Uint32 texCoordSize = 8*sizeof(float);
	if( getVBOID() == 0 )
	{
		_vertexBuffSize= 12*sizeof(float);
		requestVBO(_vertexBuffSize+texCoordSize);
	}
	
	float texX1 = 0.0;
	float texX2 = 1.0;
	float texY1 = 0.0;
	float texY2 = 1.0;
	
	float x1 = _dimension.getX1();
	float x2 = _dimension.getX2();
	float y1 = _dimension.getY1();
	float y2 = _dimension.getY2();
	
	// clipping test
	{
		float w = x2-x1;
		float h = y2-y1;
		if(x1 < _clippingRect.getX1())
		{
			texX1 = (_clippingRect.getX1()-x1)/w;
			x1 = _clippingRect.getX1();
		}
		if(x2 > _clippingRect.getX2())
		{
			texX2 = 1.0-(x2-_clippingRect.getX2())/w;
			x2 = _clippingRect.getX2();
		}	
		if(y1 < _clippingRect.getY1())
		{
			texY2 = 1.0-(_clippingRect.getY1()-y1)/h;
			y1 = _clippingRect.getY1();
		}
		if(y2 > _clippingRect.getY2())
		{
			texY1 = (y2-_clippingRect.getY2())/h;
			y2 = _clippingRect.getY2();
		}
	}

	const float texCoord[8] =
	{
	  texX1, texY2,
	  texX1, texY1,
	  texX2, texY1,
	  texX2, texY2
	};
	
	float resX = 0.0, resY = 0.0;
	GuiState::getInstance()->getResolution(resX, resY);
	y1 = (resY-y1);
	y2 = (resY-y2);
	
	const float vertex[12] =
	{
	  x1, y1, 0.f,
	  x1, y2, 0.f,
	  x2, y2, 0.f,
	  x2, y1, 0.f
	};
	
	setData(texCoord, texCoordSize, _vertexBuffSize);
	setVertices(vertex, _vertexBuffSize);
	_needUpdate = false;
}

//--------------------------------------------------------------
void ComponentGeometry::resize(float sx, float sy)
{
	float exw = _dimension.getX2() - _dimension.getX1() ;
	float exh = _dimension.getY2() - _dimension.getY1() ;

	if(sx != exw || sy != exh)
	{
		_dimension = Rectangle(
			_dimension.getX1(),
			_dimension.getY1(),
			_dimension.getX1()+sx,
			_dimension.getY1()+sy);
	
		_needUpdate = true;
	}
}

//--------------------------------------------------------------
void ComponentGeometry::setPosition(float px, float py)
{

	if(px != _dimension.getX1() || py != _dimension.getY1())
	{
		float w = _dimension.getX2() - _dimension.getX1();
		float h = _dimension.getY2() - _dimension.getY1();
	
		_dimension = Rectangle(px, py, px+w, py+h);
	
		_needUpdate = true;
	}
}

//--------------------------------------------------------------	
void ComponentGeometry::setClippingRect(const Rectangle& clippingRect)
{
	if(_clippingRect.getX1() != clippingRect.getX1()
		|| _clippingRect.getX2() != clippingRect.getX2()
		|| _clippingRect.getY1() != clippingRect.getY1()
		|| _clippingRect.getY2() != clippingRect.getY2())
	{
		_clippingRect = clippingRect;
		_needUpdate = true;
	}
}

//--------------------------------------------------------------
const Rectangle& ComponentGeometry::getClippingRect() const
{
	return _clippingRect;
}

IMPGEARS_END
