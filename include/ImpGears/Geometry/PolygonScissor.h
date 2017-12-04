#ifndef IMP_POLYGON_SCISSOR_H
#define IMP_POLYGON_SCISSOR_H

#include <Geometry/Polygon.h>

#include "Core/impBase.h"

IMPGEARS_BEGIN

class IMP_API PolygonScissor
{
    public:

    // volumetric substract operator on Geometries
    Polygon substract(Polygon& solid1, Polygon& solid2);

};

IMPGEARS_END

#endif // IMP_POLYGON_SCISSOR_H