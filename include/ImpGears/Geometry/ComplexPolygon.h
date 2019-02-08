#ifndef IMP_COMPLEXPOLYGON_H
#define IMP_COMPLEXPOLYGON_H

#include <Geometry/Path.h>

IMPGEARS_BEGIN

class IMP_API ComplexPolygon : public Object
{
	public:
	
	Meta_Class(ComplexPolygon)
	
	enum OpType
	{
		OpType_OR,
		OpType_AND,
		OpType_A_NOT_B,
		OpType_B_NOT_A
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
	
	void operator+=(const Vec3& mv);
	void operator-=(const Vec3& mv);
	void operator*=(const Vec3& mv);
	void operator/=(const Vec3& mv);
	void rotation(float rad);

	ComplexPolygon resolveHoles() const;
	static PathSet breakHoles(const Path& body, const Path& hole,const PathSet& set2test);
	void addPath(const Path& cy, Orientation=Orientation_NONE);
	void addPaths(const PathSet& other);
	
	static bool contains(const PathSet& cycles, const Vec3& v);
	
	static ComplexPolygon applyOp(const Path& cycleA, const Path& cycleB, OpType op);
	static ComplexPolygon applyOp2(const PathSet& setA, const PathSet& setB, OpType op);
	
	ComplexPolygon operator-(const ComplexPolygon& other) const;
	ComplexPolygon operator+(const ComplexPolygon& other) const;
	ComplexPolygon operator*(const ComplexPolygon& other) const;
};

IMPGEARS_END

#endif // IMP_COMPLEXPOLYGON_H
