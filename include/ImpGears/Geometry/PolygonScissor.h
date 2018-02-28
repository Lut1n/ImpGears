#ifndef IMP_POLYGON_SCISSOR_H
#define IMP_POLYGON_SCISSOR_H

#include <Geometry/Polygon.h>

#include <Core/Object.h>

IMPGEARS_BEGIN

class IMP_API PolygonScissor : public Object
{
    public:

	Meta_Class(PolygonScissor)
	
    Polygon substract(Polygon& solid1, Polygon& solid2);
	
    Polygon merge(Polygon& solid1, Polygon& solid2);
	
    Polygon intersect(Polygon& solid1, Polygon& solid2);

};

IMPGEARS_END

#endif // IMP_POLYGON_SCISSOR_H