#ifndef cycle_def
#define cycle_def

#include <Core/Vec3.h>
#include <Core/Vec4.h>
#include <Core/Math.h>
#include <sstream>


using Vec3 = imp::Vec3;
using Vec4 = imp::Vec4;
using Vertices = std::vector<Vec3>;

std::string asstring(const Vec3& v);

struct Edge
{
	Vec3 p1,p2;
	Edge();
	Edge(Vec3 p1,Vec3 p2);
	bool operator==(const Edge& other);
	bool connectedTo(const Edge& other);
};

struct Cycle
{
	Vertices vertices;
	
	Cycle();
	Cycle(const Vertices& vert);
	virtual ~Cycle();
	
	void operator+=(Vec3 mv);
	void operator-=(Vec3 mv);
	void operator*=(Vec3 mv);
	void operator/=(Vec3 mv);
	void rotation(float rad);
	
	void addVertex(Vec3 vt);
	void addVertices(Vertices toInsert);
	void addFrom(const Cycle& other, int a, int b, int dir = 1);
	int cycleIndex(int i) const;
	Vec3& vertex(int i);
	const Vec3& vertex(int i) const;
	int index(const Vec3& v) const;
	Vec3 tan(int i) const;
	Edge edge(int i) const;
	int count() const;
	void dump() const;
	
	bool composes(const Vec3& v) const;
	bool contains(const Vec3& v) const;
	bool contains(const Cycle& c) const;
	Vertices getConnexes(const Vec3& v) const;
	int getEdgeCnt(const Vec3& v) const;
	Vec3 previous(const Vec3& v) const;
	
	Vec3 gravity() const;
	std::vector<int> degrees() const;
	bool areConnected(const Vec3& v1, const Vec3& v2) const;
	bool areConnected(const Vec3& v1, const Vec3& v2, const Vec3& v3) const;
	
	void erase(std::vector<int> toErase);
	
	// keep only the surrounding shape of the polygons
	Cycle boundary() const;
	Cycle simplify() const;
	
	Vec3 leftExtremity() const;
	
	Vec3 findNextByAngle(const Edge& curr, const Vec3& tangent,bool maxi=true) const;
	
	Cycle extractTriangle();
	
	std::vector<Cycle> triangulate() const;
	
};

struct Intersection
{
	using Cache = std::vector<Intersection>;

	Edge edge[2];
	Vec3 ipoint;
	Intersection();
	Intersection(Edge e1, Edge e2);
	bool operator==(Intersection other);
	bool compute();
	
	static bool resolve(Cycle& target, const Cycle& other, Cache& precomputed);
	static bool resolve2(Cycle& cy1, Cycle& cy2, Cache& precomputed);
	static bool selfResolve(Cycle& cy, Cache& precomputed);
	
	static bool resolve(Cycle& target, const Cycle& other);
	static bool resolve2(Cycle& cy1, Cycle& cy2);
	static bool selfResolve(Cycle& cy);

	static bool isCrossing(const Cycle& target, const Edge& e, bool excludeNode=true);
	
	static bool contains(const Cache& cache, const Vec3& v);
};

#endif //cycle_def