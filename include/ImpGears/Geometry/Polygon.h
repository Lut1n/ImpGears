#ifndef IMP_POLYGON_H
#define IMP_POLYGON_H

#include <Geometry/Path.h>

#include <Core/Object.h>
#include <Core/Math.h>
#include <Core/Vec3.h>

#include <vector>

IMPGEARS_BEGIN

typedef std::vector<Edge> EdgeBuffer;

class IMP_API Polygon : public Object
{
	public:
	
	Meta_Class(Polygon)
	
	Path _path;
	
	Polygon();
	Polygon(const Path& vert);
	virtual ~Polygon();
	
	void operator+=(const Vec3& mv);
	void operator-=(const Vec3& mv);
	void operator*=(const Vec3& mv);
	void operator/=(const Vec3& mv);
	void rotation(float rad);
	
	Vec3 tan(int i) const;
	Edge edge(int i) const;
	
	bool inside(const Vec3& v) const;
	bool inside(const Polygon& c) const;
	
	Vec3 gravity() const;
	
	// keep only the surrounding shape of the polygons
	Polygon boundary() const;
	Polygon simplify() const;
	
	Vec3 leftExtremity() const;
	
	Vec3 findNextByAngle(const Edge& curr, const Vec3& tangent,bool maxi=true) const;
	
	Polygon extractTriangle();
	
	std::vector<Polygon> triangulate() const;
	
	int windingNumber() const;
	void reverse();
};

IMPGEARS_END


#endif // IMP_POLYGON_H

