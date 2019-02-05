#include <Geometry/ProceduralGeometry.h>
#include <cmath>
#include <iostream>

// primitive declarations
struct Line
{
	imp::Vector3 p0;
	imp::Vector3 dir;
};


struct Segment
{
	void operator=(const Segment& other);
	
	imp::Vector3 p1;
	imp::Vector3 p2;
};

struct Sphere
{
	Sphere();
	Sphere(const imp::Vector3& c, double r);
	
	imp::Vector3 c;
	double			r;
};

struct Plane
{
	imp::Vector3 n;
	imp::Vector3 p0;
};

struct Triangle
{
	Triangle();
	Triangle(const imp::Vector3& p1, const imp::Vector3& p2, const imp::Vector3& p3);
	
	imp::Vector3 p1;
	imp::Vector3 p2;
	imp::Vector3 p3;
};

struct IntersectionInfo
{
    bool side1;
    bool side2;
    bool side3;
    
    IntersectionInfo()
        : side1(false)
        , side2(false)
        , side3(false)
    {}
};

    
bool contains(const Sphere& sphere, const imp::Vector3& point);

bool contains(const Sphere& sphere, const Triangle& tri);

bool isOut(const Sphere& sphere, const Triangle& tri);

bool contains(const Sphere& sphere, const imp::Vector3& p1, const imp::Vector3& p2, const imp::Vector3& p3);

bool isOut(const Sphere& sphere, const imp::Vector3& p1, const imp::Vector3& p2, const imp::Vector3& p3);

bool isOut(const Sphere& sphere, const imp::Vector3& point);


// conversion functions
void segToLine(const Segment& seg, Line& line);

void triangleToPlane(const Triangle& tri, Plane& plane);

void triangleToSeg3(const Triangle& tri, Segment& s1, Segment& s2, Segment& s3);


// intersection functions
bool iLine2(const Line& l1, const Line& l2, imp::Vector3& iPoint);

bool iSegPlane( const Segment& seg, const Plane& plane, imp::Vector3& iPoint);

// side = 0 if no side, 1 for 1, 2 for 2, 4 for 3
bool iSegTriangle( const Segment& seg, const Triangle& tri, imp::Vector3& ipoint, IntersectionInfo& iinfo);

bool iTriangle2(const Triangle& tri1, const Triangle& tri2);

bool iTriangle2(const Triangle& tri1, const Triangle& tri2, Segment& iseg);

void soloFirst(Triangle& tri1, const Triangle& tri2);

void soloFirst(Triangle& tri1, const Segment& seg);

void extendsSegInTriangle(const Triangle& tri, Segment& seg);


// vertex buffer functions
struct VertexBuffer
{
	void remove(int index, int count);

	void remove(const Triangle& triangle);
	
	void remove(const imp::Vector3& p1, const imp::Vector3& p2, const imp::Vector3& p3);

	void push(const Triangle& triangle);
	
	void push(const imp::Vector3& p1, const imp::Vector3& p2, const imp::Vector3& p3);
	
	void push(const imp::Vector3& point);
	
	std::vector<imp::Vector3> _data;
};



// volumetric intersection operator on Geometries
VertexBuffer intersect(Sphere& desc1, VertexBuffer& solid1, Sphere& desc2, VertexBuffer& solid2);

// volumetric substract operator on Geometries
VertexBuffer substract(Sphere& desc1, VertexBuffer& solid1, Sphere& desc2, VertexBuffer& solid2);

// volumetric merge operator on Geometries
VertexBuffer merge(Sphere& desc1, VertexBuffer& solid1, Sphere& desc2, VertexBuffer& solid2);


Triangle normalizeTriangle(const imp::Vector3& p1, const imp::Vector3& p2, const imp::Vector3& p3, const imp::Vector3& n);



bool contains(std::vector<int>& buff, int v);

void scissors(VertexBuffer& buffer1, VertexBuffer& buffer2);






