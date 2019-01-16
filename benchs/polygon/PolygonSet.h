#include "Cycle.h"

struct PolygonSet
{
	enum OpType
	{
		OpType_OR,
		OpType_AND,
		OpType_NOT
	};
	
	using CycleSet = std::vector<Cycle>;
	
	CycleSet _cycles;
	
	PolygonSet(const Cycle& basic);
	PolygonSet(const CycleSet& bound);
	PolygonSet(const CycleSet& bound, const CycleSet& hole);
	PolygonSet();

	void resolveHoles();
	void breakHoles(int exti, int inti);
	void addCycle(const Cycle& cy, bool hole=false);
	void addCycles(const CycleSet& other);
	
	static bool contains(const CycleSet& cycles, const Vec3& v);
	
	static PolygonSet applyOp(const Cycle& cycleA, const Cycle& cycleB, OpType op);
	
	PolygonSet operator-(const PolygonSet& other) const;
	PolygonSet operator+(const PolygonSet& other) const;
	PolygonSet operator*(const PolygonSet& other) const;
};


