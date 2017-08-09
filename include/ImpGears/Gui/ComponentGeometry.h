#ifndef IMP_GUI_GEOMETRY_H
#define IMP_GUI_GEOMETRY_H

#include "Core/impBase.h"
#include <SceneGraph/VBOData.h>
#include <Core/Rectangle.h>

IMPGEARS_BEGIN

class IMP_API ComponentGeometry : public VBOData
{
	public:

	ComponentGeometry();
	
	virtual ~ComponentGeometry();
	
	void draw();
	void update();
	
	void resize(float sx, float sy);
	void setPosition(float px, float py);

	void setClippingRect(const Rectangle& clippingRect);
	const Rectangle& getClippingRect() const;

	const Rectangle& getDimension() const {return _dimension;}

	protected:

	Uint64		_vertexBuffSize;
	bool		_needUpdate;

	Rectangle	_dimension;
	Rectangle	_clippingRect;

};


IMPGEARS_END

#endif // IMP_GUI_GEOMETRY_H
