#ifndef cycle_def
#define cycle_def

#include <Core/Vec3.h>
#include <Core/Vec4.h>
#include <Core/Math.h>
#include <sstream>

#include "Path.h"


using Vec3 = imp::Vec3;
using Vec4 = imp::Vec4;
using Vertices = std::vector<Vec3>;

struct SimplePolygon
{
	Path _path;
	
	SimplePolygon();
	SimplePolygon(const Path& vert);
	virtual ~SimplePolygon();
	
	void operator+=(const Vec3& mv);
	void operator-=(const Vec3& mv);
	void operator*=(const Vec3& mv);
	void operator/=(const Vec3& mv);
	void rotation(float rad);
	
	Vec3 tan(int i) const;
	Edge edge(int i) const;
	
	bool contains(const Vec3& v) const;
	bool contains(const SimplePolygon& c) const;
	
	Vec3 gravity() const;
	
	// keep only the surrounding shape of the polygons
	SimplePolygon boundary() const;
	SimplePolygon simplify() const;
	
	Vec3 leftExtremity() const;
	
	Vec3 findNextByAngle(const Edge& curr, const Vec3& tangent,bool maxi=true) const;
	
	SimplePolygon extractTriangle();
	
	std::vector<SimplePolygon> triangulate() const;
	
	int windingNumber() const;
	void reverse();
};

#endif //cycle_def
