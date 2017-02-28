#include "Gui/Layout.h"

IMPGEARS_BEGIN

//--------------------------------------------------------------
Layout::Layout()
	: _lastFreePositionX(0.0)
	, _lastFreePositionY(0.0)
{
}
//--------------------------------------------------------------
Layout::~Layout()
{
}

//--------------------------------------------------------------
void Layout::applyParameters(const Layout::Parameters& params, Rectangle& surface)
{
	float psizeX = _parentSurface.getX2() - _parentSurface.getX1();
	float psizeY = _parentSurface.getY2() - _parentSurface.getY1();

	float sizeX = surface.getX2() - surface.getX1();
	float sizeY = surface.getY2() - surface.getY1();

	float positionX = surface.getX1();
	float positionY = surface.getY1();

	switch( params.getResizingX() )
	{
		case Resizing_Fill:
			sizeX = psizeX;
		break;
		case Resizing_Relative:
			sizeX = params.getRelativeSizeX()*psizeX;
		break;
		case Resizing_None:
			// do nothing
		break;
		default:
			// do nothing
		break;
	}

	switch( params.getResizingY() )
	{
		case Resizing_Fill:
			sizeY = psizeY;
		break;
		case Resizing_Relative:
			sizeY = params.getRelativeSizeY()*psizeY;
		break;
		case Resizing_None:
			// do nothing
		break;
		default:
			// do nothing
		break;
	}
	
	switch( params.getAlignementX() )
	{
		case Alignement_Begin:
			positionX = 0.0;
		break;
		case Alignement_End:
			positionX = psizeX-sizeX;
		break;
		case Alignement_Center:
			positionX = (psizeX-sizeX) / 2.0;
		break;
		case Alignement_None:
			// do nothing
		break;
		default:
			// do nothing
		break;
	}

	switch( params.getAlignementY() )
	{
		case Alignement_Begin:
			positionY = 0.0;
		break;
		case Alignement_End:
			positionY = psizeX-sizeY;
		break;
		case Alignement_Center:
			positionY = (psizeY-sizeY) / 2.0;
		break;
		case Alignement_None:
			// do nothing
		break;
		default:
			// do nothing
		break;
	}
	
	switch( params.getPositionningX() )
	{
		case Positionning_Auto:
			positionX = _lastFreePositionX;
			_lastFreePositionX += sizeX + 5.0;
		break;
		case Positionning_Relative:
			positionX = params.getRelativePositionX()*psizeX;
		break;
		case Positionning_None:
			// do nothing
		break;
		default:
			// do nothing
		break;
	}

	switch( params.getPositionningY() )
	{
		case Positionning_Auto:
			positionY = _lastFreePositionY;
			_lastFreePositionY += sizeY + 5.0;
		break;
		case Positionning_Relative:
			positionY = params.getRelativePositionY()*psizeY;
		break;
		case Positionning_None:
			// do nothing
		break;
		default:
			// do nothing
		break;
	}

	surface = Rectangle( positionX, positionY, positionX+sizeX, positionY+sizeY );
}

//--------------------------------------------------------------
Layout::Parameters::Parameters()
 : _alignementX(Alignement_None)
 , _alignementY(Alignement_None)
 , _positionningX(Positionning_None)
 , _positionningY(Positionning_None)
 , _resizingX(Resizing_None)
 , _resizingY(Resizing_None)
 , _positionRelativeX(0.0)
 , _positionRelativeY(0.0)
 , _sizeRelativeX(0.0)
 , _sizeRelativeY(0.0)
{
}

//--------------------------------------------------------------
Layout::Parameters::Parameters(const Parameters& other)
 : _alignementX(other._alignementX)
 , _alignementY(other._alignementY)
 , _positionningX(other._positionningX)
 , _positionningY(other._positionningY)
 , _resizingX(other._resizingX)
 , _resizingY(other._resizingY)
 , _positionRelativeX(other._positionRelativeX)
 , _positionRelativeY(other._positionRelativeY)
 , _sizeRelativeX(other._sizeRelativeX)
 , _sizeRelativeY(other._sizeRelativeY)
{
}

//--------------------------------------------------------------
Layout::Parameters::~Parameters()
{
}

//--------------------------------------------------------------
const Layout::Parameters& Layout::Parameters::Parameters::operator=(const Parameters& other)
{
	_alignementX = other._alignementX;
	_alignementY = other._alignementY;
	_positionningX = other._positionningX;
	_positionningY = other._positionningY;
	_resizingX = other._resizingX;
	_resizingY = other._resizingY;
	_positionRelativeX = other._positionRelativeX;
	_positionRelativeY = other._positionRelativeY;
	_sizeRelativeX = other._sizeRelativeX;
	_sizeRelativeY = other._sizeRelativeY;
	
	return (*this);
}

IMPGEARS_END
