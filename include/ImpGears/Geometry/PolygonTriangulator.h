#ifndef IMP_POLYGON_TRIANGULATOR_H
#define IMP_POLYGON_TRIANGULATOR_H

#include <Geometry/Polygon.h>

#include <Core/Object.h>

IMPGEARS_BEGIN

class IMP_API PolygonTriangulator
{
	public:
	
	void triangulate(Polygon& polygon, std::vector<Polygon>& TriangleSet);
};

IMPGEARS_END

#endif // IMP_POLYGON_TRIANGULATOR_H