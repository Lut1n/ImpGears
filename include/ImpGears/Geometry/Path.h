#ifndef IMP_PATH_H
#define IMP_PATH_H

#include <Core/Vec3.h>
#include <Core/Vec4.h>
#include <Core/Math.h>
#include <sstream>

IMPGEARS_BEGIN

//--------------------------------------------------------------
class IMP_API Edge : public Object
{
	public:
	
    Vec3 _p1, _p2;
	
	Meta_Class(Edge)
	
	Edge();
	
	Edge(const Edge& other);
	
	Edge(const Vec3& p1, const Vec3& p2);
	
	bool operator==(const Edge& other) const;
	bool connectedTo(const Edge& other) const;
};

//--------------------------------------------------------------
class IMP_API Path : public Object
{
	public:
	
	Meta_Class(Path)
	
	using BufType = std::vector<Vec3>;
	BufType vertices;
	
	Path();
	Path(const BufType& vert);
	virtual ~Path();
	
	void addVertex(Vec3 vt);
	void addVertices(const BufType& toInsert);
	void addFrom(const Path& cycle);
	Path subPath(int from, int to, bool reverse = false) const;
	int cycleIndex(int i) const;
	Vec3& vertex(int i);
	const Vec3& vertex(int i) const;
	int index(const Vec3& v) const;
	Vec3 tan(int i) const;
	Edge edge(int i) const;
	int count() const;
	void dump() const;
	
	bool composes(const Vec3& v) const;
	BufType getConnexes(const Vec3& v) const;
	int getEdgeCnt(const Vec3& v) const;
	Vec3 previous(const Vec3& v) const;
	Vec3 next(const Vec3& v) const;
	Vec3 normal() const;
	
	Vec3 gravity() const;
	std::vector<int> degrees() const;
	bool areConnected(const Vec3& v1, const Vec3& v2) const;
	bool areConnected(const Vec3& v1, const Vec3& v2, const Vec3& v3) const;
	
	void erase(std::vector<int> toErase);
	
	// keep only the surrounding shape of the polygons
	Path boundary() const;
	Path simplify() const;
	
	Vec3 leftExtremity() const;
	
	Vec3 findNextByAngle(const Edge& curr, const Vec3& tangent,bool maxi=true) const;
	
	Path extractTriangle();
	
	std::vector<Path> triangulate() const;
	
	int windingNumber() const;
	void reverse();
	
	bool inside(const Vec3& v) const;
	bool inside(const Path& c) const;
	
	const BufType& data() const;
	BufType& data();
	
	Vec3& operator[](int i);
	Vec3 operator[](int i) const;
	
	static Path circle(int sub, float radius);
};

//--------------------------------------------------------------
class IMP_API Intersection : public Object
{
	public:
	
	Meta_Class(Intersection)
		
	using BufType = std::vector<Vec3>;
	using Cache = std::vector<Intersection>;

	Edge edge[2];
	Vec3 refNormal;
	Vec3 ipoint;
	Intersection();
	Intersection(Edge e1, Edge e2);
	bool operator==(Intersection other);
	bool compute();
	void setNormal(const Vec3& n);
	
	static BufType getVertices(const Cache& cache);
	
	static bool resolve(Path& target, const Path& other, Cache& precomputed);
	static bool resolve2(Path& cy1, Path& cy2, Cache& precomputed);
	static bool selfResolve(Path& cy, Cache& precomputed);
	
	static bool resolve(Path& target, const Path& other);
	static bool resolve2(Path& cy1, Path& cy2);
	static bool selfResolve(Path& cy);

	static bool isCrossing(const Path& target, const Edge& e, bool excludeNode=true);
	
	static bool contains(const Cache& cache, const Vec3& v);
};

IMPGEARS_END

#endif //IMP_PATH_H
