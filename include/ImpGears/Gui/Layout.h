#ifndef IMP_LAYOUT_STATE_H
#define IMP_LAYOUT_STATE_H

#include "Core/impBase.h"
#include "Math/Rectangle.h"

IMPGEARS_BEGIN

class IMP_API Layout
{
    public:


	
	enum Alignement
	{
		Alignement_None = 0,
		Alignement_Begin,
		Alignement_Center,
		Alignement_End,
	};
	
	enum Positionning
	{
		Positionning_None = 0,
		Positionning_Auto,
		Positionning_Relative
	};
	
	enum Resizing
	{
		Resizing_None = 0,
		Resizing_Fill,
		Resizing_Relative
	};



	//--------------------------------------------------------------
	class Parameters
	{	
		public:
	
		Parameters();
		Parameters(const Parameters& other);
	
		virtual ~Parameters();
	
		const Parameters& operator=(const Parameters& other);
	
		void setAlignementX(Alignement alignement) { _alignementX = alignement; }
		Alignement getAlignementX() const {return _alignementX;}

		void setAlignementY(Alignement alignement) { _alignementY = alignement; }
		Alignement getAlignementY() const {return _alignementY;}

		void setPositionningX(Positionning positionning, float prx = 0.0) { _positionningX = positionning; _positionRelativeX=prx; }
		Positionning getPositionningX() const {return _positionningX;}

		void setPositionningY(Positionning positionning, float pry = 0.0) { _positionningY = positionning; _positionRelativeY=pry; }
		Positionning getPositionningY() const {return _positionningY;}
	
		void setResizingX(Resizing resizing, float rrx = 0.0) { _resizingX = resizing; _sizeRelativeX=rrx; }
		Resizing getResizingX() const {return _resizingX;}

		void setResizingY(Resizing resizing, float rry = 0.0) { _resizingY = resizing; _sizeRelativeY=rry; }
		Resizing getResizingY() const {return _resizingY;}
	
		float getRelativePositionX() const {return _positionRelativeX;}
		float getRelativePositionY() const {return _positionRelativeY;}
	
		float getRelativeSizeX() const {return _sizeRelativeX;}
		float getRelativeSizeY() const {return _sizeRelativeY;}
	
		protected:
	
		Alignement _alignementX;
		Alignement _alignementY;

		Positionning _positionningX;
		Positionning _positionningY;

		Resizing _resizingX;
		Resizing _resizingY;

		float _positionRelativeX;
		float _positionRelativeY;

		float _sizeRelativeX;
		float _sizeRelativeY;
	};

	Layout();
	virtual ~Layout();
	
	const Rectangle& getParentSurface() const {return _parentSurface;}
	void setParentSurface(const Rectangle& psurface) { _parentSurface = psurface; }

	float getFreePositionX() { return _lastFreePositionX++; }
	float getFreePositionY() { return _lastFreePositionY++; }

	void resetFreePosition() { _lastFreePositionX=0.0; _lastFreePositionY=0.0; }

	virtual void applyParameters(const Parameters& params, Rectangle& surface);

    protected:
	
	Rectangle _parentSurface;

	float _sizeX;
	float _sizeY;
	
	float _lastFreePositionX;
	float _lastFreePositionY;
};


IMPGEARS_END

#endif // IMP_LAYOUT_STATE_H
