#include "Path.h"

struct ComplexPolygon
{
	enum OpType
	{
		OpType_OR,
		OpType_AND,
		OpType_NOT
	};
	
	enum Orientation
	{
		Orientation_NONE,
		Orientation_POS,
		Orientation_NEG
	};
	
	using PathSet = std::vector<Path>;
	
	PathSet _cycles;
	
	ComplexPolygon(const Path& basic);
	ComplexPolygon(const PathSet& bound);
	ComplexPolygon(const PathSet& bound, const PathSet& hole);
	ComplexPolygon();

	ComplexPolygon resolveHoles() const;
	static PathSet breakHoles(const Path& body, const Path& hole,const PathSet& set2test);
	void addPath(const Path& cy, Orientation=Orientation_NONE);
	void addPaths(const PathSet& other);
	
	static bool contains(const PathSet& cycles, const Vec3& v);
	
	static ComplexPolygon applyOp(const Path& cycleA, const Path& cycleB, OpType op);
	
	ComplexPolygon operator-(const ComplexPolygon& other) const;
	ComplexPolygon operator+(const ComplexPolygon& other) const;
	ComplexPolygon operator*(const ComplexPolygon& other) const;
};


