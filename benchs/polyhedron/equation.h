#include <Geometry/Geometry.h>
#include "Triangulator.h"

using namespace imp;

using BufV3 = std::vector<Vec3>;

float sign(float f);

struct GeoStruct
{
	BufV3 buf;
	void set(Vec3 p1);
	void set(Vec3 p1, Vec3 p2);
	void set(Vec3 p1, Vec3 p2, Vec3 p3);
	int size() const;
	Vec3& operator[](int i);
	Vec3 operator[](int i) const;
	void clear();
	void add(Vec3 p);
	Vec3 normal() const;
};

using BufGeo = std::vector<GeoStruct>;

bool rayTriangle( const GeoStruct& seg, const GeoStruct& tri, Vec3& iPoint, bool strict = true);

Vec3 getExPt(const BufV3& ph);

bool contains(const BufV3& ph, const Vec3& p);

bool rayPolyhedron(const GeoStruct& seg, const BufV3& ph, Vec3& ipt);

bool iTriangle2(const GeoStruct& t1, const GeoStruct& t2, BufV3& pts);

void iPolyhedron2(const BufV3& ph1, const BufV3& ph2, BufV3& phOut, bool invRes = false);

void diffPolyhedron2(const BufV3& ph1, const BufV3& ph2, BufV3& phOut, bool inverse);

void reducepolyh(BufV3& ph1, float f);
